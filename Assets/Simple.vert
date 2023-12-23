#version 460

layout( location = 0) in vec4 Position;

uniform mat4 ProjectionViewModel;

void main()
{
	gl_Position = ProjectionViewModel * Position;
}