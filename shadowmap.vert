#version 410 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

layout (location = 0) out float Depth;


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


void main(){   

    vec4 homoPos = vec4(in_Position, 1.0);
   
    homoPos = projection() * homoPos;

    homoPos /= homoPos.w;

    gl_Position = homoPos;

    Depth = homoPos.z;

}  