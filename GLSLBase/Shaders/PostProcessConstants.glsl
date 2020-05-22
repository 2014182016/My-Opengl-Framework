
layout(std140, binding = 2) uniform RenderingConstants
{
	float g_InvGamma;
	float g_Exposure;
	float g_ScatteringTau;
	float g_ScatteringDensity;
	float g_ScatteringZFar;
	float g_Anisotropy;
	float g_Decay;
	float g_Density;
	float g_Weight;
	float g_ScatteringExposure;
	float g_MaxTexDistance;
};