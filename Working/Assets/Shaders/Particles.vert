#version 460

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 VertColour;

out vec4 Colour;

void main() // Vertex
{
	gl_Position = Position;
	Colour = VertColour;
}
