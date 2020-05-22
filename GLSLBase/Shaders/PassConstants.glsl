

layout(std140, binding = 0) uniform PassConstants
{
	mat4 g_View;
	mat4 g_InvView;
	mat4 g_Proj;
	mat4 g_InvProj;
	mat4 g_ViewProj;
	mat4 g_InvViewProj;
	vec4 g_Ambient;
	vec3 g_EyePos;
	float g_DeltaTime;
	vec2 g_ScreenDimension;
	vec2 g_InvScreenDimension;
	vec2 g_InvHalfScreenDimension;
	float g_Near;
	float g_Far;
};

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * g_Near * g_Far) / (g_Far + g_Near - z * (g_Far - g_Near));	
}

vec3 FragmentToWorldPos(vec2 texcoord, float depth)
{
	vec4 ndc = vec4(texcoord * 2.0f - 1.0f, depth * 2.0f - 1.0f , 1.0f);
	vec4 pos = g_InvViewProj * ndc;

	return pos.xyz / pos.w;
}