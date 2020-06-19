#version 410 core
in vec2 TexCoords;

// layout (location = 0) out vec4 FragColor;
out vec4 gShadowCoef;

uniform sampler2D gPosition;
uniform sampler2D gDepth;
uniform sampler2D gDepth2;


mat4 projection() {

    vec3 lightPosition = vec3(0,2,0);

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

    vec4 lightPosition = vec4(0,2,0,1);
    float lightSize = 0.2;
    float n = 1;
    float f = 5;
    float screenDepth = 1.0/f; // depth of screen, after othro projection

    // vec4 Position = vec4(texture(gPosition, TexCoords).xyz, 1);
    vec4 Position = texture(gPosition, TexCoords);
    // To light space
    vec4 lightSpacePosition = projection() * Position;
    lightSpacePosition /= lightSpacePosition.w;
    // Get depth coord
    vec2 shadowmapTexCoord = (lightSpacePosition.xy + vec2(1,1)) / 2; // map[-1,1]->[0,1]


    // Filter size
    // float lightDistance = length((lightPosition - Position).xyz);
    float lightDistance = lightSpacePosition.z;
    // float lightDistance = textureLod(gDepth, shadowmapTexCoord, 0).x;
    float blockerSearchRegion = lightSize / lightDistance * (lightDistance - screenDepth);
    float blockerSearchMipmapLevel = log2(blockerSearchRegion * 768);
    float blockerAvgDepth = textureLod(gDepth, shadowmapTexCoord, blockerSearchMipmapLevel).x;
    
    float filtersize, mipmapLevel;
    if(lightDistance > blockerAvgDepth) {
        filtersize = (lightDistance - blockerAvgDepth) * lightSize / blockerAvgDepth;
        mipmapLevel = log2(filtersize * 768);
    } else {
        mipmapLevel = 0; // filtersize cannot be 0, or, if no blocker then don't filter
    }
    // float filtersize = 0.02;
    // mipmapLevel = 2;
    
    // float receiverDepth = texture(gDepth, shadowmapTexCoord).x;
    float avgDepth = textureLod(gDepth, shadowmapTexCoord, mipmapLevel).x;
    float avgDepth2 = textureLod(gDepth2, shadowmapTexCoord, mipmapLevel).x;
    float varDepth = avgDepth2 - avgDepth * avgDepth;

    float percentage;
    if(lightSpacePosition.z - avgDepth > DEPTH_THRESHOLD) // shadow
        percentage = (varDepth + PERCENTAGE_ZERODIVISION) / (varDepth + pow(lightSpacePosition.z - avgDepth, 2) + PERCENTAGE_ZERODIVISION);
    else // this one-tailed Chebyshev's Ineq only works asssumes receiverDepth >= avgDepth
        percentage = 1;
    // I'm a genius!!!

    gShadowCoef = vec4(vec3(percentage), 1);
    // gShadowCoef = vec4(vec3(mipmapLevel/10), 1);
    // gShadowCoef = vec4(vec3(abs(varDepth)*100), 1);
    // gShadowCoef = vec4(vec3(avgDepth2)/10000, 1);

    // gShadowCoef = vec4(texture(gDepth, TexCoords).xyz, 1);
}
