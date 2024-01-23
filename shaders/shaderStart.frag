#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec3 lightPosEye;
in vec3 lightPosEye1;
in vec3 lightPosEye2;
in vec3 lightPosEye3;
in vec3 lightPosEye4;
in vec3 lightPosEye5;
in vec3 lightPosEye6;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform float fogDensity;

uniform vec3 pointLight;
uniform vec3 pointLightColor;
uniform vec3 pointLight1;
uniform vec3 pointLightColor1;
uniform vec3 pointLight2;
uniform vec3 pointLightColor2;
uniform vec3 pointLight3;
uniform vec3 pointLightColor3;
uniform vec3 pointLight4;
uniform vec3 pointLightColor4;
uniform vec3 pointLight5;
uniform vec3 pointLightColor5;
uniform vec3 pointLightColor6;
uniform int night;
uniform float intensity;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.14f;
float quadratic = 0.07f;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);

	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);

		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;

}

float computeShadow()
{
	vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	normalizedCoords = normalizedCoords * 0.5 + 0.5;
	if (normalizedCoords.z > 1.0f)
		return 0.0f;
	float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
	float currentDepth = normalizedCoords.z;
	float bias = 0.0005f;
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
	return shadow;
}

float computeFog()
{
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
 
 return clamp(fogFactor, 0.0f, 1.0f);
}

void computePointLight(vec3 lPE, vec3 pointColor){
	//transform normal
	vec3 normalEye = normalize(fNormal);

	//compute light direction
	vec3 lightDirN = normalize(lPE - fPosEye.xyz);
	
	//compute view direction 
	vec3 viewDirN = normalize(-fPosEye.xyz);

	vec3 halfVector = normalize(lightDirN + viewDirN);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);

	//compute distance to light
	float dist = length(lPE - fPosEye.xyz);

	//compute attenuation
	float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));


	vec3 ambient1 = att * ambientStrength * pointColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffuse1 = att * max(dot(normalEye, lightDirN), 0.0f) * pointColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 specular1 = att * specularStrength * specCoeff * pointColor * texture(specularTexture, fTexCoords).rgb;

	ambient += ambient1;
	diffuse += diffuse1;
	specular += specular1;
}



void computeFireLight(vec3 lPE, vec3 pointColor){
	//transform normal
	vec3 normalEye = normalize(fNormal);

	//compute light direction
	vec3 lightDirN = normalize(lPE - fPosEye.xyz);
	
	//compute view direction 
	vec3 viewDirN = normalize(-fPosEye.xyz);

	vec3 halfVector = normalize(lightDirN + viewDirN);
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);

	//compute distance to light
	float dist = length(lPE - fPosEye.xyz);

	float constantFire = 1.0f;
	float linearFire = 0.0003363f * intensity * intensity - 0.03262f * intensity + 0.8093;
	float quadraticFire = 0.001095f * intensity * intensity - 0.0994f * intensity + 1.995;

	//compute attenuation
	float att = 1.0f / (constantFire + linearFire * dist + quadraticFire * (dist * dist));
	
	vec3 ambient1 = att * ambientStrength * pointColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 diffuse1 = att * max(dot(normalEye, lightDirN), 0.0f) * pointColor * texture(diffuseTexture, fTexCoords).rgb;
	vec3 specular1 = att * specularStrength * specCoeff * pointColor * texture(specularTexture, fTexCoords).rgb;

	ambient += ambient1;
	diffuse += diffuse1;
	specular += specular1;
}

void main() 
{
	computeLightComponents();
	
	vec3 baseColor = vec3(0.9f, 0.35f, 0.0f);//orange
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	if(night){
		computeFireLight(lightPosEye, pointLightColor);
		computePointLight(lightPosEye1, pointLightColor1);
		computePointLight(lightPosEye2, pointLightColor2);
		computePointLight(lightPosEye3, pointLightColor3);
		computePointLight(lightPosEye4, pointLightColor4);
		computePointLight(lightPosEye5, pointLightColor5);
		computePointLight(lightPosEye6, pointLightColor6);

	}

	float shadow = night? 0.0 : computeShadow();

	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);

   	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	vec4 colorFromTexture = texture(diffuseTexture, fTexCoords);
	if(colorFromTexture.a < 0.1)
		discard;

    	fColor = fogColor * (1 - fogFactor) + vec4(color, 1.0f) * fogFactor;
}
