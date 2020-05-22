#version 450 core

#include "Lighting.glsl"
#include "PassConstants.glsl"
#include "PostProcessConstants.glsl"

in VertexOut
{
	vec2 texcoord;
}fs_in;

out vec4 out_FragColor;

uniform sampler2D g_DepthTexture;
uniform int g_LightIndex;

const int MAX_SAMPLES = 15;

float Random(vec2 co) 
{
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float HeyeyGreensteinScattering(vec3 inDir, vec3 outDir)
{
	float cosAngle = dot(inDir, outDir) / (length(inDir) * length(outDir));
	float nom = 1.0f - g_Anisotropy * g_Anisotropy;
	float denom = 4.0f * PI * pow(1.0f + g_Anisotropy * g_Anisotropy - (2.0f * g_Anisotropy) * cosAngle, 1.5f);
	
	return nom / denom;
}

float SchlickScattering(vec3 inDir, vec3 outDir)
{
	float cosAngle = dot(inDir, outDir) / (length(inDir) * length(outDir));
	float nom = 1.0f - g_Anisotropy * g_Anisotropy;
	float denom = 1.0f + g_Anisotropy * cosAngle;
	float denom2 = denom * denom;

	return nom / denom2;
}

float RayleighScattering(vec3 inDir, vec3 outDir)
{
	float cosAngle = dot(inDir, outDir) / (length(inDir) * length(outDir));
	float nom = 3.0f * (1.0f + cosAngle * cosAngle);
	float denom = 16.0f * PI;

	return nom / denom;
}

float MieHazy(vec3 inDir, vec3 outDir)
{
	float cosAngle = dot(inDir, outDir) / (length(inDir) * length(outDir));
	float x = (1.0f + cosAngle) / 2.0f;
	float x2 = x *x;
	float x4 = x2 * x2;
	float x8 = x4 * x4;
	//float x8 = pow(x, 8);
	float nom = 0.5f + 4.5f * x8;
	float factor = 0.25f * INV_PI;

	return nom * factor;
}

float MieMurky(vec3 inDir, vec3 outDir)
{
	float cosAngle = dot(inDir, outDir) / (length(inDir) * length(outDir));
	float x = (1.0f + cosAngle) / 2.0f;
	float x2 = x *x;
	float x4 = x2 * x2;
	float x8 = x4 * x4;
	float x16 = x8 * x8;
	float x32 = x16 * x16;
	//float x8 = pow(x, 32);
	float nom = 0.5f + 16.5f * x32;
	float factor = 0.25f * INV_PI;

	return nom * factor;
}

vec3 VolumeticLightScattering(Light light, vec3 pos)
{
	vec3 cameraToFrag = pos - g_EyePos;
	if(length(cameraToFrag) > g_ScatteringZFar)
	{
		cameraToFrag = normalize(cameraToFrag) * g_ScatteringZFar;
	}
	vec3 delta = cameraToFrag / (MAX_SAMPLES + 1);
	vec3 fragToCamera = normalize(g_EyePos - pos);
	vec3 currPos = g_EyePos + delta * Random(pos.xy + pos.z);
	float stepSize = length(delta);

	float intensity = 0.0f;
	for(int i = 0; i < MAX_SAMPLES; ++i, currPos += delta)
	{
		float shadowFactor = CalcShadowFactorWithoutPCF(light, currPos, g_EyePos);
		if(shadowFactor <= 0.0f)
			continue;

		vec3 lightToPos = currPos - light.m_Position;
		float d = length(lightToPos);
		float invD = 1.0f / d;

		float omega = 0.25f * INV_PI * invD * invD;
		float Lin = exp(-g_ScatteringTau * (d + distance(currPos, g_EyePos))) * shadowFactor * omega;
		float Li = Lin * RayleighScattering(lightToPos * invD, fragToCamera);
		intensity += Li * stepSize * CalcLightAttenuation(light, currPos);
	}

	vec3 result = intensity * g_ScatteringDensity * g_ScatteringTau * light.m_Strength;

	return result;
}

vec3 VolumeticDirectionalLightScattering(Light light, vec3 pos)
{
	vec3 cameraToFrag = pos - g_EyePos;
	if(length(cameraToFrag) > g_ScatteringZFar)
	{
		cameraToFrag = normalize(cameraToFrag) * g_ScatteringZFar;
	}
	vec3 delta = cameraToFrag / (MAX_SAMPLES + 1);
	cameraToFrag = normalize(cameraToFrag);
	vec3 currPos = g_EyePos + delta * Random(pos.xy + pos.z);

	float intensity = 0.0f;
	for(int i = 0; i < MAX_SAMPLES; ++i, currPos += delta)
	{
		float shadowFactor = CalcShadowFactorWithoutPCF(light, currPos, g_EyePos);
		if(shadowFactor <= 0.0f)
			continue;

		intensity += shadowFactor * HeyeyGreensteinScattering(light.m_Direction, -cameraToFrag);
	}

	vec3 result = intensity * light.m_Strength;
	result /= float(MAX_SAMPLES);

	return result;
}

void main()
{
	float depth = texture(g_DepthTexture, fs_in.texcoord).r;
	vec3 pos = FragmentToWorldPos(fs_in.texcoord, depth);

	vec3 color = VolumeticLightScattering(g_Lights[g_LightIndex], pos);

	out_FragColor = vec4(color, 1.0f);
}
