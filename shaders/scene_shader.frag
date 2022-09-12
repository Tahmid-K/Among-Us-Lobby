#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

in vec3 normal_output;
in vec3 pos_output;

uniform vec3 view_pos;
uniform Material a_material;
uniform Light a_light;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

vec3 phong_illumination_result(vec3 norm, vec3 view_dir);
vec3 normal_result(vec3 norm);

void main() {
    vec3 norm = normalize(normal_output);
    vec3 view_dir = normalize(view_pos - pos_output);
    vec3 result = phong_illumination_result(norm, view_dir);
    fragColor = vec4(result, 1.0);
}

vec3 phong_illumination_result(vec3 norm, vec3 view_dir) {
    vec3 light_dir = normalize(a_light.position - pos_output);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), a_material.shininess);
    float distance = length(a_light.position - pos_output);
    //float attenuation = 1.0f/(a_light.attenuation.x + a_light.attenuation.y * distance +
    //a_light.attenuation.z * (distance * distance));
    float attenuation = 1.0f/(a_light.attenuation.x + a_light.attenuation.y * distance);
    //float attenuation = 1.0f/(a_light.attenuation.y * distance);
    // ambient 
    //vec3 ambient = a_light.ambient * a_material.ambient;
    vec3 ambient = a_material.ambient;
    // diffuse // 
    vec3 diffuse = a_light.diffuse * (diff * a_material.diffuse);
    //specular //
    vec3 specular = a_light.specular * (spec * a_material.specular);    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    vec3 result = ambient + diffuse + specular;
    return result;    
}

