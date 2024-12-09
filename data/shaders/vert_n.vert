#version 400
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;
layout(location = 3) in vec3 vTangent;

out vec3 normal;
out vec4 color;
out vec3 tangent;

void main()
{
    gl_Position = vPosition;

    color = vec4(1.0, 0.0, 0.0, 1.0);
    normal = vNormals;
    tangent = vTangent;
}