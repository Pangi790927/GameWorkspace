#version 130

uniform vec4 uColor;

in vec2 texCoord;
uniform sampler2D texture;

void main()
{
	gl_FragColor = texture2D(texture, texCoord);
}