#version 400

out vec4 FragColor;

in vec4 color;
in vec2 UV;

uniform sampler2D tex;
uniform int useTexture;

void main()
{   
    if (useTexture == 1)
        FragColor = texture(tex, UV);
    else
        FragColor = vec4(UV, 0.0, 1.0);
}