#include "LightNode.h"
#include "glm\gtc\matrix_transform.hpp"
#include "Tools.h"

// Spot Light
LightNode::LightNode()
{
	m_name = "defaultSpotLight1";

	m_light_direction = glm::normalize(glm::vec3(-1, -1, 0));
	m_light_position = glm::vec3(5, 3, 0);
	m_light_color = glm::vec3(1.0f);
	SetPosition(m_light_position);	
	// umbra and penumbra in degrees
	SetConeSize(60, 60);
	m_spotlight_exponent = 2;
}

LightNode::~LightNode()
{ /* Empty */ }

void LightNode::SetColor(const glm::vec3 & color)
{
	m_light_color = color;
}

void LightNode::SetPosition(const glm::vec3 & pos)
{
	m_light_position = pos;
	m_light_direction = glm::normalize(m_light_target - m_light_position);
}

void LightNode::SetTarget(const glm::vec3 & target)
{
	m_light_target = target;
	m_light_direction = glm::normalize(m_light_target - m_light_position);
}

void LightNode::SetExponent(float value)
{
	m_spotlight_exponent = value;
}

void LightNode::SetConeSize(float umbra, float penumbra)
{
	m_umbra = umbra;
	m_penumbra = penumbra;
}

glm::vec3 LightNode::GetPosition()
{
	return m_light_position;
}

glm::vec3 LightNode::GetDirection()
{
	return m_light_direction;
}

glm::vec3 LightNode::GetColor()
{
	return m_light_color;
}

float LightNode::GetUmbra()
{
	return m_umbra;
}

float LightNode::GetPenumbra()
{
	return m_penumbra;
}

float LightNode::GetExponent()
{
	return m_spotlight_exponent;
}