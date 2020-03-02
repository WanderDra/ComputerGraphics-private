#version 460 core
layout (location = 1) in vec3 aColor;

out vec4 FragColor;

in vec4 vertexColor;

void main()
{
    FragColor = vec4(aColor, 1.0f);
} 