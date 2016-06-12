#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 3) in vec3 partCoord;
layout (location = 4) in vec4 partPosition;
layout (location = 5) in vec4 partColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalModel;

uniform int objectType;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out vec4 PartColor;

void main()
{
   if (objectType == 3) { /* particle */
       vec4 ppos = vec4(partCoord, 1.0) + partPosition;
       gl_Position = projection * view * ppos;

       PartColor = partColor;
   } else {

       gl_Position = projection * view * model * vec4(position, 1.0);

       Normal = mat3(normalModel)*normal;

       FragPos = vec3(model * vec4(position, 1.0));
       TexCoord = texCoord;
   }
}

