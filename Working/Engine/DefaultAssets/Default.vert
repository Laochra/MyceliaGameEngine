#version 460

layout( location = 0) in vec4 Position;
layout( location = 1) in vec4 Normal;
layout( location = 2) in vec4 Tangent;
layout( location = 3) in vec4 BiTangent;
layout( location = 4) in vec2 TexCoords;

out vec3 FragPos;

out vec3 N;
out mat3 TBN;

out vec2 vTexCoords;

uniform mat4 ProjectionViewModel;

uniform mat4 ModelMatrix;

void main()
{	
	FragPos = (ModelMatrix * Position).xyz;
	
	N = normalize((ModelMatrix * Normal).xyz);
	vec3 T = normalize((ModelMatrix * Tangent).xyz);
	vec3 B = normalize((ModelMatrix * BiTangent).xyz);
	
	TBN = transpose(mat3(T, B, N));

	vTexCoords = TexCoords;
	gl_Position = ProjectionViewModel * Position;
}