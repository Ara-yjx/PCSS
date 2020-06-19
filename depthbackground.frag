#version 410 core
in vec2 TexCoords;

layout (location = 0) out vec4 GDepth;
layout (location = 1) out vec4 GDepth2;
// out vec4 gShadowCoef;

uniform sampler2D gDepth;
uniform sampler2D gDepth2;

void main() {

    GDepth  = texture(gDepth, TexCoords);

    if(GDepth.x < 0.1)
        GDepth = vec4(1000,1000,1000,1);
        // GDepth = vec4(1,1,1,2) - GDepth;

    GDepth2 = texture(gDepth2, TexCoords);
    if(GDepth2.x < 0.01)
        GDepth2 = vec4(1000000,1000000,1000000,1);
}