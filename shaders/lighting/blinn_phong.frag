#version 460 core

in VertexData {
    vec4 model_pos;
    vec4 world_pos;
    vec4 proj_pos;
    vec4 light_pos;
    vec2 tex_pos;
} fs_in;

uniform vec3 eye_world_pos;
uniform vec3 light_world_pos = vec3(0.0, 4.0, 0.0);
uniform vec3 diffuse_color = vec3(0.8, 0.8, 0.8);
uniform vec3 light_color = vec3(200.0);
uniform bool with_shadows = false;
uniform sampler2D shadow_tex;

uniform float ambient_weight = 0.0005;
uniform float specular_power = 128.0;

out vec4 frag_color;

float get_shadow() {
    vec3 pos = fs_in.light_pos.xyz / fs_in.light_pos.w;
    pos = pos * 0.5 + 0.5;
    float current_depth = pos.z;
    float closest_depth = texture(shadow_tex, pos.xy).r; 
    float bias = 0.005;
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;

    return shadow;
}


void main() {
    vec3 world_pos = fs_in.world_pos.xyz;
    float light_dist = length(world_pos - light_world_pos);
    light_dist *= light_dist;

    // Ambient:
    vec3 ambient = ambient_weight * light_color;

    // Diffuse:
    vec3 light_dir = normalize(world_pos - light_world_pos);
    vec3 normal = normalize(cross(dFdx(world_pos), dFdy(world_pos)));
    float diffuse_weight = max(dot(-light_dir, normal), 0.0);
    vec3 diffuse = diffuse_weight * light_color / light_dist;

    // Specular:
    vec3 view_dir = normalize(world_pos - eye_world_pos);
    vec3 halfway_dir = normalize(-light_dir - view_dir);
    float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), specular_power);
    vec3 specular = specular_weight * light_color / light_dist;

    // Shadows:
    float shadow = with_shadows ? get_shadow() : 0.0;

    // Combined:
    vec3 color = (ambient +  (1.0 - shadow) * (diffuse + specular)) * diffuse_color;
    frag_color = vec4(color, 1.0);
}
