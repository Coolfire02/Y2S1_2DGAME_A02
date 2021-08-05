#include "MeshBuilder.h"
#include <GL\glew.h>
#include <vector>

CMesh* CMeshBuilder::GenerateQuad(glm::vec4 color, float width, float height)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	v.position = glm::vec3(-0.5f * width, -0.5f * height, 0);
	v.color = color;
	v.texCoord = glm::vec2(0, 0);
	vertex_buffer_data.push_back(v);

	v.position = glm::vec3(0.5f * width, -0.5f * height, 0);
	v.color = color;
	v.texCoord = glm::vec2(1.0f, 0);
	vertex_buffer_data.push_back(v);

	v.position = glm::vec3(0.5f * width, 0.5f * height, 0);
	v.color = color;
	v.texCoord = glm::vec2(1.0f, 1.0f);
	vertex_buffer_data.push_back(v);

	v.position = glm::vec3(-0.5f * width, 0.5f * height, 0);
	v.color = color;
	v.texCoord = glm::vec2(0, 1.0f);
	vertex_buffer_data.push_back(v);

	index_buffer_data.push_back(3);
	index_buffer_data.push_back(0);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(1);
	index_buffer_data.push_back(2);
	index_buffer_data.push_back(0);

	CMesh* mesh = new CMesh();

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = CMesh::DRAW_TRIANGLES;

	return mesh;
}

CSpriteAnimation* CMeshBuilder::GenerateSpriteAnimation(unsigned numRow, unsigned numCol, float tile_width, float tile_height)
{
	Vertex v;
	std::vector<Vertex> vertex_buffer_data;
	std::vector<GLuint> index_buffer_data;

	float width = 1.f / numCol;
	float height = 1.f / numRow;
	int offset = 0;
	for (unsigned i = 0; i < numRow; ++i)
	{
		for (unsigned j = 0; j < numCol; ++j)
		{
			float u1 = j * width;
			float v1 = 1.f - height - i * height;
			v.position = glm::vec3(-0.5f * tile_width, -0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1, v1);
			vertex_buffer_data.push_back(v);

			v.position = glm::vec3(0.5f * tile_width, -0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1 + width, v1);
			vertex_buffer_data.push_back(v);

			v.position = glm::vec3(0.5f * tile_width, 0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1 + width, v1 + height);
			vertex_buffer_data.push_back(v);

			v.position = glm::vec3(-0.5f * tile_width, 0.5f * tile_height, 0);
			v.texCoord = glm::vec2(u1, v1 + height);
			vertex_buffer_data.push_back(v);

			index_buffer_data.push_back(offset + 3);
			index_buffer_data.push_back(offset + 0);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 1);
			index_buffer_data.push_back(offset + 2);
			index_buffer_data.push_back(offset + 0);
			offset += 4;
		}
	}

	CSpriteAnimation* mesh = new CSpriteAnimation(numRow, numCol);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(Vertex), &vertex_buffer_data[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data.size() * sizeof(GLuint), &index_buffer_data[0], GL_STATIC_DRAW);

	mesh->indexSize = index_buffer_data.size();
	mesh->mode = CMesh::DRAW_TRIANGLES;

	return mesh;
}
