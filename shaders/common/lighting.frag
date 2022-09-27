#version 460 core

in vec4 es_pos;

uniform vec3 u_view_pos;
uniform vec3 u_light_pos;
uniform vec3 u_diffuse_color;
uniform vec3 u_specular_color;
uniform vec3 u_light_color;

uniform float u_ambient_weight;
uniform float u_specular_power;

layout(location = 0) out vec4 f_color;


void main() {
    vec3 pos = es_pos.xyz;
    float light_dist = length(pos - u_light_pos);
    light_dist *= light_dist;

    // Ambient:
    vec3 ambient = u_ambient_weight * u_diffuse_color;

    // Diffuse:
    vec3 light_dir = normalize(pos - u_light_pos);
    vec3 normal = normalize(cross(dFdx(pos), dFdy(pos)));
    float diffuse_weight = max(dot(-light_dir, normal), 0.0);
    vec3 diffuse = diffuse_weight * u_diffuse_color * u_light_color / light_dist;

    // Specular:
    vec3 view_dir = normalize(pos - u_view_pos);
    vec3 halfway_dir = normalize(-light_dir - view_dir);
    float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), u_specular_power);
    vec3 specular = specular_weight * u_specular_color * u_light_color / light_dist;

    // Combined:
    vec3 color = ambient + diffuse + specular;
    f_color = vec4(color, 1.0);
}
