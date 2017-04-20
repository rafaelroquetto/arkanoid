#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec4 gl_FragCoord;
in vec2 TexCoord;
in vec4 PartColor;

out vec4 color;

uniform int objectType;
uniform int viewWidth;
uniform int viewHeight;
uniform vec3 viewPos;
uniform sampler2D padTexture;

const int PAD          = 0;
const int BRICK        = 1;
const int BALL         = 2;
const int PARTICLE     = 3;
const int BOUNDINGBOX  = 4;
const int HARD_BRICK   = 5;
const int GLYPH        = 6;

vec4 getObjectColor()
{
    switch (objectType) {
    case PAD:
    case GLYPH:
        return texture(padTexture, TexCoord);
    case BRICK:
        return vec4(gl_FragCoord.x/viewWidth, gl_FragCoord.y/viewHeight, gl_FragCoord.x/gl_FragCoord.y, 1.0);
    case BALL:
        return vec4(0.5f, 0.5f, 0.4f, 1.0);
    case PARTICLE:
        return texture(padTexture, TexCoord);
    case HARD_BRICK:
        return vec4(0.9f, 0.9f, 0.9f, 0.8f);
    }
}

void main()
{
    vec3 LightPosition = vec3(0.0, 20.0, 10.0);


    vec4 objectColor = getObjectColor();

    // ambient light
    float ambientStrength = 0.2f;
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

    switch (objectType) {
    case PARTICLE:
        color = vec4(gl_FragCoord.x/viewWidth, gl_FragCoord.y/viewHeight, gl_FragCoord.x/gl_FragCoord.y, PartColor.w) * objectColor;
        break;
    case BOUNDINGBOX:
        color = vec4(0.0, 1.0, 0.0, 1.0);
        break;
    default:
        color = vec4((ambient + diffuse + specular), 1.0) * objectColor;
        break;
    }
}

