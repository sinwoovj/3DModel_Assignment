#version 400

layout(location = 0) in vec4 vPosition;
layout(location = 1) in vec3 vNormals;
layout(location = 2) in vec2 vTexCoords;
layout(location = 3) in vec3 vTangent;

uniform mat4 m2w;
uniform mat4 viewMat;
uniform mat4 projMat;
uniform mat4 lightMat;
uniform bool lightView;
uniform bool shadowCalculate;

out vec2 UV; //v_texCoords
out vec3 FragPos; //wP
out vec3 T; //tan
out vec3 BT; //btan view space
out vec3 N; //nor in ts view space
out mat3 tbnMat;

void main()
{
    vec4 temp = m2w * vPosition;
    temp = temp / temp.w;
    FragPos = temp.xyz; 
    
    UV = vTexCoords;
    T = normalize(mat3(m2w) * vTangent);
    N = normalize(mat3(m2w) * vNormals);
    BT = normalize(cross(T, N));
    //tbnMat = transpose(mat3(T, BT, N)); //row major
    tbnMat = mat3(T, BT, N);
    
    if(lightView)
    {
        gl_Position = projMat * lightMat * m2w * vPosition;
    }
    else
    {
        gl_Position = projMat * viewMat * m2w * vPosition;
    }
}