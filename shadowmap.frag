#version 410 core

layout (location = 0) in float Depth;

layout (location = 0) out vec4 gDepth;


void main()
{ 
    gDepth = vec4(vec3(Depth), 1);
}