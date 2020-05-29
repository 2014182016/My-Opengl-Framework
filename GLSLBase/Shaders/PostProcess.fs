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

const float g_FxaaSpanMax = 8.0f;
const float g_FxaaReduceMin = 1.0f / 128.0f;
const float g_FxaaReduceMul = 1.0f / 8.0f;

float Luma(vec3 rgb)
{
	return dot(rgb, vec3(0.299f, 0.587f, 0.114f));
}

void main()
{
	vec3 color = texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy), 0).rgb;

	// FXAA
	{
		float lumaCenter = Luma(color);
		float lumaTopLeft = Luma(texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy) + ivec2(-1, -1), 0).rgb);
		float lumaTopRight = Luma(texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy) + ivec2(1, -1), 0).rgb);
		float lumaBottomLeft = Luma(texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy) + ivec2(-1, 1), 0).rgb);
		float lumaBottomRight = Luma(texelFetch(g_SceneTexture, ivec2(gl_FragCoord.xy) + ivec2(1, 1), 0).rgb);

		vec2 dir;
		dir.x = -((lumaTopLeft + lumaTopRight) - (lumaBottomLeft + lumaBottomRight));
		dir.y = ((lumaTopLeft + lumaBottomLeft) - (lumaTopRight + lumaBottomRight));

		float dirReduce = max((lumaTopLeft + lumaTopRight + lumaBottomLeft + lumaBottomRight) * (g_FxaaReduceMul * 0.25f), g_FxaaReduceMin);
		float inverseDirAdjustment = 1.0f / (min(abs(dir.x), abs(dir.y)) + dirReduce);

		dir = min(vec2(g_FxaaSpanMax, g_FxaaSpanMax), max(vec2(-g_FxaaSpanMax, -g_FxaaSpanMax), dir * inverseDirAdjustment)) * g_InvScreenDimension;

		vec3 result1 = (1.0f / 2.0f) * 
						(texture(g_SceneTexture, fs_in.texcoord + (dir * vec2(1.0f / 3.0f - 0.5f))).xyz +
						texture(g_SceneTexture, fs_in.texcoord + (dir * vec2(2.0f / 3.0f - 0.5f))).xyz);

		vec3 result2 = result1 * (1.0f / 2.0f)  + (1.0f / 4.0f) * 
						(texture(g_SceneTexture, fs_in.texcoord + (dir * vec2(0.0f / 3.0f - 0.5f))).xyz +
						texture(g_SceneTexture, fs_in.texcoord + (dir * vec2(3.0f / 3.0f - 0.5f))).xyz);

		float lumaMin = min(lumaCenter, min(min(lumaTopLeft, lumaTopRight), min(lumaBottomLeft, lumaBottomRight)));
		float lumaMax = max(lumaCenter, max(max(lumaTopLeft, lumaTopRight), max(lumaBottomLeft, lumaBottomRight)));
		float lumaResult2 = Luma(result2);

		if(lumaResult2 < lumaMin || lumaResult2 > lumaMax)
			color = result1;
		else
			color = result2;
	}

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
		color += texture(g_BloomTexture, fs_in.texcoord).rgb;
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
