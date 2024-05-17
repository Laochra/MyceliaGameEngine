#version 460

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

uniform mat4 ProjectionView;
uniform vec3 CameraPosition;
uniform vec3 CameraUp;

in vec4 GeomColour[];
in mat4 PVM[];

out vec4 Colour;
out vec2 FragTexCoords;

void main() // Geometry
{	
	if (gl_in[0].gl_Position.a == 0.0)
	{
		gl_Position = gl_in[0].gl_Position;
		Colour = vec4(0.0);
		FragTexCoords = vec2(0.0);
		EmitVertex();
		EmitVertex();
		EmitVertex();
		EmitVertex();
		return;
	}
	
	vec3 position = gl_in[0].gl_Position.xyz;
	vec3 cameraToPoint = normalize(position - CameraPosition);
	
	vec3 up = vec3(0, 1, 0);
	vec3 right = normalize(cross(cameraToPoint, up));
	
	vec3 UP = up * vec3(0.5);
	vec3 RIGHT = vec3(right.x * 0.5, right.y * 0.5, right.z * 0.5);
	
	Colour = GeomColour[0];
	
	// Bottom Left
	gl_Position 	= PVM[0] * vec4(position + -UP + -RIGHT, 1.0);
	FragTexCoords 	= vec2(0.0, 0.0);
	EmitVertex();
	
	// Bottom Right
	gl_Position 	= PVM[0] * vec4(position + -UP +  RIGHT, 1.0);
	FragTexCoords 	= vec2(1.0, 0.0);
	EmitVertex();
	
	// Top Left
	gl_Position 	= PVM[0] * vec4(position +  UP + -RIGHT, 1.0);
	FragTexCoords 	= vec2(0.0, 1.0);
	EmitVertex();
	
	// Top Right
	gl_Position 	= PVM[0] * vec4(position +  UP +  RIGHT, 1.0);
	FragTexCoords 	= vec2(1.0, 1.0);
	EmitVertex();
	
	EndPrimitive();
}
