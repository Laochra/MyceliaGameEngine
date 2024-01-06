#version 460

layout( location = 0) in vec4 Position;
layout( location = 1) in vec4 Normal;
layout( location = 2) in vec2 TexCoords;

out vec3 vNormal;
out vec2 vTexCoords;

uniform mat4 ProjectionViewModel;

uniform mat4 ModelMatrix;

void main()
{
	vNormal = (ModelMatrix * Normal).xyz;
	vTexCoords = TexCoords;
	gl_Position = ProjectionViewModel * Position;
}