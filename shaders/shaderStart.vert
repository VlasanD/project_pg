#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormal;
out vec4 fPosEye;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;
out vec3 lightPosEye;
out vec3 lightPosEye1;
out vec3 lightPosEye2;
out vec3 lightPosEye3;
out vec3 lightPosEye4;
out vec3 lightPosEye5;
out vec3 lightPosEye6;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;
uniform vec3 pointLight;
uniform vec3 pointLight1;
uniform vec3 pointLight2;
uniform vec3 pointLight3;
uniform vec3 pointLight4;
uniform vec3 pointLight5;
uniform vec3 pointLight6;

void main() 
{
	//compute eye space coordinates
	fPosEye = view * model * vec4(vPosition, 1.0f);
	fNormal = normalize(normalMatrix * vNormal);
	fTexCoords = vTexCoords;
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);

	lightPosEye = (view * model * vec4(pointLight, 1.0f)).xyz;
	lightPosEye1 = (view * model * vec4(pointLight1, 1.0f)).xyz;
	lightPosEye2 = (view * model * vec4(pointLight2, 1.0f)).xyz;
	lightPosEye3 = (view * model * vec4(pointLight3, 1.0f)).xyz;
	lightPosEye4 = (view * model * vec4(pointLight4, 1.0f)).xyz;
	lightPosEye5 = (view * model * vec4(pointLight5, 1.0f)).xyz;
	lightPosEye6 = (view * model * vec4(pointLight6, 1.0f)).xyz;

	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);

}
