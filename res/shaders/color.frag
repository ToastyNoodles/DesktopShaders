#version 330 core
out vec4 FragColor;

in vec3 fPosition;

void main()
{
    FragColor = vec4(fPosition + 0.5, 1.0);
}