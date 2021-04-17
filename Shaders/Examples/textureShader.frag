#version 130

in vec4 color;
in vec3 normal;
in vec2 texCoord;
in vec4 position;

uniform vec3 lightDir = vec3(1, 1, 0);

uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2D(texture, texCoord);
}