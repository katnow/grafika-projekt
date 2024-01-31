#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 transformation;
uniform mat4 modelMatrix;

out vec3 vecNormal;
out vec3 worldPos;
out vec2 vecTex;

void main() {
    vec3 vecPos = vertexPosition.xzy; 
    worldPos = (modelMatrix * vec4(vecPos, 1)).xyz;
    vecNormal = (modelMatrix * vec4(vertexNormal, 0)).xyz;
    vecTex = vertexTexCoord;

    gl_Position = transformation * vec4(vecPos, 1.0);
}
