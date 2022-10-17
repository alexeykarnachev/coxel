#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
} fs_in;

uniform vec3 eye_world_pos;

// Material:
uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform vec3 specular_color;
uniform float shininess;

// Point light:
uniform vec3 point_light_world_pos;
uniform vec3 point_light_color;
uniform float point_light_energy;

// Shadow:
uniform float shadow_max_dist;
uniform samplerCube shadow_cube_tex;

out vec4 frag_color;

void main() {
    vec3 world_pos = fs_in.world_pos.xyz;
    vec3 normal = normalize(cross(dFdx(world_pos), dFdy(world_pos)));
    float point_light_dist = pow(length(world_pos - point_light_world_pos), 2.0);

    // Ambient:
    vec3 ambient = 0.005 * ambient_color;

    // Diffuse:
    vec3 point_light_dir = normalize(world_pos - point_light_world_pos);
    float diffuse_weight = max(dot(-point_light_dir, normal), 0.0);
    vec3 diffuse = diffuse_weight * point_light_energy * point_light_color / point_light_dist;

    // Specular:
    vec3 view_dir = normalize(world_pos - eye_world_pos);
    vec3 halfway_dir = normalize(-point_light_dir - view_dir);
    float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), shininess);
    vec3 specular = specular_weight * specular_color * point_light_energy / point_light_dist;

    // Shadows:
    vec3 light_to_frag = fs_in.world_pos.xyz - point_light_world_pos;
    float curr_depth = length(light_to_frag);
    float closest_depth = texture(shadow_cube_tex, light_to_frag).r * shadow_max_dist;
    float bias = 0.05;
    float shadow = curr_depth - bias > closest_depth ? 1.0 : 0.0;

    // Combined:
    vec3 color = (ambient +  (1.0 - shadow) * (diffuse + specular)) * diffuse_color;
    frag_color = vec4(color, 1.0);
}
