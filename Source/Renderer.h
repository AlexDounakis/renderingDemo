#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "helpers/ShaderProgram.h"
#include "helpers/GeometryNode.h"

class Renderer
{
public:
	// Empty

protected:
	int												m_screen_width, m_screen_height;
	glm::mat4										m_world_matrix;
	glm::mat4										m_view_matrix;
	glm::mat4										m_projection_matrix;
	glm::vec3										m_camera_position;
	glm::vec3										m_camera_target_position;
	glm::vec3										m_camera_up_vector;
	glm::vec2										m_camera_movement;
	glm::vec2										m_camera_look_angle_destination;
	
	float											m_continous_time;

	enum OBJECS { TERRAIN = 0, CRAFT };

	std::vector<GeometryNode*>						m_nodes;
	ShaderProgram									m_geometry_rendering_program;

	// Protected Functions
													// init functions
	bool											InitShaders();
	bool											InitGeometricMeshes();
	void											BuildWorld();
	void											InitCamera();
													// render functions
	void											RenderGeometry();

public:

	Renderer();
	~Renderer();
													// basic functions
	bool											Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void											Update(float dt);
	void											Render();
													// update functions
	void											UpdateGeometry(float dt);
	void											UpdateCamera(float dt);

													// camera functions
	void											CameraMoveForward(bool enable);
	void											CameraMoveBackWard(bool enable);
	void											CameraMoveLeft(bool enable);
	void											CameraMoveRight(bool enable);
	void											CameraLook(glm::vec2 lookDir);
};

#endif
