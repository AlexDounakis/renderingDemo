#include "GeometryNode.h"
#include "GeometricMesh.h"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"

GeometryNode::GeometryNode()
{
	m_vao = 0;
	m_vbo_positions = 0;
	m_vbo_normals = 0;
}

GeometryNode::~GeometryNode()
{
	// delete buffers
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo_positions);
	glDeleteBuffers(1, &m_vbo_normals);
}

void GeometryNode::Init(GeometricMesh* mesh)
{
	std::vector<glm::vec3>& vertices = mesh->vertices;
	std::vector<glm::vec3>& normals = mesh->normals;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo_positions);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,       // the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	glGenBuffers(1, &m_vbo_normals);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_normals);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,				// attribute index
		3,              // number of elements per vertex, here (x,y,z)
		GL_FLOAT,		// the type of each element
		GL_FALSE,       // take our values as-is
		0,		         // no extra data between each position
		0				// pointer to the C array or an offset to our buffer
	);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// *********************************************************************

	for (int i = 0; i < mesh->objects.size(); i++)
	{
		Objects part;
		part.start_offset = mesh->objects[i].start;
		part.count = mesh->objects[i].end - mesh->objects[i].start;
		parts.push_back(part);
	}

	this->m_aabb.min = glm::vec3(std::numeric_limits<float_t>::max());
	this->m_aabb.max = glm::vec3(-std::numeric_limits<float_t>::max());

	for (auto& v : mesh->vertices)
	{
		this->m_aabb.min = glm::min(this->m_aabb.min, v);
		this->m_aabb.max = glm::max(this->m_aabb.max, v);
	}

	this->m_aabb.center = (this->m_aabb.min + this->m_aabb.max) * 0.5f;
}