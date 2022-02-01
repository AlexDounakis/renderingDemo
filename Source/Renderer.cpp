#include "Renderer.h"
#include "helpers/GeometryNode.h"
#include "helpers/Tools.h"
#include "helpers/ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "helpers/OBJLoader.h"

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
	// empty
}

// INIT
bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	bool techniques_initialization = InitShaders();
	bool meshes_initialization = InitGeometricMeshes();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	if (meshes_initialization)
	{
		this->BuildWorld();
		this->InitCamera();
	}

	//If everything initialized
	return techniques_initialization && meshes_initialization;
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

		return true;
	}

	// init objects paths
	bool Renderer::InitGeometricMeshes()
	{
		std::array<const char*, 2 > assets = {
			"Assets/game_assets/terrain.obj",
			"Assets/game_assets/craft.obj"
		};

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
			else
			{
				initialized = false;
			}
		}

		return initialized;
	}

	// transformate the world
	void Renderer::BuildWorld()
	{
		GeometryNode& terrain = *this->m_nodes[OBJECS::TERRAIN];
		GeometryNode& craft = *this->m_nodes[OBJECS::CRAFT];

		//terrain object init
		terrain.model_matrix = glm::mat4(1.f);

		//craft object init
		craft.model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 200.f, 0.f));

		this->m_world_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.02, 0.02, 0.02));
	}

	// init the camera properties
	void Renderer::InitCamera()
{
	this->m_camera_position = glm::vec3(0, 10, 6);
	this->m_camera_target_position = glm::vec3(0, 0, 0);
	this->m_camera_up_vector = glm::vec3(0, 1, 0);

	this->m_view_matrix = glm::lookAt(
		this->m_camera_position,
		this->m_camera_target_position,
		m_camera_up_vector);

	this->m_projection_matrix = glm::perspective(
		glm::radians(45.f),
		this->m_screen_width / (float) this->m_screen_height,
		0.1f, 1000.f);
}

// UPDATE
void Renderer::Update(float dt)
{
	this->UpdateGeometry(dt);
	this->UpdateCamera(dt);
	m_continous_time += dt;
}

	// update geometry
	void Renderer::UpdateGeometry(float dt)
	{
		GeometryNode& craft = *this->m_nodes[OBJECS::CRAFT];

		craft.app_model_matrix =
			glm::translate(glm::mat4(1.f), glm::vec3(craft.m_aabb.center.x, craft.m_aabb.center.y, craft.m_aabb.center.z)) *
			glm::rotate(glm::mat4(1.f), m_continous_time, glm::vec3(0.f, 1.f, 0.f)) *
			glm::translate(glm::mat4(1.f), glm::vec3(-craft.m_aabb.center.x, -craft.m_aabb.center.y, -craft.m_aabb.center.z)) *
			craft.model_matrix;

	}

	// update camera
	void Renderer::UpdateCamera(float dt)
	{
		glm::vec3 direction = glm::normalize(m_camera_target_position - m_camera_position);

		m_camera_position = m_camera_position + (m_camera_movement.x * 5.f * dt) * direction;
		m_camera_target_position = m_camera_target_position + (m_camera_movement.x * 5.f * dt) * direction;

		glm::vec3 right = glm::normalize(glm::cross(direction, m_camera_up_vector));

		m_camera_position = m_camera_position + (m_camera_movement.y * 5.f * dt) * right;
		m_camera_target_position = m_camera_target_position + (m_camera_movement.y * 5.f * dt) * right;

		float speed = glm::pi<float>() * 0.0002;
		glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.y * speed, right);
		rotation *= glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.x * speed, m_camera_up_vector);
		m_camera_look_angle_destination = glm::vec2(0.f);

		direction = rotation * glm::vec4(direction, 0.f);
		m_camera_target_position = m_camera_position + direction * glm::distance(m_camera_position, m_camera_target_position);

		m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);
	}

// RENDER
void Renderer::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0, 0, m_screen_width, m_screen_height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.f);
	glClear(GL_DEPTH_BUFFER_BIT);

	RenderGeometry();

	glDisable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GLenum error = Tools::CheckGLError();
	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}
}

	void Renderer::RenderGeometry()
	{
		this->m_geometry_rendering_program.Bind();

		glm::mat4 proj = m_projection_matrix * m_view_matrix * m_world_matrix;

		for (auto& node : this->m_nodes)
		{
			glBindVertexArray(node->m_vao);

			glUniformMatrix4fv(m_geometry_rendering_program["uniform_projection_matrix"], 1, GL_FALSE,
				glm::value_ptr(proj * node->app_model_matrix));

			glm::mat4 normal_matrix = glm::transpose(glm::inverse(m_world_matrix * node->app_model_matrix));

			glUniformMatrix4fv(m_geometry_rendering_program["uniform_normal_matrix"], 1, GL_FALSE,
				glm::value_ptr(normal_matrix));

			for (int j = 0; j < node->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
			}

			glBindVertexArray(0);
		}


		this->m_geometry_rendering_program.Unbind();
	}

// CAMERA
void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable) ? 1 : 0;
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