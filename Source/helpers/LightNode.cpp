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


/// SHADOW MAP  FUNCTIONS 

void LightNode::CastShadow(bool cast)
{
	m_cast_shadow = cast;

	if (cast)
	{
		if (m_shadow_map_texture == 0)
			glGenTextures(1, &m_shadow_map_texture);
		// Depth buffer
		glBindTexture(GL_TEXTURE_2D, m_shadow_map_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_shadow_map_resolution, m_shadow_map_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_shadow_map_fbo == 0)
			glGenFramebuffers(1, &m_shadow_map_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadow_map_texture, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		GLenum status = Tools::CheckFramebufferStatus(m_shadow_map_fbo);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Error in Spotlight shadow FB generation.\n");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

bool LightNode::GetCastShadowsStatus()
{
	return m_cast_shadow;
}

GLuint LightNode::GetShadowMapFBO()
{
	return m_shadow_map_fbo;
}

GLuint LightNode::GetShadowMapDepthTexture()
{
	return m_shadow_map_texture;
}

int LightNode::GetShadowMapResolution()
{
	return m_shadow_map_resolution;;
}

glm::mat4 LightNode::GetProjectionMatrix()
{
	return m_projection_matrix;
}

glm::mat4 LightNode::GetViewMatrix()
{
	return m_view_matrix;
}