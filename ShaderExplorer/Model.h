#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Entity.h"

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

class Model : public Entity
{
public:
	~Model();
	// The name of the whole model
	std::string name;
	// The filename of this model 
	std::string filename;
	// The materials 
	std::vector<Material> materials;
	// A model will contain one or more "Meshes"
	std::vector<Mesh> meshes;
	// Buffers on CPU
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> texture_coordinates;
	// Buffers on GPU
	uint32_t positions_bo;
	uint32_t normals_bo;
	uint32_t texture_coordinates_bo;
	// Vertex Array Object
	uint32_t vaob;

	void LoadModelFromOBJ(std::string filename);
	Texture LoadTexture(const std::string & filename, int nof_components);
	void Render(const bool submitMaterials = true);
};

void SaveModelToOBJ(Model * model, std::string filename);