#version 410 core


layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;

layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gColor;


void main()
{ 
    gPosition = vec4(Position, 1.0);
    gNormal = vec4(Normal, 1.0);
    gColor = vec4(1.0);
}