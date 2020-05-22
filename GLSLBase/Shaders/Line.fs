#version 450 core

in VertexOut
{
	vec3 color;
}fs_in;

out vec4 out_FragColor;

void main()
{
	out_FragColor = vec4(fs_in.color, 1.0f);
}
