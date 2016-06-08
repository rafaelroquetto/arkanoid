#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

void main()
{
    vec3 LightPosition = vec3(50.0, 10.0, 40.0);

    // ambient light
    float ambientStrength = 0.2f;
    vec3 objectColor = vec3(1.0f, 0.5f, 0.2f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;

    //diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;


    vec3 result = (ambient + diffuse) * objectColor;
    color = vec4(result, 1.0f);
}

