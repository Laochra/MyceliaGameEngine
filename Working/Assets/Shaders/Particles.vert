#version 460

// Active status is packed into the 4th component of position since it will always be 1.0 otherwise
layout (location = 0) in vec4 Position;
layout (location = 1) in vec4 VertColour;

uniform mat4 PVMatrix;

out vec4 Colour;

void main() // Vertex
{
	if (Position.a == 0)
	{
		gl_Position = vec4(0.0, 0.0, 1.0, 0.0);
		Colour = vec4(0.0);
		return;
	}

	gl_Position = PVMatrix * vec4(Position.xyz, 1.0);
	
	Colour = VertColour;
}
