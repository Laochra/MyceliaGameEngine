#version 460

// Active status is packed into the 4th component of position since it will always be 1.0 otherwise
layout (location = 0) in vec4 Position;
layout (location = 1) in vec4 VertColour;

uniform mat4 ProjectionViewModel;

out vec4 GeomColour;
out mat4 PVM;

void main() // Vertex
{
	if (Position.a == 0)
	{
		gl_Position = vec4(0.0, 0.0, 1.0, 0.0);
		GeomColour = vec4(0.0);
		return;
	}

	gl_Position = vec4(Position.xyz, 1.0);
	
	GeomColour = VertColour;
	PVM = ProjectionViewModel;
}
