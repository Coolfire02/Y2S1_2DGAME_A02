#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <includes/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	Vertex() {}
};

bool LoadOBJ(
	const char *file_path, 
	std::vector<glm::vec3> & out_vertices, 
	std::vector<glm::vec2> & out_uvs, 
	std::vector<glm::vec3> & out_normals
);

void IndexVBO(
	std::vector<glm::vec3> & in_vertices,
	std::vector<glm::vec2> & in_uvs,
	std::vector<glm::vec3> & in_normals,

	std::vector<unsigned> & out_indices,
	std::vector<Vertex> & out_vertices
);

#endif