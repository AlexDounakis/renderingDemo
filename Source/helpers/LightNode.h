#ifndef SPOTLIGHT_NODE_H
#define SPOTLIGHT_NODE_H

#include "glm/glm.hpp"

#include <unordered_map>
#include "GLEW\glew.h"

class LightNode
{
	std::string m_name;
	glm::vec3 m_light_direction;
	glm::vec3 m_light_position;
	glm::vec3 m_light_target;
	glm::vec3 m_light_color;

	float m_umbra;
	float m_penumbra;
	float m_spotlight_exponent;

public:	
	LightNode();
	~LightNode();

	void SetPosition(const glm::vec3 & pos);
	void SetColor(const glm::vec3 & color);
	void SetTarget(const glm::vec3 & target);
	void SetConeSize(float umbra, float penumbra);
	void SetExponent(float value);

	glm::vec3 GetPosition();
	glm::vec3 GetDirection();
	glm::vec3 GetColor();

	float GetUmbra();
	float GetPenumbra();
	float GetExponent();
};

#endif
