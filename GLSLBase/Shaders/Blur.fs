#version 450 core

in VertexOut
{
	vec2 texcoord;
}fs_in;

out vec4 out_FragColor;

uniform sampler2D g_Texture;
uniform bool g_Horizontal;
uniform	int g_BlurRadius;
uniform	float g_Weights[5]; // MAX_RADIUS

void main()
{
	vec2 texOffset = 1.0f / textureSize(g_Texture, 0);
	vec3 result = texture(g_Texture, fs_in.texcoord).rgb * g_Weights[0];

	for(int i = 1; i < g_BlurRadius; ++i)
	{
		if(g_Horizontal)
		{
			result += texture(g_Texture,  fs_in.texcoord + vec2(texOffset.x * i, 0.0f)).rgb * g_Weights[i];
			result += texture(g_Texture,  fs_in.texcoord - vec2(texOffset.x * i, 0.0f)).rgb * g_Weights[i];
		}
		else
		{
			result += texture(g_Texture,  fs_in.texcoord + vec2(0.0f, texOffset.y * i)).rgb * g_Weights[i];
			result += texture(g_Texture,  fs_in.texcoord - vec2(0.0f, texOffset.y * i)).rgb * g_Weights[i];
		}
	}

    out_FragColor = vec4(result, 1.0);
}
