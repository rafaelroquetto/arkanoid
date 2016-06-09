#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec4 gl_FragCoord;

out vec4 color;

uniform int objectType;

void main()
{
    vec3 LightPosition = vec3(50.0, 10.0, 40.0);

    // ambient light
    float ambientStrength = 0.2f;
    vec3 objectColor = (objectType == 0) ? vec3(1.0f, 0.5f, 0.2f) : vec3(gl_FragCoord.x/1024, gl_FragCoord.y/768, gl_FragCoord.z);//vec3(0.6, 0.9, 0.2);
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

