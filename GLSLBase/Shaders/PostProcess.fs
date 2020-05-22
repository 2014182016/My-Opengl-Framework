#version 450 core

#include "Lighting.glsl"
#include "PostProcessConstants.glsl"
#include "PassConstants.glsl"

in VertexOut
{
	vec2 texcoord;
}fs_in;

out vec4 out_FragColor;

uniform sampler2D g_SceneTexture;
uniform sampler2D g_BloomTexture;
uniform sampler2D g_LightScatteringTexture;

void main()
{
	vec3 color = texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy), 0).rgb;

	// Volumetric Light Scattering
	{
	    //vec2 texelSize = 1.0f / textureSize(g_LightScatteringTexture, 0);
        //vec3 scattering = 0.5f * texture(g_LightScatteringTexture, fs_in.texcoord).rgb;
        //scattering += texture(g_LightScatteringTexture, fs_in.texcoord + vec2(-0.5f * texelSize.x, -texelSize.y)).rgb;
        //scattering += texture(g_LightScatteringTexture, fs_in.texcoord + vec2(texelSize.x, -0.5f * texelSize.y)).rgb;
        //scattering += texture(g_LightScatteringTexture, fs_in.texcoord + vec2(0.5f * texelSize.x, texelSize.y)).rgb;
        //scattering += texture(g_LightScatteringTexture, fs_in.texcoord + vec2(-texelSize.x, 0.5f * texelSize.y)).rgb;
        //color += (2.0f / 9.0f) * scattering;

		color += texture(g_LightScatteringTexture, fs_in.texcoord).rgb;
	}

	// Bloom
	{
		//color += texture(g_BloomTexture, fs_in.texcoord).rgb;
	}

	// Exposure
	{
		//color = vec3(1.0f) - exp(-color * g_Exposure);
	}
	// Gamma Correction
	{
		//color = pow(color, vec3(g_InvGamma));
	}

	out_FragColor = vec4(color, 1.0f);
}
