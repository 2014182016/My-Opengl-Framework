#version 450 core

#include "Lighting.glsl"
#include "RenderingConstants.glsl"

in VertexOut
{
	vec2 texcoord;
}fs_in;

out vec4 out_FragColor;

uniform sampler2D g_SceneTexture;
uniform sampler2D g_BloomTexture;

void main()
{
	vec3 color = texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy), 0).rgb;

	// Bloom
	{
		vec3 bloomColor = texture(g_BloomTexture, fs_in.texcoord).rgb;
		color += bloomColor;
	}

	// Exposure
	{
		color = vec3(1.0f) - exp(-color * g_Exposure);
	}
	// Gamma Correction
	{
		color = pow(color, vec3(g_InvGamma));
	}

	out_FragColor = vec4(color, 1.0f);
}
