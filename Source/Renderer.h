#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "helpers/ShaderProgram.h"
#include "helpers/GeometryNode.h"
#include "helpers/CollidableNode.h"
#include "helpers/LightNode.h"

class Renderer
{
public:
	// Empty

protected:
	//craft positions
	float										speedBias = 10.f;
	float										craft_x;
	float										craft_y;
	float										craft_z;

	int											m_screen_width, m_screen_height;

	glm::mat4									m_world_matrix;
	glm::mat4									m_view_matrix;
	glm::mat4									m_projection_matrix;
	glm::vec3									m_camera_position;
	glm::vec3									m_camera_target_position;
	glm::vec3									m_camera_up_vector;
	glm::vec2									m_camera_movement;
	glm::vec2									m_camera_look_angle_destination;
	
	float										m_continous_time;

	//Objects
	enum OBJECS									{ TERRAIN, CRAFT };

	std::vector<GeometryNode*>					m_nodes;								
	std::vector<CollidableNode*>				m_collidables_nodes;

	LightNode									m_light;
	ShaderProgram								m_geometry_program;
	ShaderProgram								m_post_program;
	ShaderProgram								m_spot_light_shadow_map_program;

	GLuint										m_fbo;
	GLuint										m_fbo_texture;
	GLuint										m_fbo_depth_texture;
	GLuint										m_vao_fbo;
	GLuint										m_vbo_fbo_vertices;

	// Protected Functions
	//'init' functions
	bool										InitShaders();
	bool										InitGeometricMeshes();
	bool										InitLights();
	bool										InitCommonItems();
	bool										InitIntermediateBuffers();
	void										BuildWorld();
	void										InitCamera();

	//'render' function
	void										RenderGeometry();
	void										RenderStaticGeometry();
	void										RenderShadowMaps();
	void										RenderPostProcess();

public:

	Renderer();
	~Renderer();
	bool										Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										Update(float dt);
	void										Render();
	
	//update functions
	void										UpdateGeometry(float dt);
	void										UpdateCamera(float dt);

	//camera move functions
	void										CameraMoveForward(bool enable);
	void										CameraMoveBackWard(bool enable);
	void										CameraMoveLeft(bool enable);
	void										CameraMoveRight(bool enable);
	void										CameraLook(glm::vec2 lookDir);

	//craft move functions
	void										CraftMoveForward(bool enable);
	void										CraftMoveBackward(bool enable);
	void										CraftMoveLeft(bool enable);
	void										CraftMoveRight(bool enable);
	void										CraftLook(glm::vec2 lookDir);

	bool										ReloadShaders();
	bool										ResizeBuffers(int SCREEN_WIDTH, int SCREEN_HEIGHT);
};

#endif