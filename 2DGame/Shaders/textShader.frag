#version 130

uniform vec4 uColor;

in vec3 normal;
in vec4 color;
in vec2 texCoord;
in vec4 position;

uniform sampler2D texture;
uniform float alpha;

void main()
{
	vec4 chr = texture2D(texture, texCoord);

	gl_FragColor = vec4(color.r, color.g, color.b, chr.r + alpha);
	// gl_FragColor = vec4(1, 0, 1, 1);
}