R""(
#version 330 core

out vec4 color;
in vec4 fragColor;

void main()
{
    color = fragColor;
}
)""