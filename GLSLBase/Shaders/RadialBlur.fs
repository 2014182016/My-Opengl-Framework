#version 450 core

#include "Lighting.glsl"
#include "PostProcessConstants.glsl"

in VertexOut
{
	vec2 texcoord;
}fs_in;

out vec4 out_FragColor;

uniform sampler2D g_LightScatteringTexture;
uniform vec2 g_LightPosH;

const int MAX_SAMPLES = 30;

void main()
{
	vec2 delta = vec2(fs_in.texcoord - g_LightPosH);
	if(length(delta) > g_MaxTexDistance)
	{
		delta = normalize(delta) * g_MaxTexDistance;
	}
	delta *= 1.0f / float(MAX_SAMPLES + 1) * g_Density;
	vec2 currTex = fs_in.texcoord;
	float illuminationDecay = 1.0f;

	vec3 color = vec3(0.0f);
	for(int i = 0; i < MAX_SAMPLES; ++i, currTex -= delta, illuminationDecay *= g_Decay)
	{
		vec3 scattering = texture(g_LightScatteringTexture, currTex).rgb;
		scattering *= illuminationDecay * g_Weight;
		color += scattering;
	}

    out_FragColor = vec4(color * g_Exposure, 1.0);
}
