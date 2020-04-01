#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

uniform vec3 lightPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPos;
out vec2 TexCoord;

//out vec4 vertexColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    //Normal = aNormal;
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    //vertexColor = vec4(aColor, 1.0f);
    //vertexColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    LightPos = vec3(view * vec4(lightPos, 1.0));
    TexCoord = aTexCoord;
} 