#version 410 core

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Normal;

uniform float sceneRotationX;
uniform float sceneRotationY;


mat4 projection() {
    float camZ = -2;


    mat4 rotationY = mat4(
        cos(sceneRotationY), 0, sin(sceneRotationY),0,
        0,1,0,0,
        -sin(sceneRotationY), 0, cos(sceneRotationY),0,
        0,0,0,1
    );
    mat4 rotationX = mat4(
        1,0,0,0,
        0,cos(sceneRotationX), -sin(sceneRotationX),0,
        0,sin(sceneRotationX), cos(sceneRotationX),0,
        0,0,0,1
    );

    // The scene is supposed to be in box [-1,1]^3
    // Pretend camera in (0,0,-3), move z +=3 to [2,4]
    mat4 view = mat4(
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,-camZ,1
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

    mat4 mvp = ortho * persp * view * rotationX * rotationY;
    return mvp;
}

void main()
{   
    vec4 homoPos = vec4(in_Position, 1.0);

    gl_Position = projection() * homoPos;


    Position = in_Position; 
    Normal = in_Normal;
    // Normal = vec3((gl_Position.z+1)/2);
    
}  