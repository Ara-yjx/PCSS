#version 410 core
in vec2 TexCoords;

// layout (location = 0) out vec4 FragColor;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform sampler2D gShadowmap;


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

    // vec4 Position = vec4(texture(gPosition, TexCoords).xyz, 1);
    vec4 Position = texture(gPosition, TexCoords);
    // To light space
    vec4 lightSpacePosition = projection() * Position;
    lightSpacePosition = lightSpacePosition / lightSpacePosition.w;
    // Get depth
    vec2 shadowmapTexCoord = (lightSpacePosition.xy + vec2(1,1)) / 2; // map[-1,1]->[0,1]
    float shadowmapDepth = texture(gShadowmap, shadowmapTexCoord).x;

    if(shadowmapDepth < lightSpacePosition.z - DEPTH_THRESHOLD) {
        FragColor = vec4(0,0,0,1);
    }
    else {
        FragColor = vec4(1);
    }
    // FragColor = vec4(vec3(shadowmapDepth), 1);
    // FragColor = (vec4(shadowmapTexCoord,0, 1));
    // FragColor = texture(gShadowmap, TexCoords) / 5;
    // FragColor = (vec4(vec3(shadowmapTexCoord.x), 1));
    // FragColor = vec4(1.0);
}
