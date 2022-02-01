#version 330
layout(location = 0) out vec4 out_color;

in vec2 f_texcoord;

uniform sampler2D uniform_texture;

void main(void)
{
	vec3 color = texture2D(uniform_texture, f_texcoord).rgb;
	out_color = vec4(color, 1.0);
}