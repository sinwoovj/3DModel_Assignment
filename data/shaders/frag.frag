#version 400

out vec4 FragColor;

in vec2 UV;
in vec3 Normal;  
in vec3 FragPos;

uniform sampler2D tex;
uniform int useTexture;

struct LightSourceParameters
{
    int type;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec3 direction;
    float spotExponent;
    float spotInner;
    float spotOuter;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};
struct MaterialParameters
{
    vec4 emission;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
uniform LightSourceParameters light[10];
uniform MaterialParameters material;
uniform int activeCount;
uniform vec3 cameraPos;

void main()
{
    vec4 result = vec4(1.0f);
    vec4 matDiff = vec4(UV, 0.0, 1.0);
    if (useTexture == 1) // when using texture
    {
        matDiff = texture(tex, UV); //textureColor == diffuse of material
    }

    vec3 norm = normalize(Normal);
    vec3 asi = vec3(0.0f); //ambient + att(spot * (I.diffuse+I.specular))
    vec3 t_amb = vec3(0.0f);
    vec3 t_dfsc = vec3(0.0f);
    
    for(int i = 0; i < activeCount; i++)
    {
        //distance between the light source and the point
        float dis = length(vec3(light[i].position) - FragPos);
        vec3 L = normalize(vec3(light[i].position) - FragPos);
        vec3 lightDir = vec3(0.0f);

        vec3 tmp = vec3(1.0f);
        float spot = 1.0f;

        //ambient
        vec3 amb = vec3(light[i].ambient * material.ambient);
        //attenuation
        //- light source attenuation
        float lAtt = 1.0;
        //- atmospheric attenuation (these time not using)

        switch (light[i].type)
        {
            case 0: // when it is point
                lAtt = min(1/(light[i].constantAttenuation + (light[i].linearAttenuation * dis) + light[i].quadraticAttenuation * (dis * dis)), 1);
                lightDir = L;
                break;
            case 1: // when it is spot
                lAtt = min(1/(light[i].constantAttenuation + (light[i].linearAttenuation * dis) + light[i].quadraticAttenuation * (dis * dis)), 1);
                lightDir = light[i].direction;
                vec3 RL = (FragPos - vec3(light[i].position));
                float Alpha = acos(dot(L,lightDir)/dot(length(L),length(lightDir)));
                float Theta = light[i].spotInner;
                float Phi = light[i].spotOuter;
                float P = light[i].spotExponent;

                spot = pow(((cos(Alpha)-cos(Phi))/(cos(Theta)-cos(Phi))), P);
                break;
            case 2 : //when it is dir
                //tmp *= lAtt;  not use at dir
                lightDir = light[i].direction;
                break;
        }
        //diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(matDiff);

        //specular
        vec3 R = normalize((2.0f * (dot(norm, lightDir)) * norm) - lightDir);
        vec3 V = normalize(cameraPos - FragPos);
        vec3 specular = vec3(light[i].specular * material.specular) * (pow(max(dot(R, V),0), material.shininess));
        tmp = spot * (diffuse + specular);

        //assemble result
        asi += amb + tmp * lAtt;
    }
    result = vec4(asi, 1.0f);

    FragColor = result;
}