#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "helpers/ShaderProgram.h"
#include "helpers/GeometryNode.h"
#include "helpers/LightNode.h"

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

		// Craft Movement 
		glm::vec3										m_craft_position;
		glm::vec3										m_craft_target_position;
		glm::vec3										m_craft_facing;
		glm::vec3										m_craft_right;
		glm::vec3										m_craft_up;
		glm::vec2										m_craft_movement;
		glm::vec2										m_craft_look_angle_destination;



	
		float											m_continous_time;
		std::vector<GeometryNode*>						m_nodes;

		LightNode										m_light;
		ShaderProgram									m_geometry_rendering_program;
		ShaderProgram									m_post_rendering_program;
		ShaderProgram									m_spot_light_shadow_map_program;

		enum OBJECS										{ TERRAIN, CRAFT };

		GLuint											m_fbo;
		GLuint											m_fbo_texture;
		GLuint											m_fbo_depth_texture;
		GLuint											m_vao_fbo;
		GLuint											m_vbo_fbo_vertices;

		// Protected Functions
														//'init' functions
		bool											InitShaders();
		bool											InitGeometricMeshes();
		bool											InitLights();
		bool											InitCommonItems();
		bool											InitIntermediateBuffers();
		void											BuildWorld();
		void											InitCamera();
		void											RenderShadowMaps();
		void											RenderPostProcess();

														//'update' functions
		void											UpdateGeometry(float dt);
		void											UpdateCamera(float dt);
		void											UpdateCraft(float dt);


														//'render' function
		void											RenderGeometry();

	public:

		Renderer();
		~Renderer();
													//basic functions
		bool										Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
		void										Update(float dt);
		void										Render();

													//camera functions
		void										CameraMoveForward(bool enable);
		void										CameraMoveBackWard(bool enable);
		void										CameraMoveLeft(bool enable);
		void										CameraMoveRight(bool enable);
		void										CameraLook(glm::vec2 lookDir);

													//craft functions
		void										CraftMoveForward(bool enable);
		void										CraftMoveBackward(bool enable);
		void										CraftMoveLeft(bool enable);
		void										CraftMoveRight(bool enable);
		void										CraftLook(glm::vec2 lookDir);


		bool										ResizeBuffers(int SCREEN_WIDTH, int SCREEN_HEIGHT);
		bool										ReloadShaders();
	};

	#endif
