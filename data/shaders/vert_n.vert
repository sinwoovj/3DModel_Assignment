#version 400
layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;
layout(location = 3) in vec3 vTangent;

uniform mat4 m2w;

out vec4 red;
out vec4 green;
out vec4 blue;
out vec3 T;
out vec3 BT; 
out vec3 N;

void main()
{
    gl_Position = vPosition;

    red = vec4(1.0, 0.0, 0.0, 1.0);
    green = vec4(0.0, 1.0, 0.0, 1.0);
    blue = vec4(0.0, 0.0, 1.0, 1.0);
    T = normalize(vTangent);
    BT = normalize(cross(vTangent, vNormals));
    N = normalize(vNormals);
}