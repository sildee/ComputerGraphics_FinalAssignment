#version 430 core

// Uniform inputs
uniform vec3 mat_ambient;
uniform vec3 mat_diffuse;
uniform float mat_power;
uniform vec3 mat_specular;
uniform sampler2D texsampler;
uniform int model_uses_texture;
uniform int model_is_skybox;
uniform vec3 light_colors[20];
uniform float light_intensities[20];

// Inputs from vertexshader
in VS_OUT
{
    vec3 N;
    vec3 L[20];
    vec3 V;
} fs_in;
in vec2 UV;

// Output color
out vec4 fragColor;

int max_lights = 20;

void main()
{
    // Normalize the incoming N and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 V = normalize(fs_in.V);

    vec3 ambient = mat_ambient;
    vec3 diffuse;
    vec3 specular;

    if(model_is_skybox == 1){
        diffuse += texture2D(texsampler, UV).rgb;
    }
    else {
        for (int i = 0; i < max_lights; ++i) {

            vec3 L = normalize(fs_in.L[i]);
            vec3 R = reflect(-L, N);
            
            float distance = length(fs_in.L[i]);

            float diff_strength = max(dot(N, L), 0.0);
            float spec_strength = pow(max(dot(R, V), 0.0), mat_power);

            vec3 combinedLight = light_colors[i] * light_intensities[i];
            
            if (model_uses_texture == 1) {
                vec3 texColor = texture2D(texsampler, UV).rgb;
                diffuse += diff_strength * texColor * combinedLight;
            } else {
                diffuse += diff_strength * mat_diffuse * combinedLight;
            }

            // Compute the specular component
            specular += spec_strength * mat_specular * combinedLight;
        }
    }

    // Write final color to the framebuffer
    fragColor = vec4(mat_ambient + diffuse + specular, 1.0);
}
