#include <string>
#include <vector>
#include "scene.h"
#ifndef SHADER_H
#define SHADER_H

// consistent with screen.frag
#define KERNEL_SAMPLE 64
#define RANDOM_FLOATS 320
#define SCREEN_SIZE 768


class BaseShader {
public:
    unsigned int shaderProgram;

    int success;
    char infoLog[512];

    BaseShader(std::string vert, std::string frag);
    
};


class GeomShader: public BaseShader {
public:
    std::vector<float> vertices;
    unsigned int VAO, VBO, FBO, RBO;
    unsigned int gPosition, gNormal, gColor;

    GeomShader(std::string vert, std::string frag);

    void init(std::vector<float> vertices);
    void render(Scene scene);
};



class DepthShader: public BaseShader {
public:
    std::vector<float> vertices;
    // const static size_t stride = 6;
    unsigned int VAO, VBO, FBO, RBO;
    unsigned int gDepth, gDepth2;
    // float *gDepthInitData, *gDepth2InitData;
    DepthShader();

    void init(std::vector<float> vertices);
    void render(Light light);
};



class DepthBackgroundShader: public BaseShader {
public:
    std::vector<float> vertices;
    // const static size_t stride = 6;
    unsigned int VAO, VBO, FBO, RBO;
    unsigned int gDepth, gDepth2;
    DepthBackgroundShader(std::string vert, std::string frag);

    void init();
    void render(unsigned int gDepth, unsigned int gDepth2);
};



class ShadowShader: public BaseShader {
public:
    unsigned int FBO, RBO;
    unsigned int gShadow;
    ShadowShader(std::string vert, std::string frag);
    void init();    
    void render(unsigned int gPosition, unsigned int gShadowmap, unsigned int gShadowmap2);
};



class DisplayShader: public BaseShader {
public:
    DisplayShader(std::string vert, std::string frag);
    void init();    
    void render(unsigned int texture);
};



class BlendShader: public BaseShader {
public:
    BlendShader(std::string vert, std::string frag);
    void init();    
    void render(Scene scene, unsigned int gPosition, unsigned int gNormal, unsigned int gColor, unsigned int gShadow);
};



class Shader {
public:
    virtual void initShader();
    virtual void updateShader(Scene scene);

    GeomShader *geomShader;
    DepthShader depthShader[3];
    ShadowShader *shadowShader;
    DepthBackgroundShader *depthBackgroundShader;
    BlendShader *blendShader;
    DisplayShader *displayShader;
};




#endif

