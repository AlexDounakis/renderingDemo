#version 330 core
layout(location = 0) out vec4 out_color;

in vec3 f_normal;
in vec2 f_texcoord;
in vec3 f_position_wcs;

#define _PI_ 3.14159

//#define _DIRECTIONAL_LIGHT_
//#define _POINT_LIGHT_
#define _SPOT_LIGHT_

uniform vec3 uniform_light_color;
uniform vec3 uniform_light_pos;
uniform vec3 uniform_light_dir;

uniform float uniform_light_umbra;
uniform float uniform_light_penumbra;

uniform vec3 uniform_camera_pos;
uniform vec3 uniform_camera_dir;

uniform vec3 uniform_diffuse;
uniform vec3 uniform_specular;
uniform vec3 uniform_ambient;
uniform float uniform_shininess;

uniform int uniform_has_texture;
uniform sampler2D uniform_tex_diffuse;

float compute_spotlight(const in vec3 pSurfToLight)
{
	float cos_umbra = cos(radians(0.5 * uniform_light_umbra));
	float cos_penumbra = cos(radians(0.5 * uniform_light_penumbra));
	float spoteffect = 1;
	float angle_vertex_spot_dir = dot(-pSurfToLight, uniform_light_dir);

	if (angle_vertex_spot_dir > cos_umbra) 
	{
		spoteffect = 1;
	}
	else if(angle_vertex_spot_dir >= cos_penumbra) 
	{
		spoteffect = smoothstep(cos_penumbra, cos_umbra, angle_vertex_spot_dir);
	}
	else
	{
		spoteffect = 0;
	}
	
	return spoteffect;
}

vec3 blinn_phong(const in vec3 pSurfToEye, const in vec3 pSurfToLight)
{
	vec3 halfVector = normalize(pSurfToEye + pSurfToLight);

	float NdotL = max(dot(f_normal, pSurfToLight), 0.0);
	float NdotH = max(dot(f_normal, halfVector), 0.0);

	vec3 albedo = uniform_has_texture == 1 ? texture(uniform_tex_diffuse, f_texcoord).rgb : uniform_diffuse;

	vec3 kd = albedo / _PI_;
	vec3 ks = uniform_specular;

	float fn =
		((uniform_shininess + 2) * (uniform_shininess + 4)) /
		(8 * _PI_ * (uniform_shininess + 1.0 / pow(2, uniform_shininess / 2.0)));

	vec3 diffuse = kd * NdotL;
	vec3 specular = NdotL > 0.0 ? ks * fn * pow(NdotH, uniform_shininess) : vec3(0.0);

	return (diffuse + specular) * uniform_light_color + uniform_ambient;
}

void main(void)
{
	vec3 surfToEye = normalize(uniform_camera_pos - f_position_wcs);

#if defined(_DIRECTIONAL_LIGHT_)

	out_color = vec4(blinn_phong(surfToEye, -uniform_light_dir), 1.0);

#elif defined(_POINT_LIGHT_)

	vec3 surfToLight = normalize(uniform_light_pos - f_position_wcs);
	float dist = distance(uniform_light_pos, f_position_wcs);

	out_color = vec4(blinn_phong(surfToEye, surfToLight) / pow(dist, 2), 1.0);

#elif defined(_SPOT_LIGHT_)

	vec3 surfToLight = normalize(uniform_light_pos - f_position_wcs);
	vec3 brdf = blinn_phong(surfToEye, surfToLight);
	float spotEffect =  compute_spotlight(surfToLight);
	float dist = distance(uniform_light_pos, f_position_wcs);
	out_color = vec4(brdf * spotEffect / pow(dist, 2), 1.0);

#endif
}