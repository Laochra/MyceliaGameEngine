#version 460

layout( location = 0) in vec4 Position;
layout( location = 4) in vec2 TexCoords;

out vec2 FragTexCoords;

void main()
{
	gl_Position = vec4(Position.x, Position.z, 0.0, 1.0);
	FragTexCoords = TexCoords;
}