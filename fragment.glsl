#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec4 gl_FragCoord;
in vec2 TexCoord;

out vec4 color;

uniform int objectType;
uniform vec3 viewPos;
uniform sampler2D padTexture;

void main()
{
    vec3 LightPosition = vec3(0.0, 20.0, 10.0);

    // ambient light
    float ambientStrength = 0.2f;

    vec4 objectColor;

    switch (objectType) {
    case 0: // pad
        //vec4 blendColor = vec4(gl_FragCoord.x/800, gl_FragCoord.y/768, gl_FragCoord.x/gl_FragCoord.y, 1.0);
        //objectColor = texture(padTexture, TexCoord) * blendColor;
        objectColor = texture(padTexture, TexCoord);
        break;
    case 1: // brick
        objectColor = vec4(gl_FragCoord.x/1024, gl_FragCoord.y/768, gl_FragCoord.x/gl_FragCoord.y, 1.0);
        break;
    case 2: // ball
        objectColor = vec4(0.5f, 0.5f, 0.4f, 1.0);
        break;
    }

    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    vec3 ambient = ambientStrength * lightColor;

    //diffuse light
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //specular light
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;


    vec4 result = vec4((ambient + diffuse + specular), 1.0) * objectColor;
    //color = vec4(result, 1.0f);
    color = result;
}

