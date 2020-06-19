#ifndef SCENE_H
#define SCENE_H

#include <nanogui/nanogui.h>


class Light {
public:
    Eigen::Vector3f position;
    Eigen::Vector3f displacement;
    float size;
    nanogui::Color color;
    bool on;

    // Light(float x, float y, float z, float s, float r, float g, float b, bool);
};

class Scene {
public:
    Light light[3];
    float rotationX, rotationY;
    bool shadowOn;
};


#endif