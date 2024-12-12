#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTexCoords;
layout(location = 3) in vec3 vTangent;

uniform mat4 m2w;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 lightMat;
uniform bool shadowCalculate;
uniform bool lightView;

out vec2 UV; //v_texCoords
out vec3 FragPos; //wP
out vec3 Normal; //wN
out vec3 T; //tan
out vec3 BT; //btan view space
out vec3 N; //nor in ts view space
out mat3 tbnMat;

void main()
{
    FragPos = (m2w * vPosition).xyz; 
    
    UV = vTexCoords; 
    Normal = vec3(normalize(m2w * vec4(vNormals, 0.0f)));
    T = normalize(vTangent);
    vec3 Bin = normalize(cross(T, vNormals));
    tbnMat = transpose(mat3(T, Bin, vNormals)); //row major
    BT = normalize(cross(vTangent, vNormals));
    N = normalize(vNormals);
    
    if(lightView)
    {
        gl_Position = projMat * lightMat * m2w * vPosition;
    }
    else
    {
        gl_Position = projMat * viewMat * m2w * vPosition;
    }
}