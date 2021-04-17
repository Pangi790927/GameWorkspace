#version 130

in vec4 color;
in vec2 texCoord;

uniform sampler2D texture;

/// flat 
in vec3 normal;

uniform vec3 lightDir = vec3(1, 1, 0);

vec3 brightnessContrast(vec3 color, float brightness, float contrast)
{
    return (color - 0.5) * contrast + 0.5 + brightness;
}

void main()
{
	float ambient = 0.0f;
	float diffuze = 0.8f;

	/// lighting
	diffuze *= max(dot(normal, lightDir), 0.0f);
	vec4 fragColor = texture2D(texture, texCoord) * (diffuze + ambient);	

	/// tone map?
	fragColor = fragColor / (1 + fragColor);
	fragColor = pow(fragColor, vec4(1 / 2.2));

	fragColor = vec4(brightnessContrast(vec3(fragColor), -0.1, 1.3), 1.0);

	fragColor.a = 1.0f;

    gl_FragColor = fragColor; 
}