#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTexCoords;
layout(location = 3) in vec3 vTangent;

uniform mat4 model;
uniform mat4 m2w;

out vec2 UV; //v_texCoords
out vec3 FragPos; //wP
out vec3 Normal; //wN
out mat3 tbnMat;

void main()
{
   FragPos = (m2w * vPosition).xyz; 

   UV = vTexCoords; 
   Normal = vec3(normalize(m2w * vec4(vNormals, 0.0f)));
   
   vec3 Bin = cross(vTangent, vNormals);
   //vec3 Bin = cross(vNormals, vTangent);
   tbnMat = transpose(mat3(vTangent, Bin, vNormals)); //row major

   gl_Position = model * vPosition;
}