
#define MAX_LIGHT 3

#define DIRECTIONAL_LIGHT 0
#define SPOT_LIGHT 1
#define POINT_LIGHT 2

//#define BLINN_PHONG
#define PBR

#define NUM_CASCADES 3

const float g_Bias = 0.001f;
const float g_ShadowMapSize = 1000.0f;
const float g_InvShadowMapSize = 1.0f / g_ShadowMapSize;
const float PI = 3.14159265359f;
const float INV_PI = 0.3183098861f;
const int cubeMapSamples = 20;
const vec3 g_SampleOffsetDirections[cubeMapSamples] = vec3[](
	vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
	vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
	vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
	vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));

struct MaterialData
{
	vec3 m_Diffuse;
	float m_Metallic;
	vec3 m_Specular;
	float m_Roughness;
};

struct Light
{
	mat4 m_Transform[3];
	
	vec3 m_Strength;
	float m_FalloffStart;

	vec3 m_Direction;
	float m_FalloffEnd;

	vec3 m_Position;
	float m_SpotAngle;

	int m_Type;
	int m_LightIndex;
	int m_ShadowMapIndex;
	bool m_Enabled;
};

layout(std140, binding = 1) uniform Lights
{
	Light g_Lights[MAX_LIGHT];
};

uniform float g_CascadeDists[NUM_CASCADES + 1];
uniform float g_FarPlane;
uniform float g_NearPlane;

layout(binding = 16) uniform sampler2DArrayShadow g_ShadowMapArray;
layout(binding = 17) uniform samplerCubeArray g_ShadowCubeMapArray;

float VectorToDepthValue(vec3 vec)
{
	vec3 absVec = abs(vec);
	float nfSub = g_FarPlane - g_NearPlane;
	float localZcomp = max(absVec.x, max(absVec.y, absVec.z));
	float normZcomp = ((g_FarPlane + g_NearPlane) / nfSub) - ((2.0f * g_FarPlane * g_NearPlane) / nfSub);

	return (normZcomp / localZcomp + 1.0f) * 0.5f;
}

// Normal Distribution Function
// 중간 벡터 h에 정확하게 정렬된 미세면의 상대 표면적을 통계적으로 근사한다.
float DistributionGGX(vec3 normal, vec3 halfVec, float roughness)
{
	// a²/ PI((n·h)²(a²- 1) + 1)²

	float a = roughness * roughness;
	float a2 = a * a;
	float ndoth = max(dot(normal, halfVec) , 0.0f);
	float ndoth2 = ndoth * ndoth;

	float nom = a2;
	float denom = (ndoth2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}

// Geometry Function에서 사용할 기하학 함수
float GeometrySchlickGGX(float ndotv, float roughness)
{
	// (n·v) / (n·v)(1 - k) + k

	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;

	float nom = ndotv;
	float denom = ndotv * (1.0f - k) + k;

	return nom / denom;
}

// Geometry Function
// 미세면의 세부 사항이 서로 겹쳐져 광선이 차단되는 상대 표면적을 통계적으로 근사한다.
float GeometrySmith(vec3 normal, vec3 view, vec3 lightVec, float roughness)
{
	// Gsub(n, v, k) * Gsub(n, l, k)

	float ndotl = max(dot(normal, lightVec), 0.0f);
	float ndotv = max(dot(normal, view), 0.0f);
	float ggx1 = GeometrySchlickGGX(ndotl, roughness);
	float ggx2 = GeometrySchlickGGX(ndotv, roughness);

	return ggx1 * ggx2;
}

// Fresnel Equation
// 굴절되는 빛에 반사되는 빛의 비율을 나타내며 표면을 바라보는 각도에 따라 다르다.
// 시야각이 90도일 때 반사가 가장 강해진다.
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	// F0 + (1.0f - F0) * pow(1.0f - (h·v), 5)
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

// 프레넬 방적식의 슐릭 근사를 사용한다.
// 즉, 법선이 n인 표면에서 프레넬 효과에 의해 반사되는 빛의 비율을
// 빛 벡터 L과 표면 법선 n 사이의 각도에 근거해서 근사한다.
vec3 SchlickFresnel(vec3 r0, vec3 normal, vec3 lightVec)
{
	float cosIncidentAngle = clamp(dot(normal, lightVec), 0.0f, 1.0f);

	float f0 = 1.0f - cosIncidentAngle;
	vec3 reflectPercent = r0 + (1.0f - r0) * pow(f0, 5);

	return reflectPercent;
}

// Bidirectional Reflective Distribution Fuction(양방향 반사 분포 함수) 
// Cook-Torrance BRDF 사용
// 물질 특성을 고려하여 불투명 표면의 최종 반사광에 각 개별 광선이
// 얼마나 많이 기여하는지 대략적으로 나타낸다.
vec3 BRDF(MaterialData material, vec3 normal, vec3 toEye, vec3 halfVec, vec3 lightVec, vec3 ks)
{
	// DFG / 4(w0·n)(wi·n);

	float D  = DistributionGGX(normal, halfVec, material.m_Roughness);
	float G = GeometrySmith(normal, toEye, lightVec, material.m_Roughness);
	vec3 F = ks; // 미리 계산된 Fresnel

	vec3 nom = D * G * F;
	float denom = 4.0f * max(dot(normal, toEye), 0.0f) * max(dot(normal, lightVec), 0.0f);

	return nom / max(denom, 0.001f);
}

// 미세면 표면 모델, 에너지 보존 법칙 ,물리적 기반 BRDF를 사용하여
// 물리적인 매개 변수를 사용하여 실제 물리학과 가까운 렌더링을 수행한다.
vec3 PBRLighting(MaterialData material, vec3 lightStrength, vec3 lightVec, vec3 normal, vec3 toEye)
{
	vec3 halfVec = normalize(toEye + lightVec);
	vec3 F0 = mix(vec3(0.04f), material.m_Diffuse, material.m_Metallic);

	vec3 ks = SchlickFresnel(F0, normal, lightVec);
	vec3 kd = vec3(1.0f) - ks;
	kd *= 1.0f - material.m_Metallic;

	vec3 specular = BRDF(material, normal, toEye, halfVec, lightVec, ks);
	specular *= material.m_Specular;
	
	return (kd * material.m_Diffuse / PI + specular) * lightStrength;
}

// 점광과 점적광에 적용하는 선형 감쇠 계수를 계산한다.
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	return clamp((falloffEnd - d) / (falloffEnd - falloffStart), 0.0f, 1.0f);
}

// 눈에 도달한 반사광의 양을 계산한다.
// 즉, 분산 반사와 반영 반사의 합을 구한다.
vec3 BlinnPhong(MaterialData material, vec3 lightStrength, vec3 lightVec, vec3 normal, vec3 toEye)
{
	const float shininess = 1.0f - material.m_Roughness;
	const float m = shininess * 256.0f;
	vec3 halfVec = normalize(toEye + lightVec);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
	vec3 specularFactor = SchlickFresnel(material.m_Specular, halfVec, lightVec);

	vec3 specAlbedo = specularFactor * roughnessFactor;

	return (material.m_Diffuse.rgb + specAlbedo) * lightStrength;
}

vec3 ComputeDirectionalLight(Light light, MaterialData material, vec3 normal, vec3 toEye)
{
	// 빛 벡터는 광선들이 나아가는 방향의 반대 방향을 가리킨다.
	vec3 lightVec = -light.m_Direction;

	// 람베트르 코사인 법칙에 따라 빛의 세기를 줄인다.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	vec3 lightStrength = light.m_Strength * ndotl;

	#ifdef BLINN_PHONG
	return BlinnPhong(material, lightStrength, lightVec, normal, toEye);
	#endif
	return PBRLighting(material, lightStrength, lightVec, normal, toEye);
}

vec3 ComputePointLight(Light light, MaterialData material, vec3 pos, vec3 normal, vec3 toEye)
{
	// 표면에서 광원으로의 벡터
	vec3 lightVec = light.m_Position - pos;

	// 광원과 표면 사이의 거리
	float d = length(lightVec);

	// 범위 판정
	if(d > light.m_FalloffEnd)
		return vec3(0.0f, 0.0f, 0.0f);

	// 빛 벡터를 정규화한다.
	lightVec /= d;

	// 람베트르 코사인 법칙에 따라 빛의 세기를 줄인다.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	vec3 lightStrength = light.m_Strength * ndotl;

	// 거리에 따라 빛을 감쇠한다.
	lightStrength *= CalcAttenuation(d, light.m_FalloffStart, light.m_FalloffEnd);
	
	#ifdef BLINN_PHONG
	return BlinnPhong(material, lightStrength, lightVec, normal, toEye);
	#endif
	return PBRLighting(material, lightStrength, lightVec, normal, toEye);
}

vec3 ComputeSpotLight(Light light, MaterialData material, vec3 pos, vec3 normal, vec3 toEye)
{
	// 표면에서 광원으로의 벡터
	vec3 lightVec = light.m_Position - pos;

	// 광원과 표면 사이의 거리
	float d = length(lightVec);

	// 범위 판정
	if(d > light.m_FalloffEnd)
		return vec3(0.0f, 0.0f, 0.0f);

	// 빛 벡터를 정규화한다.
	lightVec /= d;

	// 람베트르 코사인 법칙에 따라 빛의 세기를 줄인다.
	float ndotl = max(dot(lightVec, normal), 0.0f);
	vec3 lightStrength = light.m_Strength * ndotl;

	// 거리에 따라 빛을 감쇠한다.
	lightStrength *= CalcAttenuation(d, light.m_FalloffStart, light.m_FalloffEnd);
	
	float minCos = cos(radians(light.m_SpotAngle));
	float maxCos = mix(minCos, 1.0f, 0.5f);
	float cosAngle = max(dot(-lightVec, light.m_Direction), 0.0f);

	// Angle에 따라 빛을 감쇠한다.
	lightStrength *= smoothstep(minCos, maxCos, cosAngle);

	#ifdef BLINN_PHONG
	return BlinnPhong(material, lightStrength, lightVec, normal, toEye);
	#endif
	return PBRLighting(material, lightStrength, lightVec, normal, toEye);
}

float CalcLightAttenuation(Light light, vec3 pos)
{
	float lightInensity = 1.0f;

	switch(light.m_Type)
	{
		case POINT_LIGHT:
		{
			float d = length(light.m_Position - pos);
			lightInensity *= CalcAttenuation(d, light.m_FalloffStart, light.m_FalloffEnd);
			break;
		}
		case SPOT_LIGHT:
		{
			float d = length(light.m_Position - pos);
			lightInensity *= CalcAttenuation(d, light.m_FalloffStart, light.m_FalloffEnd);
			break;
		}
	}

	return lightInensity;
}

float CalcShadowFactor(Light light, vec3 pos, vec3 eyePos)
{
	float result = 0.0f;

	switch(light.m_Type)
	{
		case DIRECTIONAL_LIGHT:
		{
			float dist = distance(pos, eyePos);
			int cascadeIndex = -1;
			for(int i = 0; i < NUM_CASCADES; ++i)
			{
				if(dist < g_CascadeDists[i + 1] && cascadeIndex == -1)
				{
					cascadeIndex = i;
				}
			}

			vec4 shadowPosH = light.m_Transform[cascadeIndex] * vec4(pos, 1.0f);
			shadowPosH.xyz /= shadowPosH.w;
			shadowPosH.xyz = shadowPosH.xyz * 0.5f + 0.5f;

			if(shadowPosH.x < 0.0f || shadowPosH.x > 1.0f || shadowPosH.y < 0.0f || shadowPosH.y > 1.0f || shadowPosH.z > 1.0f)
				return 1.0f;

			float shadowMapIndex = float(light.m_ShadowMapIndex + cascadeIndex);
			float currentDepth = shadowPosH.z;
			for(int x = -1; x <= 1; ++x)
			{
				for(int y = -1; y <= 1; ++y)
				{
					vec4 shadowTex = vec4(shadowPosH.xy + vec2(x, y) * g_InvShadowMapSize, shadowMapIndex, currentDepth - g_Bias);
					result += texture(g_ShadowMapArray, shadowTex).r;
				}
			}
			
			return result / 9.0f;
		}
		case SPOT_LIGHT:
		{
			vec4 shadowPosH = light.m_Transform[0] * vec4(pos, 1.0f);
			shadowPosH.xyz /= shadowPosH.w;
			shadowPosH.xyz = shadowPosH.xyz * 0.5f + 0.5f;

			if(shadowPosH.x < 0.0f || shadowPosH.x > 1.0f || shadowPosH.y < 0.0f || shadowPosH.y > 1.0f || shadowPosH.z > 1.0f)
				return 1.0f;

			float currentDepth = shadowPosH.z;
			for(int x = -1; x <= 1; ++x)
			{
				for(int y = -1; y <= 1; ++y)
				{
					vec4 shadowTex = vec4(shadowPosH.xy + vec2(x, y) * g_InvShadowMapSize, float(light.m_ShadowMapIndex), currentDepth - g_Bias);
					result += texture(g_ShadowMapArray, shadowTex).r;
				}
			}
			
			return result / 9.0f;
		}
		case POINT_LIGHT:
		{
			vec3 toLight = pos - light.m_Position;
			float currentDepth = length(toLight);
			float viewDistance = length(eyePos - pos);
			float diskRadius = (1.0f + (viewDistance / g_FarPlane)) / 25.0f;
			for(int i = 0; i < cubeMapSamples; ++i)
			{
				vec4 shadowTex = vec4(toLight + g_SampleOffsetDirections[i] * diskRadius, light.m_ShadowMapIndex);
				float closestDepth = texture(g_ShadowCubeMapArray, shadowTex).r;
				closestDepth *= g_FarPlane;
				result += currentDepth - g_Bias <= closestDepth ? 1.0f : 0.0f;
			}

			return result / float(cubeMapSamples);
		}
	}

	return 1.0f;
}

float CalcShadowFactorWithoutPCF(Light light, vec3 pos, vec3 eyePos)
{
	float result = 0.0f;

	switch(light.m_Type)
	{
		case DIRECTIONAL_LIGHT:
		{
			float dist = distance(pos, eyePos);
			int cascadeIndex = -1;
			for(int i = 0; i < NUM_CASCADES; ++i)
			{
				if(dist < g_CascadeDists[i + 1] && cascadeIndex == -1)
				{
					cascadeIndex = i;
				}
			}

			vec4 shadowPosH = light.m_Transform[cascadeIndex] * vec4(pos, 1.0f);
			shadowPosH.xyz /= shadowPosH.w;
			shadowPosH.xyz = shadowPosH.xyz * 0.5f + 0.5f;

			if(shadowPosH.x < 0.0f || shadowPosH.x > 1.0f || shadowPosH.y < 0.0f || shadowPosH.y > 1.0f || shadowPosH.z > 1.0f)
				return 1.0f;

			float shadowMapIndex = float(light.m_ShadowMapIndex + cascadeIndex);
			float currentDepth = shadowPosH.z;
			vec4 shadowTex = vec4(shadowPosH.xy, shadowMapIndex, currentDepth - g_Bias);
			result = texture(g_ShadowMapArray, shadowTex).r;
			
			return result;
		}
		case SPOT_LIGHT:
		{
			vec4 shadowPosH = light.m_Transform[0] * vec4(pos, 1.0f);
			shadowPosH.xyz /= shadowPosH.w;
			shadowPosH.xyz = shadowPosH.xyz * 0.5f + 0.5f;

			if(shadowPosH.x < 0.0f || shadowPosH.x > 1.0f || shadowPosH.y < 0.0f || shadowPosH.y > 1.0f || shadowPosH.z > 1.0f)
				return 0.0f;

			float currentDepth = shadowPosH.z;
			vec4 shadowTex = vec4(shadowPosH.xy, float(light.m_ShadowMapIndex), currentDepth - g_Bias);
			result = texture(g_ShadowMapArray, shadowTex).r;
			
			return result;
		}
		case POINT_LIGHT:
		{
			vec3 toLight = pos - light.m_Position;
			float currentDepth = length(toLight);
			vec4 shadowTex = vec4(toLight, light.m_ShadowMapIndex);
			float closestDepth = texture(g_ShadowCubeMapArray, shadowTex).r;
			closestDepth *= g_FarPlane;
			result = currentDepth - g_Bias <= closestDepth ? 1.0f : 0.0f;

			return result;
		}
	}

	return 1.0f;
}

vec3 ComputeLighting(Light light, MaterialData material, vec3 pos, vec3 normal, vec3 toEye, vec3 eyePos)
{
	if(light.m_Enabled == false)
		return vec3(0.0f);

	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	switch(light.m_Type)
	{
		case DIRECTIONAL_LIGHT:
		{
			result = ComputeDirectionalLight(light, material, normal, toEye);
			break;
		}
		case SPOT_LIGHT:
		{
			result = ComputeSpotLight(light, material, pos, normal, toEye);
			break;
		}
		case POINT_LIGHT:
		{
			result = ComputePointLight(light, material, pos, normal, toEye);
			break;
		}
	}

	float shadowFactor = CalcShadowFactor(light, pos, eyePos);

	return shadowFactor * result;
}

vec3 ComputeLightings(Light lights[MAX_LIGHT], MaterialData material, vec3 pos, vec3 normal, vec3 toEye, vec3 eyePos)
{
	vec3 result = vec3(0.0f, 0.0f, 0.0f);

	for(int i = 0; i < MAX_LIGHT; ++i)
	{
		result += ComputeLighting(lights[i], material, pos, normal, toEye, eyePos);
	}

	return result;
}