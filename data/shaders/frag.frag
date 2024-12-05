#version 400

out vec4 FragColor;

in vec4 color;
in vec2 UV;
in vec3 Normal;  
in vec3 FragPos;

uniform sampler2D tex;
uniform int useTexture;

struct LightSourceParameters
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 position;
    vec3 spotDirection;
    float spotExponent;
    float spotCutoff; // (range: [0.0,90.0], 180.0)
    float spotCosCutoff; // (range: [1.0,0.0], -1.0)
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

void main()
{
    vec3 result = vec3(1.0f);
    vec4 objectColor = vec4(UV, 0.0, 1.0);
    if (useTexture == 1) // when using texture
    {
        objectColor = texture(tex, UV);
        material.diffuse = texture(tex, UV); //textureColor == diffuse of material
    }

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    
    for(int i = 0; i < activeCount; i++)
    {
        if(light[i].type == 1) // when it is spot
        {
            // spot code here!!
        }
        //calculate lights [emission, att, ambient, Sportlight, diffuse, specular]
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
    }
    result = light.ambient;

    FragColor = objectColor * vec4(result, 1.0);
}