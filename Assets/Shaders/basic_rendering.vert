#version 330 core
layout(location = 0) in vec3 coord3d;
layout(location = 1) in vec3 v_normal;

out vec3 f_normal;

uniform mat4 uniform_projection_matrix;
uniform mat4 uniform_normal_matrix;

void main(void) 
{
	f_normal = normalize((uniform_normal_matrix * vec4(v_normal, 0)).xyz);
	gl_Position = uniform_projection_matrix * vec4(coord3d, 1.0);
}
