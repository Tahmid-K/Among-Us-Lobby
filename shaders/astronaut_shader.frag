#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

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
uniform vec3 color;
uniform Light a_light;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

vec4 toon_shading_result(vec3 norm, vec3 view_dir, vec3 input_color);
vec4 normal_result(vec3 norm);

void main() {
    //vec4 input_color = vec4(color, 1.0);
    vec3 norm = normalize(normal_output);
    vec3 view_dir = normalize(view_pos - pos_output);
    //fragColor = vec4(color, 1.0f);
    //fragColor = normal_result(norm);
    fragColor = toon_shading_result(norm, view_dir, color);
}

vec4 toon_shading_result(vec3 norm, vec3 view_dir, vec3 input_color) {
    vec4 out_color = vec4(input_color, 1.0);
    // diffuse
    vec3 light_dir = normalize(a_light.position - pos_output);
    float diff = max(dot(norm, light_dir), 0.0);
    //vec3 diffuse = a_light.diffuse * (diff * a_material.diffuse); // material
    vec3 diffuse = a_light.diffuse * (diff * input_color);
    // specular
    vec3 reflect_dir = reflect(-light_dir, norm);
    //float spec = pow(max(dot(view_dir, reflect_dir), 0.0), a_material.shininess); // material
    //vec3 specular = a_light.specular * (spec * a_material.specular); // material
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 1.0f); 
    vec3 specular = a_light.specular * (spec * input_color) ; // use normal instead
    
    // edge
    float edge = max(0, dot(norm, view_dir));
    if(edge < .3) {
        return vec4(0, 0, 0, 1.0);
    }
    float intensity = dot(spec, diff);
    if(intensity > .95) {
        out_color = vec4(1.0, 1, 1, 1.0) * out_color;
    }
    else if(intensity > .5) {
        out_color = vec4(.7, .7, .7, 1.0) * out_color;
    }
    //else {
    else if(intensity > .05)  {
        out_color = vec4(.35, .35, .35, 1.0) * out_color;
    }
    else {
        out_color = vec4(.1, .1, .1, 1.0) * out_color;
    }
    return out_color;
}

vec4 normal_result(vec3 norm) {
    norm += 1;
    norm /= 2;
    return vec4(norm, 1.0);
}
