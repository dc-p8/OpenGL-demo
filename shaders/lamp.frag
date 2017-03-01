#version 330 core
out vec4 color;

in vec3 vsoColor;

void main()
{
    color = vec4(vsoColor, 1);
}