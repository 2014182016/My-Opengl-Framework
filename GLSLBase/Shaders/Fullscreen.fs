#version 450 core

in VertexOut
{
	vec2 texcoord;
}fs_in;

layout(location = 0) out vec4 out_FragColor;

uniform sampler2D g_ScreenTexture;

void main()
{
	out_FragColor.rgb = texture(g_ScreenTexture, fs_in.texcoord).rgb;
	out_FragColor.a = 1.0f;
}
