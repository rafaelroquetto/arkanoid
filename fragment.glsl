#version 330 core

in vec3 Normal;

out vec4 color;

void main()
{
    float ambientStrength = 0.9f;
    vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;
    vec3 result = ambient * objectColor;
    color = vec4(result, 1.0f);
}

