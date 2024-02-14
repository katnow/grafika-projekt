#version 430 core

float AMBIENT = 0.5;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform sampler2D colorTexture;

in vec3 vecNormal;
in vec3 worldPos;
in vec2 vecTex;

in vec3 viewDir;

out vec4 outColor;

vec3 phongLight(vec3 lightDir, vec3 color, vec3 normal, vec3 viewDir) {
	float diffuse=max(0,dot(normal,lightDir));

	vec3 R = reflect(-lightDir, normal);  

	float specular = pow(max(dot(viewDir, R), 0.0), 32);

	vec3 resultColor = color*diffuse*lightColor+lightColor*specular;
	return resultColor;
}

void main() {
    vec3 lightDir = normalize(lightPos) - normalize(worldPos);
    vec3 normal = normalize(vecNormal);

    vec4 textureColor = texture(colorTexture, vecTex);
    vec3 sampledColor = textureColor.rgb;

    float diffuse = max(0, dot(normal, lightDir));

    vec3 AMBIENT_C = AMBIENT * sampledColor;
    vec3 illumination = AMBIENT_C + phongLight(lightDir, lightColor, normal, viewDir);
    outColor = vec4(1.0 - exp(-illumination), 1.0);
}
