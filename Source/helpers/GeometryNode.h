#ifndef GEOMETRY_NODE_H
#define GEOMETRY_NODE_H

#include <vector>
#include "GLEW\glew.h"
#include <unordered_map>
#include "glm\gtx\hash.hpp"

class GeometryNode
{
public:
	GeometryNode();
	~GeometryNode();

	void Init(class GeometricMesh* mesh);

	struct Objects
	{
		unsigned int start_offset;
		unsigned int count;
	};

	struct aabb
	{
		glm::vec3 min;
		glm::vec3 max;
		glm::vec3 center;
	};

	std::vector<Objects> parts;

	GLuint m_vao;
	GLuint m_vbo_positions;
	GLuint m_vbo_normals;

	glm::mat4 model_matrix;
	glm::mat4 app_model_matrix;
	aabb m_aabb;
};

#endif