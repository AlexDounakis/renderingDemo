#version 330 core
layout(location = 0) out vec4 out_color;

in vec3 f_normal;

void main(void)
{
	out_color = vec4(f_normal, 1.0);
}