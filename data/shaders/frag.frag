#version 400

out vec4 FragColor;

in vec4 color;
in vec2 UV;

void main()
{   
    FragColor = vec4(UV , 0.0 , 1.0);
}