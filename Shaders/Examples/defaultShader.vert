#version 130

uniform mat4 worldMatrix, viewMatrix, projectionMatrix;
uniform vec4 uColor;

void main()
{
	gl_Position = projectionMatrix * viewMatrix * worldMatrix * gl_Vertex;
}