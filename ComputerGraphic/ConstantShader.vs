#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

//out vec4 vertexColor;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos = vec3(model * vec4(aPos, 1.0));
    //Normal = aNormal;
    Normal = mat3(transpose(inverse(model))) * aNormal;
    //vertexColor = vec4(aColor, 1.0f);
    //vertexColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
} 