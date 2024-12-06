#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTextCoords;

uniform mat4 model;
uniform mat4 m2w;

out vec2 UV;
out vec3 FragPos;
out vec3 Normal;

void main()
{
   gl_Position = model * vPosition;
   UV = vTextCoords;
   FragPos = vec3(m2w * vPosition);
   Normal = vec3(normalize(m2w * vec4(vNormals, 0.0f)));
}