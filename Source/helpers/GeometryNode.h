#ifndef GEOMETRY_NODE_H
#define GEOMETRY_NODE_H

#include <vector>
#include "GLEW\glew.h"
#include <unordered_map>
#include "glm\gtx\hash.hpp"
#include "TextureManager.h"

class GeometryNode
{
public:
	GeometryNode();
	~GeometryNode();

	void Init(class GeometricMesh* mesh);

	void Init(
		const std::vector<glm::vec3> & vertices,
		const std::vector<glm::vec3>& normals,
		const std::vector<glm::vec2>& texcoords,
		const std::string & texFile,
		bool pUseMipMap = false);

	struct Objects
	{
		glm::vec3 diffuse;
		glm::vec3 specular;
		glm::vec3 ambient;

		unsigned int start_offset;
		unsigned int count;

		float shininess;
		GLuint textureID;
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
	GLuint m_vbo_texcoords;

	glm::mat4 model_matrix;
	glm::mat4 app_model_matrix;

	aabb m_aabb;
};

#endif