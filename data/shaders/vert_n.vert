#version 400
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;
layout(location = 3) in vec3 vTangent;

out vec3 normal;
out vec4 red;
out vec4 green;
out vec4 blue;
out vec3 tangent;

void main()
{
    gl_Position = vPosition;

    red = vec4(1.0, 0.0, 0.0, 1.0);
    green = vec4(0.0, 1.0, 0.0, 1.0);
    blue = vec4(0.0, 0.0, 1.0, 1.0);
    normal = vNormals;
    tangent = vTangent;
}