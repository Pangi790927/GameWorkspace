#version 130

uniform sampler2D texture;

in vec3 normal;
in vec4 color;
in vec2 texCoord;
in vec4 position;

uniform vec3 lightDir = vec3(1, 1, 3);

vec3 brightnessContrast(vec3 color, float brightness, float contrast)
{
    return (color - 0.5) * contrast + 0.5 + brightness;
}

void main()
{
	float ambient = 0.2;
	float diffuze = 0.8;

	/// lighting
	diffuze *= max(dot(normal, lightDir), 0.0f);
	vec4 fragColor = texture2D(texture, texCoord) * (diffuze + ambient) + color;	

	/// tone map?
	fragColor = fragColor / (1 + fragColor);
	fragColor = pow(fragColor, vec4(1 / 2.2));

	fragColor = vec4(brightnessContrast(vec3(fragColor), -0.1, 1.3), 1.0);

	fragColor.a = 1.0f;

    // gl_FragColor = vec4(position); 
    // gl_FragColor = vec4(normal, 0); 
    // gl_FragColor = vec4(texCoord, 0, 0); 
    gl_FragColor = vec4(brightnessContrast(vec3(fragColor), -0.1, 1.3), 1.0)
    		* (diffuze + ambient); 
}