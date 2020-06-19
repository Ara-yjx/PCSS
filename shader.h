#include <string>
#include <vector>
#ifndef SHADER_H
#define SHADER_H

// consistent with screen.frag
#define KERNEL_SAMPLE 64
#define RANDOM_FLOATS 320
#define SCREEN_SIZE 768

class ShaderArg {
public:
    float sliderValue;
    float arrowXstate;
    float arrowYstate;
    float bracketState;
    bool switchState1;
    bool switchState2;
    std::string argObj;

    ShaderArg() {};
    ShaderArg(std::string argObj)
    : argObj(argObj) {};
    ShaderArg(float sliderValue, float arrowXstate, float arrowYstate, float bracketState, bool switchState1, bool switchState2) 
    : sliderValue(sliderValue), arrowXstate(arrowXstate), arrowYstate(arrowYstate), bracketState(bracketState), switchState1(switchState1), switchState2(switchState2) {};
};



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
    // std::vector<unsigned int> indices;
    // const static size_t stride = 6;
    unsigned int VAO, VBO, FBO, RBO;
    unsigned int gPosition, gNormal;

    GeomShader(std::string vert, std::string frag);

    void init(std::vector<float> vertices, std::vector<unsigned int> indices);
    void render(float sceneRotation, float sceneScale);
};



class ShadowmapShader: public BaseShader {
public:
    std::vector<float> vertices;
    // const static size_t stride = 6;
    unsigned int VAO, VBO, FBO, RBO;
    unsigned int gDepth, gDepth2;
    // float *gDepthInitData, *gDepth2InitData;
    ShadowmapShader(std::string vert, std::string frag);

    void init(std::vector<float> vertices);
    void render();
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



class Shader {
public:
    virtual void initShader(ShaderArg* arg);
    virtual void updateShader(ShaderArg* arg);

    GeomShader *geomShader;
    DisplayShader *displayShader;
    ShadowmapShader *shadowmapShader;
    ShadowShader *shadowShader;
    DepthBackgroundShader *depthBackgroundShader;
};




#endif

