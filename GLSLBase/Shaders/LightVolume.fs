#version 450 core

uniform int g_LightIndex;

layout(location = 0) out float out_LightIndex;

void main()
{
	float mask = 1 << g_LightIndex;
	out_LightIndex = mask;
}
