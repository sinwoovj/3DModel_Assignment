#version 400
out vec4 FragColor;

in vec4 vColor;
in vec2 UV;

void main()
{   
    FragColor = vColor;
}