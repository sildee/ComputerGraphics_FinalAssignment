#version 430 core

// Uniform inputs
uniform mat4 mv;
uniform mat4 projection;
uniform vec3 light_positions[20];
uniform vec3 light_colors[20];
uniform int num_lights;       

// Per-vertex inputs
in vec3 position;
in vec3 normal;
in vec2 uv;

// Outputs
out VS_OUT
{
   vec3 N;
   vec3 L[20];
   vec3 V;
} vs_out;
out vec2 UV;

void main()
{
    // Calculate view-space coordinate
    vec4 P = mv * vec4(position, 1.0);

    // Calculate normal in view-space
    vs_out.N = mat3(mv) * normal;

    // Calculate light vectors
    for (int i = 0; i < num_lights; ++i) {
        vs_out.L[i] = light_positions[i] - P.xyz;
    }

    // Calculate view vector;
    vs_out.V = -P.xyz;

    // Calculate the clip-space position of each vertex
    gl_Position = projection * P;

    UV = uv;
}
