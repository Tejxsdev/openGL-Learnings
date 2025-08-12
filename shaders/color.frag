
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;  

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NUM_LIGHTS 2
uniform Light lights[NUM_LIGHTS];
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < NUM_LIGHTS; ++i) {
        // Ambient
        vec3 ambient = lights[i].ambient * texture(texture_diffuse1, TexCoords).rgb;

        // Diffuse
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lights[i].diffuse * diff * texture(texture_diffuse1, TexCoords).rgb;

        // Specular
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = lights[i].specular * spec * texture(texture_specular1, TexCoords).rgb;

        result += ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0f);
}

