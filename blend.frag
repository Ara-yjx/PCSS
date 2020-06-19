#version 410 core
in vec2 TexCoords;

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gShadow;

uniform int shadowSwitch;

void main() {
    vec4 lightPosition = vec4(0,2,0,1);
    vec3 lightColor = vec3(1,1,1);
    float ambientCoef = 0.05;
    float diffuseCoef = 0.85;
    float specularCoef = 1;
    float specularExp = 20;

    vec4 Position = texture(gPosition, TexCoords);
    vec4 Normal = texture(gNormal, TexCoords);
    vec4 Color = texture(gColor, TexCoords);
    vec4 Shadow = texture(gShadow, TexCoords);

    vec4 lightDirection = lightPosition - Position;
    float inOutAngle = max(0, dot(lightDirection / length(lightDirection), Normal));
    vec4 diffuse = vec4(lightColor * inOutAngle, 1.0);
    vec4 specular = vec4(vec3(pow(inOutAngle, specularExp)), 1.0);

    float shadowCoef = 1;
    if(shadowSwitch == 1)
        shadowCoef = Shadow.x;

    FragColor = ambientCoef * vec4(1) + shadowCoef*(diffuseCoef * diffuse + specularCoef * specular);
    if(shadowCoef > 1)
        FragColor = vec4(1,0,0,1);
    // FragColor = vec4(vec3(inOutAngle),1);
    // FragColor = textureLod(tex, TexCoords, 3);
}
