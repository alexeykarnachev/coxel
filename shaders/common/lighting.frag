#version 460 core

in vec4 frag_pos;

uniform vec3 u_view_pos;
uniform vec3 u_light_pos;
uniform vec3 u_diffuse_color;
uniform vec3 u_light_color;

uniform bool u_with_shadows;
uniform samplerCube u_depth_map;
uniform float u_far;

uniform float u_ambient_weight;
uniform float u_specular_power;

layout(location = 0) out vec4 f_color;

float calc_shadow() {
    vec3 frag_to_light = frag_pos.xyz - u_light_pos;
    float closest_depth = texture(u_depth_map, frag_to_light).r;
    closest_depth *= far_plane;
    float current_depth = length(frag_to_light);
    float bias = 0.05;
    float shadow = current_depth - bias > closest_depth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}


void main() {
    vec3 pos = frag_pos.xyz;
    float light_dist = length(pos - u_light_pos);
    light_dist *= light_dist;

    // Ambient:
    vec3 ambient = u_ambient_weight * u_light_color;

    // Diffuse:
    vec3 light_dir = normalize(pos - u_light_pos);
    vec3 normal = normalize(cross(dFdx(pos), dFdy(pos)));
    float diffuse_weight = max(dot(-light_dir, normal), 0.0);
    vec3 diffuse = diffuse_weight * u_light_color / light_dist;

    // Specular:
    vec3 view_dir = normalize(pos - u_view_pos);
    vec3 halfway_dir = normalize(-light_dir - view_dir);
    float specular_weight = pow(max(dot(normal, halfway_dir), 0.0), u_specular_power);
    vec3 specular = specular_weight * u_light_color / light_dist;

    // Shadow:
    float shadow = u_with_shadows ? calc_shadow() : 0.0;

    // Combined:
    vec3 color = (ambient + (1.0 - shadow) * (diffuse + specular)) * u_diffuse_color;
    f_color = vec4(color, 1.0);
}
