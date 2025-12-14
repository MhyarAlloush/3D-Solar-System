#version 460 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 texCoord;

uniform vec3 objectColor;
uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 moonPos;
uniform vec3 moonColor;

uniform bool blinn;
uniform bool isSun;


void main()
{
    vec3 color = texture(texture_diffuse1, texCoord).rgb * vec3(objectColor);

    if (isSun)
    {
        FragColor = vec4(color, 1.0);
        return;
    }

    vec3 ambient = 0.1 * color;

    vec3 normal = normalize(fragNormal);

    vec3 lightDir = normalize(lightPos - fragPos);
    float diffSun = max(dot(normal, lightDir), 0.0);
    vec3 diffuseSun = diffSun * color;

    vec3 viewDir = normalize(viewPos - fragPos);
    float specSun = 0.0;
    if (blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        specSun = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        specSun = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specularSun = vec3(0.5) * specSun;

    vec3 moonDir = normalize(moonPos - fragPos);           
    float diffMoon = max(dot(normal, moonDir), 0.0);
    vec3 diffuseMoon = diffMoon * moonColor;              

    vec3 reflectMoon = reflect(-moonDir, normal);
    float specMoon = pow(max(dot(viewDir, reflectMoon), 0.0), 8.0) * 0.1; 
    vec3 specularMoon = vec3(1.0) * specMoon;           

    vec3 diffuse = diffuseSun + diffuseMoon;
    vec3 specular = specularSun + specularMoon;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
