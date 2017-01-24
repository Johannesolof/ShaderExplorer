#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Texture {
	bool valid = false;
	uint32_t gl_id;
	std::string filename;
	int width, height;
	uint8_t * data;
};

struct Material
{
	std::string name;
	glm::vec3	color;
	float		reflectivity;
	float		shininess;
	float		metalness;
	float		fresnel;
	float		emission;
	float		transparency;
	Texture color_texture;
	Texture	reflectivity_texture;
	Texture	shininess_texture;
	Texture	metalness_texture;
	Texture	fresnel_texture;
	Texture	emission_texture;
};

struct Mesh
{
	std::string name;
	uint32_t material_idx;
	// Where this Mesh's vertices start
	uint32_t start_index;
	uint32_t number_of_vertices;
};

class Model
{
public:
	~Model();
	
	void LoadModelFromOBJ(std::string filename);
	Texture LoadTexture(const std::string & filename, int nof_components);
	void Render(const bool submitMaterials = true);
	
private:
	// The name of the whole model
	std::string name_;
	// The filename of this model
	std::string filename_;
	// The materials 
	std::vector<Material> materials_;
	// A model will contain one or more "Meshes"
	std::vector<Mesh> meshes_;
	// Buffers on CPU
	std::vector<glm::vec3> positions_;
	std::vector<glm::vec3> normals_;
	std::vector<glm::vec2> texture_coordinates_;
	// Buffers on GPU
	uint32_t positions_bo_;
	uint32_t normals_bo_;
	uint32_t texture_coordinates_bo_;
	// Vertex Array Object
	uint32_t vaob_;
};

//void SaveModelToOBJ(Model * model, std::string filename);