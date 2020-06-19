#version 410 core

layout (location = 0) in float Depth;

layout (location = 0) out vec4 gDepth;
layout (location = 1) out vec4 gDepth2;


void main()
{ 
    gDepth = vec4(vec3(Depth), 1);
    gDepth2 = vec4(vec3(Depth*Depth), 1);
}