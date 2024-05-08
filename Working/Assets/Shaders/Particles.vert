#version 460

layout (location = 0) in vec4 Position;
layout (location = 1) in vec4 VertColour;

uniform mat4 PVMatrix;

out vec4 Colour;

void main() // Vertex
{
	gl_Position = PVMatrix * Position;
	Colour = VertColour;
}
