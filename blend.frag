#version 410 core
in vec2 TexCoords;

out vec4 FragColor;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gShadow0;
uniform sampler2D gShadow1;
uniform sampler2D gShadow2;

uniform vec3 lightPosition[3];
uniform vec3 lightColor[3];
uniform float lightSize[3];
uniform int lightOn[3];
uniform int shadowSwitch;

void main() {
    float ambientCoef = 0.05;
    float diffuseCoef = 0.85 / 3;
    float specularCoef = 1.0 / 3;
    float specularExp = 20;

    vec4 Position = texture(gPosition, TexCoords);
    vec4 Normal = texture(gNormal, TexCoords);
    vec4 Color = texture(gColor, TexCoords);
    vec4 Shadow[3];
    Shadow[0] = texture(gShadow0, TexCoords);
    Shadow[1] = texture(gShadow1, TexCoords);
    Shadow[2] = texture(gShadow2, TexCoords);

    vec4 fragColor = ambientCoef * vec4(1);

    for(int l = 0; l < 3; l++) {
        if(lightOn[l] == 1) {

            vec4 lightPos = vec4(lightPosition[l],1);
            vec3 lightcolor = lightColor[l];
            vec4 lightDirection = lightPos - Position;
            float inOutAngle = max(0, dot(lightDirection / length(lightDirection), Normal));
            vec4 diffuse = vec4(lightcolor * inOutAngle, 1.0);
            vec4 specular = vec4(vec3(pow(inOutAngle, specularExp)), 1.0);

            float shadowCoef = 1;
            if(shadowSwitch == 1)
                shadowCoef = Shadow[l].x;

            fragColor += shadowCoef * (diffuseCoef * diffuse + specularCoef * specular);
            // if(shadowCoef > 1)
            //     FragColor = vec4(1,0,0,1);
        }
    }


    FragColor = fragColor;
    // FragColor = Shadow[0];


    // FragColor = vec4(vec3(inOutAngle),1);
    // FragColor = textureLod(tex, TexCoords, 3);
}
