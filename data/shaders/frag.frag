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
    vec3 test_color;
    vec3 norm = normalize(Normal);
    vec3 asi = vec3(0.0f); //ambient + att(spot * (I.diffuse+I.specular))
    
    for(int i = 0; i < activeCount; i++)
    {
        //distance between the light source and the point
        float dis = length(vec3(light[i].position) - FragPos);
        vec3 L = normalize(vec3(light[i].position) - FragPos);
        vec3 lightDir = vec3(0.0f);

        vec3 tmp = vec3(1.0f);
        float spot = 1.0f;

        //ambient
        vec3 amb = vec3(light[i].ambient * material.ambient) * matDiff.rgb;
        //attenuation
        //- light source attenuation
        float lAtt = 1.0f;
        //- atmospheric attenuation (these time not using)
        
        float c1 = light[i].constantAttenuation;
        float c2 = light[i].linearAttenuation;
        float c3 = light[i].quadraticAttenuation;
        switch (light[i].type)
        {
            case 0: // when it is point
                lAtt = min(1.0f/((c1) + (c2 * dis) + (c3 * (dis * dis))), 1.0f);
                lightDir = L;
                break;
            case 1: // when it is spot
                lAtt = min(1.0f/((c1) + (c2 * dis) + (c3 * (dis * dis))), 1.0f);
                lightDir = -light[i].direction;
                vec3 RL = (FragPos - vec3(light[i].position));
                float Alpha = acos(dot(L,lightDir)/dot(length(L),length(lightDir)));
                float Theta = radians(light[i].spotInner);
                float Phi = radians(light[i].spotOuter);
                float P = light[i].spotExponent;

                spot = pow(((cos(Alpha)-cos(Phi))/(cos(Theta)-cos(Phi))), P);
                spot = clamp(spot, 0, 1);
                break;
            case 2 : //when it is dir
                //tmp *= lAtt;  not use at dir
                lightDir = -light[i].direction;
                break;
        }
        //diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * matDiff.xyz;

        //specular
        vec3 R = (2.0f * (dot(norm, lightDir)) * norm) - lightDir;
        vec3 V = normalize(cameraPos - FragPos);
        vec3 specular = vec3(light[i].specular * material.specular) * (pow(max(dot(R, V),0), material.shininess));
        tmp = spot * (diffuse + specular);

        //assemble result
        asi += amb + (tmp * lAtt);
    }
    result = material.emission + vec4(asi, 1.0f);

    FragColor = result;
    //FragColor = vec4(test_color/2 , 1.0);
}