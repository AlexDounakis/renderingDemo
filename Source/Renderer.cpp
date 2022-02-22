#include "Renderer.h"
#include "helpers/GeometricMesh.h"
#include "helpers/Tools.h"
#include "helpers/ShaderProgram.h"
#include "helpers/OBJLoader.h"

#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <algorithm>
#include <array>

// RENDERER
Renderer::Renderer()
{
	this->m_nodes = {};
	this->m_continous_time = 0.0;
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &m_fbo_texture);
	glDeleteFramebuffers(1, &m_fbo);

	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);

}

// INIT
bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	bool techniques_initialization = InitShaders();

	bool meshes_initialization = InitGeometricMeshes();

	bool light_initialization = InitLights();

	bool common_initialization = InitCommonItems();
	bool inter_buffers_initialization = InitIntermediateBuffers();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}
	this->BuildWorld();
	this->InitCamera();

	//If everything initialized
	return techniques_initialization && meshes_initialization &&
		common_initialization && inter_buffers_initialization;
}

	// init shaders paths
	bool Renderer::InitShaders()
	{
		std::string vertex_shader_path = "Assets/Shaders/basic_rendering.vert";
		std::string fragment_shader_path = "Assets/Shaders/basic_rendering.frag";

		this->m_geometry_rendering_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
		this->m_geometry_rendering_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
		this->m_geometry_rendering_program.CreateProgram();
		this->m_geometry_rendering_program.LoadUniform("uniform_projection_matrix");
		this->m_geometry_rendering_program.LoadUniform("uniform_normal_matrix");
		this->m_geometry_rendering_program.LoadUniform("uniform_diffuse");
		this->m_geometry_rendering_program.LoadUniform("uniform_has_texture");
		this->m_geometry_rendering_program.LoadUniform("uniform_texture");

		vertex_shader_path = "Assets/Shaders/post_process.vert";
		fragment_shader_path = "Assets/Shaders/post_process.frag";

		this->m_post_rendering_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
		this->m_post_rendering_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
		this->m_post_rendering_program.CreateProgram();
		this->m_post_rendering_program.LoadUniform("uniform_texture");

		vertex_shader_path = "Assets/Shaders/shadow_map_rendering.vert";
		fragment_shader_path = "Assets/Shaders/shadow_map_rendering.frag";

		/*this->m_spot_light_shadow_map_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
		this->m_spot_light_shadow_map_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
		this->m_spot_light_shadow_map_program.CreateProgram();*/


		return true;
	}

	// init objects paths
	bool Renderer::InitGeometricMeshes()
	{
		std::array<const char*, 2> assets = { "Assets/game_assets/terrain.obj", "Assets/game_assets/craft.obj" };

		bool initialized = true;
		OBJLoader loader;

		for (auto& asset : assets)
		{
			GeometricMesh* mesh = loader.load(asset);
			
			if (mesh != nullptr)
			{
				GeometryNode* node = new GeometryNode();
				node->Init(mesh);
				this->m_nodes.push_back(node);
				delete mesh;
			}
			else  //maybe not right
			{
				initialized = false;
			}

		}

		return initialized;
	}

	// init the light
	bool Renderer::InitLights()
	{
		//this->m_light.SetColor(glm::vec3(0.5f));
		this->m_light.SetColor(glm::vec3(1000.f));
		this->m_light.SetPosition(glm::vec3(0, 30, 0));
		this->m_light.SetTarget(glm::vec3(0));
		this->m_light.SetConeSize(30, 40);
		this->m_light.CastShadow(false);

		return true;
	}

	// transformate the world
	void Renderer::BuildWorld()
	{
		GeometryNode& terrain = *this->m_nodes[OBJECS::TERRAIN];
		GeometryNode& craft = *this->m_nodes[OBJECS::CRAFT];

		//terrain object init
		terrain.model_matrix = glm::mat4(1.f);

		//craft object init
		craft.model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -200.f, 0.f));

		this->m_craft_position = glm::vec3(10.f, 20.f, 0.f);
		this->m_craft_target_position = glm::vec3(0, 0, 1);
		this->m_craft_up = glm::vec3(0, 1, 0);

		//craft.model_matrix = m_camera_position;

		this->m_world_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.02, 0.02, 0.02));
	}

	// texturing
	bool Renderer::InitCommonItems()
	{
		glGenVertexArrays(1, &m_vao_fbo);
		glBindVertexArray(m_vao_fbo);

		GLfloat fbo_vertices[] = { -1, -1, 1, -1, -1, 1, 1, 1, };

		glGenBuffers(1, &m_vbo_fbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindVertexArray(0);
		return true;
	}

	bool Renderer::InitIntermediateBuffers()
	{
		glGenTextures(1, &m_fbo_texture);
		glGenTextures(1, &m_fbo_depth_texture);
		glGenFramebuffers(1, &m_fbo);

		return ResizeBuffers(m_screen_width, m_screen_height);
	}

	// init the camera properties
	void Renderer::InitCamera()
	{
		this->m_camera_position = glm::vec3(m_craft_position.x , m_camera_position.y , m_craft_position.z + 10 );
		this->m_camera_target_position = m_craft_target_position;
		this->m_camera_up_vector = m_craft_up;

		this->m_view_matrix = glm::lookAt(
			this->m_craft_position,
			this->m_craft_target_position,
			m_craft_up);

		this->m_projection_matrix = glm::perspective(
			glm::radians(45.f),
			this->m_screen_width / (float) this->m_screen_height,
			0.1f, 1000.f);
	}

// UPDATE
void Renderer::Update(float dt)
{
	//this->UpdateGeometry(dt);
	this->UpdateCraft(dt);
	//this->UpdateCamera(dt);
	m_continous_time += dt;
}

	void Renderer::UpdateGeometry(float dt)
	{
		GeometryNode& craft = *this->m_nodes[OBJECS::CRAFT];

		craft.app_model_matrix =
			/*glm::translate(glm::mat4(1.f), glm::vec3(craft.m_aabb.center.x, craft.m_aabb.center.y, craft.m_aabb.center.z)) *
			glm::rotate(glm::mat4(1.f), m_continous_time, glm::vec3(0.f, 1.f, 0.f)) *
			glm::translate(glm::mat4(1.f), glm::vec3(-craft.m_aabb.center.x, -craft.m_aabb.center.y, -craft.m_aabb.center.z)) **/
			craft.model_matrix;

	}

	void Renderer::UpdateCamera(float dt)
	{

		/*m_camera_position = glm::lookAt(m_craft_position, m_craft_position + m_craft_facing, m_craft_up);
		m_camera_target_position = m_craft_facing;

		m_view_matrix = glm::lookAt(m_camera_position, m_camera_position + m_camera_target_position, glm::vec3(0, 1, 0));*/
		/*glm::vec3 direction = glm::normalize(m_craft_target_position - m_craft_position);

		m_camera_position = m_craft_position + (m_craft_movement.x * 5.f * dt) * direction;
		m_camera_target_position = m_craft_target_position + (m_craft_movement.x * 5.f * dt) * direction;

		glm::vec3 right = glm::normalize(glm::cross(direction, m_craft_up));

		m_camera_position = m_craft_position + (m_craft_movement.y * 5.f * dt) * right;
		m_camera_target_position = m_craft_target_position + (m_craft_movement.y * 5.f * dt) * right;

		float speed = glm::pi<float>() * 0.0002;

		glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_craft_look_angle_destination.y * speed, right);
		rotation *= glm::rotate(glm::mat4(1.f), m_craft_look_angle_destination.x * speed, m_craft_up);
		m_camera_look_angle_destination = m_craft_look_angle_destination;

		direction = rotation * glm::vec4(direction, 0.f);
		m_camera_target_position = m_craft_position + direction * glm::distance(m_craft_position, m_craft_target_position);*/

		m_view_matrix = glm::lookAt(m_craft_position, m_craft_target_position, m_craft_up);

		// if i want the light follow the camera
		//m_light.SetPosition(m_camera_position);
		//m_light.SetTarget(m_camera_target_position);
	}


	void Renderer::UpdateCraft(float dt) 
	{
		GeometryNode& craft = *this->m_nodes[OBJECS::CRAFT];

		glm::vec3 direction = glm::normalize(m_craft_target_position - m_craft_position);

		m_craft_position = m_craft_position + (m_craft_movement.x * 5.f * dt) * direction;
		m_craft_target_position = m_craft_target_position + (m_craft_movement.x * 5.f * dt) * direction;

		glm::vec3 right = glm::normalize(glm::cross(direction, m_craft_up));

		m_craft_position = m_craft_position + (m_craft_movement.y * 5.f * dt) * right;
		m_craft_target_position = m_craft_target_position + (m_craft_movement.y * 5.f * dt) * right;

		float speed = glm::pi<float>() * 20.;

		glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.y * speed, right);
		rotation *= glm::rotate(glm::mat4(1.f), m_craft_look_angle_destination.x * speed, m_craft_up);
		m_craft_look_angle_destination = glm::vec2(0.f);

		direction = rotation * glm::vec4(direction, 0.f);
		m_craft_target_position = m_craft_position + direction * glm::distance(m_craft_position, m_craft_target_position);

		craft.app_model_matrix = glm::lookAt(m_craft_position, m_craft_target_position, m_craft_up);
		
		m_view_matrix = glm::lookAt(m_craft_position, m_craft_target_position, m_craft_up);
		//m_projection_matrix = m_view_matrix;
	}

// RENDER
void Renderer::Render()
{
	//RenderShadowMaps();
	RenderGeometry();
	//RenderPostProcess();

	GLenum error = Tools::CheckGLError();

	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}

}

	void Renderer::RenderGeometry()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawbuffers);

		glViewport(0, 0, m_screen_width, m_screen_height);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glClearDepth(1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		this->m_geometry_rendering_program.Bind();

		glm::mat4 proj = m_projection_matrix * m_view_matrix * m_world_matrix;

		m_geometry_rendering_program.loadVec3("uniform_light_color", m_light.GetColor());
		m_geometry_rendering_program.loadVec3("uniform_light_dir", m_light.GetDirection());
		m_geometry_rendering_program.loadVec3("uniform_light_pos", m_light.GetPosition());

		m_geometry_rendering_program.loadFloat("uniform_light_umbra", m_light.GetUmbra());
		m_geometry_rendering_program.loadFloat("uniform_light_penumbra", m_light.GetPenumbra());

		m_geometry_rendering_program.loadVec3("uniform_camera_pos", m_camera_position);
		m_geometry_rendering_program.loadVec3("uniform_camera_dir", normalize(m_camera_target_position - m_camera_position));

		/*m_geometry_rendering_program.loadMat4("uniform_light_projection_view", m_light.GetProjectionMatrix() * m_light.GetViewMatrix());
		m_geometry_rendering_program.loadInt("uniform_cast_shadows", m_light.GetCastShadowsStatus() ? 1 : 0);*/

		//glActiveTexture(GL_TEXTURE2);
		//m_geometry_rendering_program.loadInt("uniform_shadow_map", 2);
		//glBindTexture(GL_TEXTURE_2D, m_light.GetShadowMapDepthTexture());

		///*RenderStaticGeometry();
		//RenderCollidableGeometry();*/

		//m_geometry_rendering_program.Unbind();
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glDisable(GL_DEPTH_TEST);

		for (auto& node : this->m_nodes)
		{
			glBindVertexArray(node->m_vao);

			glUniformMatrix4fv(m_geometry_rendering_program["uniform_projection_matrix"], 1, GL_FALSE, glm::value_ptr(proj * node->app_model_matrix));

			glm::mat4 normal_matrix = glm::transpose(glm::inverse(m_world_matrix * node->app_model_matrix));
			glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE, glm::value_ptr(normal_matrix));
			
			m_geometry_rendering_program.loadMat4("uniform_world_matrix", m_world_matrix * node->app_model_matrix);

			for (int j = 0; j < node->parts.size(); ++j)
			{

				m_geometry_rendering_program.loadVec3("uniform_diffuse", node->parts[j].diffuse);
				m_geometry_rendering_program.loadVec3("uniform_ambient", node->parts[j].ambient);
				m_geometry_rendering_program.loadVec3("uniform_specular", node->parts[j].specular);
				m_geometry_rendering_program.loadFloat("uniform_shininess", node->parts[j].shininess);
				m_geometry_rendering_program.loadInt("uniform_has_texture", (node->parts[j].textureID > 0) ? 1 : 0);	

				glActiveTexture(GL_TEXTURE0);
				glUniform1i(m_geometry_rendering_program["uniform_texture"], 0);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].textureID);

				glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);

			}

			glBindVertexArray(0);
		}

		this->m_geometry_rendering_program.Unbind();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.f, 0.8f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		m_post_rendering_program.Bind();

		glBindVertexArray(m_vao_fbo);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
		glUniform1i(m_post_rendering_program["uniform_texture"], 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		m_post_rendering_program.Unbind();
	}

/// SHADOWMAPS

void Renderer::RenderShadowMaps()
{
	if (m_light.GetCastShadowsStatus())
	{
		int m_depth_texture_resolution = m_light.GetShadowMapResolution();

		glBindFramebuffer(GL_FRAMEBUFFER, m_light.GetShadowMapFBO());
		glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Bind the shadow mapping program
		m_spot_light_shadow_map_program.Bind(); // !!!!

		glm::mat4 proj = m_light.GetProjectionMatrix() * m_light.GetViewMatrix() * m_world_matrix;

		for (auto& node : this->m_nodes)
		{
			glBindVertexArray(node->m_vao);

			m_spot_light_shadow_map_program.loadMat4("uniform_projection_matrix", proj * node->app_model_matrix);

			for (int j = 0; j < node->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
			}

			glBindVertexArray(0);
		}

		glm::vec3 camera_dir = normalize(m_camera_target_position - m_camera_position);
		float_t isectT = 0.f;

		// FOR COLLIDABLE NODES , WE DONT IMPLEMENT YET 

		/*for (auto& node : this->m_collidables_nodes)
		{
			if (node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT)) continue;

			glBindVertexArray(node->m_vao);

			m_spot_light_shadow_map_program.loadMat4("uniform_projection_matrix", proj * node->app_model_matrix);

			for (int j = 0; j < node->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
			}

			glBindVertexArray(0);
		}*/

		m_spot_light_shadow_map_program.Unbind();
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		printf("RenderShadowMaps DONE\n");
	}
}

void Renderer::RenderPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.f, 0.8f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	m_post_rendering_program.Bind();

	glBindVertexArray(m_vao_fbo);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	m_post_rendering_program.loadInt("uniform_texture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light.GetShadowMapDepthTexture());
	m_post_rendering_program.loadInt("uniform_shadow_map", 1);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_post_rendering_program.Unbind();
	printf("RenderPostProcess DONE\n");
}

// CAMERA
void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable) ? 1 : 0;
	//m_craft_movemement.x = 
}

void Renderer::CameraMoveBackWard(bool enable)
{
	m_camera_movement.x = (enable) ? -1 : 0;
}

void Renderer::CameraMoveLeft(bool enable)
{
	m_camera_movement.y = (enable) ? -1 : 0;
}

void Renderer::CameraMoveRight(bool enable)
{
	m_camera_movement.y = (enable) ? 1 : 0;
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	m_camera_look_angle_destination = lookDir;
}

// CRAFT FUNCTIONS

void Renderer::CraftMoveForward(bool enable)
{
	m_craft_movement.x = (enable) ? 5 : 0;
	//m_craft_movemement.x = 
}

void Renderer::CraftMoveBackward(bool enable)
{
	m_craft_movement.x = (enable) ? -5 : 0;
}

void Renderer::CraftMoveLeft(bool enable)
{
	m_craft_movement.y = (enable) ? -5 : 0;
}

void Renderer::CraftMoveRight(bool enable)
{
	m_craft_movement.y = (enable) ? 5: 0;
}

void Renderer::CraftLook(glm::vec2 lookDir)
{
	m_craft_look_angle_destination = lookDir;
}


// HELPEPS
bool Renderer::ResizeBuffers(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_screen_width, m_screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool Renderer::ReloadShaders()
{
	m_geometry_rendering_program.ReloadProgram();
	m_post_rendering_program.ReloadProgram();
	return true;
}