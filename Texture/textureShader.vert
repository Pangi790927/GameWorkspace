#version 130

uniform mat4 worldMatrix, viewMatrix, projectionMatrix;
uniform vec4 uColor;

out vec2 texCoord;

void main()
{
	texCoord = gl_MultiTexCoord0.st;
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * gl_Vertex;
}