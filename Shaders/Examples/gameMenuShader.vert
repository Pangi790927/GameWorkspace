#version 130
 
uniform mat4 worldMatrix, viewMatrix, projectionMatrix;
 
varying vec4 color;

void main()
{
    color = gl_Color;
    color.a = 1.0f;
    gl_Position = projectionMatrix * viewMatrix * worldMatrix * gl_Vertex;
}