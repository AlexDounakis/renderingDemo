#version 330 core
layout(location = 0) out vec4 out_color;

in vec3 f_normal;
in vec2 f_texcoord;

uniform int uniform_has_texture;
uniform vec3 uniform_diffuse;
uniform sampler2D uniform_texture;

#define TEXTURE_TEST

void main(void)
{
#ifdef TEXTURE_TEST
	vec4 tex_val = texture(uniform_texture, f_texcoord);
	//if(tex_val.a < 0.01) discard;
	out_color = tex_val;
#else
	vec3 color = uniform_has_texture == 1 ?
		texture(uniform_texture, f_texcoord).rgb : uniform_diffuse;

	out_color = vec4(color, 1.0);
#endif
}