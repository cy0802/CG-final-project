#version 330 core

struct Light {
	vec3 position;
    vec3 color;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 calcLight(Light light, vec3 normal, vec3 viewDir);

out vec4 FragColor;
in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texture_;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

uniform bool useNormalMap;
uniform sampler2D normalMap;

void main(){
    vec3 normalized;
    if(useNormalMap){
        vec3 _normal = texture(normalMap, texCoord).xyz;
        normalized = normalize(_normal * 2.0 - 1.0);
    } else {
        normalized = normalize(normal);
    }
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 result = calcLight(light, normalized, viewDir);
	FragColor = texture(texture_, texCoord) * vec4(result * 0.7, 1.0);
}

vec3 calcLight(Light light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    
    vec3 diffuse = 2 * max(dot(normal, lightDir), 0.0) * material.diffuse; // * light.color
    
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * material.specular * light.color;  

    vec3 ambient = light.color * material.ambient;
    vec3 result = (ambient + diffuse + specular);
    // result = diffuse;
    return result;
}