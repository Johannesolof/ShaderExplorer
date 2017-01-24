#include "Model.h"

#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include <tiny_obj_loader.h>
//#include <experimental/tinyobj_loader_opt.h>
#include <algorithm>
#include <sstream>
#include <iomanip> 
#include <GL/glew.h>
// STB_IMAGE for loading images of many filetypes
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


Texture Model::LoadTexture(const std::string & _filename, int _components) {
	filename_ = _filename;
	Texture texture;
	texture.valid = true;
	int components;
	texture.data = stbi_load(filename_.c_str(), &texture.width, &texture.height, &components, _components);
	if (texture.data == nullptr) {
		std::cout << "ERROR: LoadModelFromOBJ(): Failed to load texture: " << filename_ << "\n";
		exit(1);
	}
	glGenTextures(1, &texture.gl_id);
	glBindTexture(GL_TEXTURE_2D, texture.gl_id);
	GLenum format, internal_format;
	if (_components == 1) { format = GL_R;  internal_format = GL_R8; }
	else if (_components == 3) { format = GL_RGB; internal_format = GL_SRGB; }
	else if (_components == 4) { format = GL_RGBA;  internal_format = GL_SRGB_ALPHA; }
	else {
		std::cout << "Texture loading not implemented for this number of compenents.\n";
		exit(1);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);
	return texture;
}

///////////////////////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////////////////////
Model::~Model()
{
	for (auto & material : materials_) {
		if (material.color_texture.valid) glDeleteTextures(1, &material.color_texture.gl_id);
		if (material.reflectivity_texture.valid) glDeleteTextures(1, &material.reflectivity_texture.gl_id);
		if (material.shininess_texture.valid) glDeleteTextures(1, &material.shininess_texture.gl_id);
		if (material.metalness_texture.valid) glDeleteTextures(1, &material.metalness_texture.gl_id);
		if (material.fresnel_texture.valid) glDeleteTextures(1, &material.fresnel_texture.gl_id);
		if (material.emission_texture.valid) glDeleteTextures(1, &material.emission_texture.gl_id);
	}
	glDeleteBuffers(1, &positions_bo_);
	glDeleteBuffers(1, &normals_bo_);
	glDeleteBuffers(1, &texture_coordinates_bo_);
}

void Model::LoadModelFromOBJ(std::string path)
{
	///////////////////////////////////////////////////////////////////////
	// Separate filename into directory, base filename and extension
	// NOTE: This can be made a LOT simpler as soon as compilers properly 
	//		 support std::filesystem (C++17)
	///////////////////////////////////////////////////////////////////////
	size_t separator = path.find_last_of("\\/");
	std::string filename, extension, directory;
	if (separator != std::string::npos) {
		filename = path.substr(separator + 1, path.size() - separator - 1);
		directory = path.substr(0, separator + 1);
	}
	else {
		filename = path;
		directory = "./";
	}
	separator = filename.find_last_of(".");
	if (separator == std::string::npos) {
		std::cout << "Fatal: LoadModelFromOBJ(): Expecting filename ending in '.obj'\n";
		exit(1);
	}
	extension = filename.substr(separator, filename.size() - separator);
	filename = filename.substr(0, separator);

	///////////////////////////////////////////////////////////////////////
	// Parse the OBJ file using tinyobj
	///////////////////////////////////////////////////////////////////////
	std::cout << "Loading " << path << "..." << std::flush;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	// Expect '.mtl' file in the same directory and triangulate meshes 
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err,
		(directory + filename + extension).c_str(), directory.c_str(),
		true);
	if (!err.empty()) { // `err` may contain warning message.
		std::cerr << err << std::endl;
	}
	if (!ret) { exit(1); }
	//Model * model_ = new Model;
	this->name_ = filename;
	this->filename_ = path;

	///////////////////////////////////////////////////////////////////////
	// Transform all materials into our datastructure
	///////////////////////////////////////////////////////////////////////
	for (const auto & m : materials) {
		Material material;
		material.name = m.name;
		material.color = glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
		if (m.diffuse_texname != "") {
			material.color_texture = LoadTexture(directory + m.diffuse_texname, 4);
		}
		material.reflectivity = m.specular[0];
		if (m.specular_texname != "") {
			material.reflectivity_texture = LoadTexture(directory + m.specular_texname, 1);
		}
		material.metalness = m.metallic;
		if (m.metallic_texname != "") {
			material.metalness_texture = LoadTexture(directory + m.metallic_texname, 1);
		}
		material.fresnel = m.sheen;
		if (m.sheen_texname != "") {
			material.fresnel_texture = LoadTexture(directory + m.sheen_texname, 1);
		}
		material.shininess = m.roughness;
		if (m.roughness_texname != "") {
			material.fresnel_texture = LoadTexture(directory + m.sheen_texname, 1);
		}
		material.emission = m.emission[0];
		if (m.emissive_texname != "") {
			material.emission_texture = LoadTexture(directory + m.emissive_texname, 4);
		}
		material.transparency = m.transmittance[0];
		this->materials_.push_back(material);
	}

	///////////////////////////////////////////////////////////////////////
	// A vertex in the OBJ file may have different indices for position, 
	// normal and texture coordinate. We will not even attempt to use 
	// indexed lookups, but will store a simple vertex stream per mesh. 
	///////////////////////////////////////////////////////////////////////
	size_t number_of_vertices = 0;
	for (const auto & shape : shapes) {
		number_of_vertices += shape.mesh.indices.size();
	}
	this->positions_.resize(number_of_vertices);
	this->normals_.resize(number_of_vertices);
	this->texture_coordinates_.resize(number_of_vertices);

	///////////////////////////////////////////////////////////////////////
	// For each vertex _position_ auto generate a normal that will be used
	// if no normal is supplied. 
	///////////////////////////////////////////////////////////////////////
	std::vector<glm::vec4> auto_normals(attrib.vertices.size() / 3);
	for (const auto & shape : shapes) {
		for (int face = 0; face < int(shape.mesh.indices.size()) / 3; face++) {

			glm::vec3 v0 = glm::vec3(
				attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 0],
				attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 1],
				attrib.vertices[shape.mesh.indices[face * 3 + 0].vertex_index * 3 + 2]
			);
			glm::vec3 v1 = glm::vec3(
				attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 0],
				attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 1],
				attrib.vertices[shape.mesh.indices[face * 3 + 1].vertex_index * 3 + 2]
			);
			glm::vec3 v2 = glm::vec3(
				attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 0],
				attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 1],
				attrib.vertices[shape.mesh.indices[face * 3 + 2].vertex_index * 3 + 2]
			);

			glm::vec3 e0 = glm::normalize(v1 - v0);
			glm::vec3 e1 = glm::normalize(v2 - v0);
			glm::vec3 face_normal = cross(e0, e1);

			auto_normals[shape.mesh.indices[face * 3 + 0].vertex_index] += glm::vec4(face_normal, 1.0f);
			auto_normals[shape.mesh.indices[face * 3 + 1].vertex_index] += glm::vec4(face_normal, 1.0f);
			auto_normals[shape.mesh.indices[face * 3 + 2].vertex_index] += glm::vec4(face_normal, 1.0f);
		}
	}
	for (auto & normal : auto_normals) {
		normal = (1.0f / normal.w) * normal;
	}

	///////////////////////////////////////////////////////////////////////
	// Now we will turn all shapes into Meshes. A shape that has several 
	// materials will be split into several meshes with unique names
	///////////////////////////////////////////////////////////////////////
	int vertices_so_far = 0;
	for (const auto & shape : shapes)
	{
		///////////////////////////////////////////////////////////////////
		// The shapes in an OBJ file may several different materials. 
		// If so, we will split the shape into one Mesh per Material
		///////////////////////////////////////////////////////////////////
		int next_material_index = shape.mesh.material_ids[0];
		int next_material_starting_face = 0;
		std::vector<bool> finished_materials(materials.size(), false);
		int number_of_materials_in_shape = 0;
		while (next_material_index != -1)
		{
			int current_material_index = next_material_index;
			int current_material_starting_face = next_material_starting_face;
			next_material_index = -1;
			next_material_starting_face = -1;
			// Process a new Mesh with a unique material
			Mesh mesh;
			mesh.name = shape.name + "_" + materials[current_material_index].name;
			mesh.material_idx = current_material_index;
			mesh.start_index = vertices_so_far;
			number_of_materials_in_shape += 1;

			size_t number_of_faces = shape.mesh.indices.size() / 3;
			for (int i = current_material_starting_face; i < number_of_faces; i++)
			{
				if (shape.mesh.material_ids[i] != current_material_index) {
					if (next_material_index >= 0) continue;
					if (finished_materials[shape.mesh.material_ids[i]]) continue;
					// Found a new material that we have not processed.
					next_material_index = shape.mesh.material_ids[i];
					next_material_starting_face = i;
				}
				else {
					///////////////////////////////////////////////////////
					// Now we generate the vertices
					///////////////////////////////////////////////////////
					for (int j = 0; j < 3; j++) {
						int v = shape.mesh.indices[i * 3 + j].vertex_index;
						this->positions_[vertices_so_far + j] = glm::vec3(
							attrib.vertices[v * 3 + 0],
							attrib.vertices[v * 3 + 1],
							attrib.vertices[v * 3 + 2]);
						if (shape.mesh.indices[i * 3 + j].normal_index == -1) {
							// No normal, use the autogenerated
							this->normals_[vertices_so_far + j] = glm::vec3(auto_normals[v]);
						}
						else {
							this->normals_[vertices_so_far + j] = glm::vec3(
								attrib.normals[v * 3 + 0],
								attrib.normals[v * 3 + 1],
								attrib.normals[v * 3 + 2]);
						}
						if (shape.mesh.indices[i * 3 + j].texcoord_index == -1) {
							// No UV coordinates. Use null. 
							this->texture_coordinates_[vertices_so_far + j] = glm::vec2(0.0f);
						}
						else {
							this->texture_coordinates_[vertices_so_far + j] = glm::vec2(
								attrib.texcoords[shape.mesh.indices[i * 3 + j].texcoord_index * 2 + 0],
								attrib.texcoords[shape.mesh.indices[i * 3 + j].texcoord_index * 2 + 1]);
						}
					}
					vertices_so_far += 3;
				}
			}
			///////////////////////////////////////////////////////////////
			// Finalize and push this mesh to the list
			///////////////////////////////////////////////////////////////
			mesh.number_of_vertices = vertices_so_far - mesh.start_index;
			this->meshes_.push_back(mesh);
			finished_materials[current_material_index] = true;
		}
		if (number_of_materials_in_shape == 1) {
			this->meshes_.back().name = shape.name;
		}
	}

	///////////////////////////////////////////////////////////////////////
	// Upload to GPU
	///////////////////////////////////////////////////////////////////////
	glGenVertexArrays(1, &this->vaob_);
	glBindVertexArray(this->vaob_);
	glGenBuffers(1, &this->positions_bo_);
	glBindBuffer(GL_ARRAY_BUFFER, this->positions_bo_);
	glBufferData(GL_ARRAY_BUFFER, this->positions_.size() * sizeof(glm::vec3),
		&this->positions_[0].x, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(0);
	glGenBuffers(1, &this->normals_bo_);
	glBindBuffer(GL_ARRAY_BUFFER, this->normals_bo_);
	glBufferData(GL_ARRAY_BUFFER, this->normals_.size() * sizeof(glm::vec3),
		&this->normals_[0].x, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &this->texture_coordinates_bo_);
	glBindBuffer(GL_ARRAY_BUFFER, this->texture_coordinates_bo_);
	glBufferData(GL_ARRAY_BUFFER, this->texture_coordinates_.size() * sizeof(glm::vec2),
		&this->texture_coordinates_[0].x, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, 0, nullptr);
	glEnableVertexAttribArray(2);

	std::cout << "done.\n";
}

///////////////////////////////////////////////////////////////////////
// Loop through all Meshes in the Model and Render them
///////////////////////////////////////////////////////////////////////
void Model::Render(const bool submitMaterials)
{
	glBindVertexArray(vaob_);
	for (auto & mesh : meshes_)
	{
		if (submitMaterials) {
			const Material & material = materials_[mesh.material_idx];

			bool has_color_texture = material.color_texture.valid;
			bool has_reflectivity_texture = material.reflectivity_texture.valid;
			bool has_metalness_texture = material.metalness_texture.valid;
			bool has_fresnel_texture = material.fresnel_texture.valid;
			bool has_shininess_texture = material.shininess_texture.valid;
			bool has_emission_texture = material.emission_texture.valid;
			if (has_color_texture) glBindTextures(0, 1, &material.color_texture.gl_id);
			if (has_reflectivity_texture) glBindTextures(1, 1, &material.reflectivity_texture.gl_id);
			if (has_metalness_texture) glBindTextures(2, 1, &material.metalness_texture.gl_id);
			if (has_fresnel_texture) glBindTextures(3, 1, &material.fresnel_texture.gl_id);
			if (has_shininess_texture) glBindTextures(4, 1, &material.shininess_texture.gl_id);
			if (has_emission_texture) glBindTextures(5, 1, &material.emission_texture.gl_id);
			GLint current_program = 0;
			glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
			glUniform1i(glGetUniformLocation(current_program, "has_color_texture"), has_color_texture);
			glUniform1i(glGetUniformLocation(current_program, "has_diffuse_texture"), has_color_texture ? 1 : 0); // FIXME
			glUniform1i(glGetUniformLocation(current_program, "has_reflectivity_texture"), has_reflectivity_texture);
			glUniform1i(glGetUniformLocation(current_program, "has_metalness_texture"), has_metalness_texture);
			glUniform1i(glGetUniformLocation(current_program, "has_fresnel_texture"), has_fresnel_texture);
			glUniform1i(glGetUniformLocation(current_program, "has_shininess_texture"), has_shininess_texture);
			glUniform1i(glGetUniformLocation(current_program, "has_emission_texture"), has_emission_texture);
			glUniform3fv(glGetUniformLocation(current_program, "material_color"), 1, &material.color.x);
			glUniform3fv(glGetUniformLocation(current_program, "material_diffuse_color"), 1, &material.color.x); //FIXME: Compatibility with old shading model_ of lab3.
			glUniform3fv(glGetUniformLocation(current_program, "material_emissive_color"), 1, &material.color.x); //FIXME: Compatibility with old shading model_ of lab3.
			glUniform1i(glGetUniformLocation(current_program, "has_diffuse_texture"), has_color_texture);//FIXME: Compatibility with old shading model_ of lab3.
			glUniform1fv(glGetUniformLocation(current_program, "material_reflectivity"), 1, &material.reflectivity);
			glUniform1fv(glGetUniformLocation(current_program, "material_metalness"), 1, &material.metalness);
			glUniform1fv(glGetUniformLocation(current_program, "material_fresnel"), 1, &material.fresnel);
			glUniform1fv(glGetUniformLocation(current_program, "material_shininess"), 1, &material.shininess);
			glUniform1fv(glGetUniformLocation(current_program, "material_emission"), 1, &material.emission);
		}
		glDrawArrays(GL_TRIANGLES, mesh.start_index, static_cast<GLsizei>(mesh.number_of_vertices));
	}
}
