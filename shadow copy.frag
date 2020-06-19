#version 410 core
in vec2 TexCoords;

out vec4 gShadowCoef;

uniform sampler2D gPosition;
uniform sampler2D gDepth;
uniform sampler2D gDepth2;

uniform vec3 lightPosition;
uniform float lightSize;

mat4 projection() {

    mat4 centerLight = mat4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        -lightPosition.x, -lightPosition.y, -lightPosition.z, 1
    );

    // -90 around -x
    mat4 rotateLight = mat4(
        1,0,0,0,
        0,0,-1,0,
        0,1,0,0,
        0,0,0,1
    );

    // Persp projection: frustum [-1,1]^2 * [1,10]
    float n = 1;
    float f = 5;
    mat4 persp = mat4(
        n,0,0,0,
        0,n,0,0,
        0,0,n+f,1,
        0,0,-n*f,0
    );

    // Ortho projectio: [-1,1]^2*[1,10] -> [-1,1]^2*[0.1,1]
    mat4 ortho = mat4(
        1,0,0,0,
        0,1,0,0,
        0,0,1/f,0,
        0,0,0,1
    );

    mat4 mvp = ortho * persp * rotateLight * centerLight;
    return mvp;
}


void main() {
    float DEPTH_THRESHOLD = 0.01;
    float PERCENTAGE_ZERODIVISION = 0.0006;
    int BLOCKER_SEARCH_SAMPLE = 4; // 9*9
    int PCF_SAMPLE = 4;

    float n = 1;
    float f = 5;
    float screenDepth = 1.0/f; // depth of screen, after othro projection

    vec4 Position = texture(gPosition, TexCoords);
    // To light space
    vec4 lightSpacePosition = projection() * Position;
    lightSpacePosition /= lightSpacePosition.w;
    // Get depth coord
    vec2 shadowmapTexCoord = (lightSpacePosition.xy + vec2(1,1)) / 2; // map[-1,1]->[0,1]


    // Filter size
    float receiverDepth = lightSpacePosition.z;
    float blockerSearchRegion = lightSize / receiverDepth * (receiverDepth - screenDepth);
    int blockerNum = 0;
    float blockerTotalDepth = 0.0;
    for(int i = -BLOCKER_SEARCH_SAMPLE; i <= BLOCKER_SEARCH_SAMPLE; i++) {
        for(int j = -BLOCKER_SEARCH_SAMPLE; j <= BLOCKER_SEARCH_SAMPLE; j++) {
            vec2 samplePosition = lightSpacePosition.xy + vec2(i,j) / BLOCKER_SEARCH_SAMPLE * blockerSearchRegion;
            vec2 sampleTexCoord = (samplePosition + vec2(1,1)) / 2;
            float sampleDepth = texture(gDepth, sampleTexCoord).x;
            if(receiverDepth - sampleDepth > DEPTH_THRESHOLD) {
                blockerNum += 1;
                blockerTotalDepth += sampleDepth;
            }
        }
    }
    float blockerAvgDepth;
    if(blockerNum != 0) 
        blockerAvgDepth = blockerTotalDepth / blockerNum;
    else
        blockerAvgDepth = 1000;
        
    float filtersize, mipmapLevel;
    if(receiverDepth > blockerAvgDepth) {
        filtersize = (receiverDepth - blockerAvgDepth) * lightSize / blockerAvgDepth;
        mipmapLevel = log2(filtersize * 768);
    } else {
        filtersize = 1.0 / 768;
        mipmapLevel = 0; // filtersize cannot be 0, or, if no blocker then don't filter
    }

    // mipmapLevel = 0;
    

    // // Variance Soft Shadow, not working ideally

    // float avgDepth = textureLod(gDepth, shadowmapTexCoord, mipmapLevel).x;
    // float avgDepth2 = textureLod(gDepth2, shadowmapTexCoord, mipmapLevel).x;
    // float varDepth = avgDepth2 - avgDepth * avgDepth;

    // float percentage;
    // if(receiverDepth - avgDepth > DEPTH_THRESHOLD) // shadow
    //     percentage = (varDepth + PERCENTAGE_ZERODIVISION) / (varDepth + pow(lightSpacePosition.z - avgDepth, 2) + PERCENTAGE_ZERODIVISION);
    // else // this one-tailed Chebyshev's Ineq only works asssumes receiverDepth >= avgDepth
    //     percentage = 1;

    // ////////////////////


    // count percentage of closer
    float shadowSample = 0;
    for(int i = -PCF_SAMPLE; i <= PCF_SAMPLE; i++) {
        for(int j = -PCF_SAMPLE; j <= PCF_SAMPLE; j++) {
            vec2 samplePosition = lightSpacePosition.xy + vec2(i,j) / PCF_SAMPLE * blockerSearchRegion;
            vec2 sampleTexCoord = (samplePosition + vec2(1,1)) / 2;
            float sampleDepth = texture(gDepth, sampleTexCoord).x;
            if(lightSpacePosition.z - sampleDepth > DEPTH_THRESHOLD) {
                shadowSample += 1;
            }
        }
    }
    float percentage = 1 - shadowSample / 81;

    gShadowCoef = vec4(vec3(percentage), 1);
}
