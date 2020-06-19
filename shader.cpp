#include <nanogui/nanogui.h>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <math.h>
#include "shader.h"
#include "util.h"

#ifndef NATIVESHADER_H
#define NATIVESHADER_H

// using namespace nanogui;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::min;
using std::max;
using std::vector;

using Eigen::Vector3f;
using Eigen::Vector3d;

const string GEOM_VERT = "../geom.vert";
const string GEOM_FRAG = "../geom.frag";
const string DEPTH_VERT = "../depth.vert";
const string DEPTH_FRAG = "../depth.frag";
const string DEPTHBACKGROUND_VERT = "../depthbackground.vert";
const string DEPTHBACKGROUND_FRAG = "../depthbackground.frag";
const string SHADOW_VERT = "../shadow.vert";
const string SHADOW_FRAG = "../shadow.frag";
const string BLEND_VERT = "../blend.vert";
const string BLEND_FRAG = "../blend.frag";
const string DISPLAY_VERT = "../display.vert";
const string DISPLAY_FRAG = "../display.frag";
const string DEFAULT_MODEL = "../models/xbox.obj";
const float PI = 3.14159265358979f;


// got lots of error when try to encapsulate into class BaseShader
const float QUAD_VERTICES[] = {
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};
unsigned int QUAD_VBO, QUAD_VAO;

void initQuad() {
    glGenVertexArrays(1, &QUAD_VAO);
    glGenBuffers(1, &QUAD_VBO);
    glBindVertexArray(QUAD_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, QUAD_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTICES), &QUAD_VERTICES, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}


void loadModel(string modelFile, vector<float>& vertices, vector<unsigned int>& indices) {
    tinyobj::attrib_t attrib;
    vector<tinyobj::shape_t> shapes;
    vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelFile.c_str());

    // if (!warn.empty()) 
    //     std::cout << warn << std::endl;
    if (!err.empty()) 
        std::cerr << err << std::endl;
    if (!ret) {
        exit(1);
    }

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f]; // 3

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                vertices.push_back(vx);
                vertices.push_back(vy);
                vertices.push_back(vz);
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);
                indices.push_back(index_offset + v);
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
}


BaseShader::BaseShader(string vert, string frag) {
    // Vertex Shader
    std::string vsSrc = fileToString(vert);
    const char* vertexShaderSource = vsSrc.c_str(); // convert to GLchar
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER); // create shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // set shader code
    glCompileShader(vertexShader); // compile shader code
    // Check compilation success
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment Shader
    std::string fragmentShaderSourceString = fileToString(frag);
    const char* fragmentShaderSource = fragmentShaderSourceString.c_str();
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check compilation success
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    // Compile shaders
    this->shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader); // attach shaders to a program
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // link shaders to a single program
    // check
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR WITH SHADER PROGRAM \n" << infoLog << std::endl;
    }

    // Clean
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


GeomShader::GeomShader(string vert, string frag): BaseShader(vert, frag) {}


void GeomShader::init(vector<float> vertices, vector<unsigned int> indices) {
    cout<<vertices.size()<<' '<<indices.size()<<endl;
    this->vertices = vertices;
    // this->indices = indices;
    // VBO
    glGenBuffers(1, &(this->VBO)); // create a buffer object with ID
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO); // let the buffer be a VBO buffer 
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), (void*)vertices.data(), GL_STATIC_DRAW); // copy vertices to this buffer
    // glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    
    // VAO
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3)); // normal
    glEnableVertexAttribArray(1);

    // FBO
    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // RBO (depth & stencil buffer)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 768, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR Framebuffer error." << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void GeomShader::render(float sceneRotationY, float sceneRotationX) {

    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);   

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glUseProgram(this->shaderProgram);

    // Set Uniform Params
    int sceneRotationYLocation = glGetUniformLocation(this->shaderProgram, "sceneRotationY");
    glUniform1f(sceneRotationYLocation, sceneRotationY);
    int sceneRotationXLocation = glGetUniformLocation(this->shaderProgram, "sceneRotationX");
    glUniform1f(sceneRotationXLocation, sceneRotationX);

    // Draw
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // draw triangles without EBO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DisplayShader::DisplayShader(string vert, string frag): BaseShader(vert, frag) {}


void DisplayShader::init() {}


void DisplayShader::render(unsigned int texture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glUseProgram(shaderProgram);
    // glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTextureParameteri(texture, GL_TEXTURE_BASE_LEVEL, 0);
    // glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, 10);
    // glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindVertexArray(QUAD_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


DepthShader::DepthShader(string vert, string frag): BaseShader(vert, frag) {};


void DepthShader::init(vector<float> vertices) {
    this->vertices = vertices;

    // // init gDepth to 1000, gDepth2 to 1000000
    // this->gDepthInitData = new float[768*768*4];
    // for(int i = 0; i < 768*768*4; i++)
    //     gDepthInitData[i] = 1000;
    // this->gDepth2InitData = new float[768*768*4];
    // for(int i = 0; i < 768*768*4; i++)
    //     gDepth2InitData[i] = 1000000;

    // VBO
    glGenBuffers(1, &(this->VBO)); // create a buffer object with ID
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO); // let the buffer be a VBO buffer 
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), (void*)vertices.data(), GL_STATIC_DRAW); // copy vertices to this buffer
    
    // VAO
    glGenVertexArrays(1, &this->VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3)); // normal
    glEnableVertexAttribArray(1);

    // FBO
    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gDepth, 0);

    glGenTextures(1, &gDepth2);
    glBindTexture(GL_TEXTURE_2D, gDepth2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gDepth2, 0);

    unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // RBO (depth & stencil buffer)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 768, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR Framebuffer error." << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void DepthShader::render() {
    glEnable(GL_DEPTH_TEST); 
    glDepthFunc(GL_LESS);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);   

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glUseProgram(this->shaderProgram);

    // Draw
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // draw triangles without EBO

    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


DepthBackgroundShader::DepthBackgroundShader(string vert, string frag): BaseShader(vert, frag) {}


void DepthBackgroundShader::init() {
    // FBO
    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gDepth, 0);

    glGenTextures(1, &gDepth2);
    glBindTexture(GL_TEXTURE_2D, gDepth2);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gDepth2, 0);

    unsigned int attachments[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // RBO (depth & stencil buffer)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 768, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR Framebuffer error." << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void DepthBackgroundShader::render(unsigned int gDepth, unsigned int gDepth2) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "gDepth"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "gDepth2"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gDepth2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindVertexArray(QUAD_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


ShadowShader::ShadowShader(string vert, string frag): BaseShader(vert, frag) {}


void ShadowShader::init() {

    // FBO
    glGenFramebuffers(1, &this->FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &gShadow);
    glBindTexture(GL_TEXTURE_2D, gShadow);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 768, 768, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gShadow, 0);

    unsigned int attachments[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    // RBO (depth & stencil buffer)
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 768, 768);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR Framebuffer error." << endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void ShadowShader::render(unsigned int gPosition, unsigned int gDepth, unsigned int gDepth2) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "gDepth"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "gDepth2"), 2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // set min_filter to mipmap to enable mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(gDepth, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(gDepth, GL_TEXTURE_MAX_LEVEL, 10);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gDepth2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // set min_filter to mipmap to enable mipmap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(gDepth, GL_TEXTURE_BASE_LEVEL, 0);
    glTextureParameteri(gDepth, GL_TEXTURE_MAX_LEVEL, 10);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindVertexArray(QUAD_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


BlendShader::BlendShader(string vert, string frag): BaseShader(vert, frag) {}


void BlendShader::init() {}


void BlendShader::render(unsigned int texture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glUseProgram(shaderProgram);
    // glUniform1i(glGetUniformLocation(shaderProgram, "tex"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindVertexArray(QUAD_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



void Shader::initShader(ShaderArg* arg = nullptr) {

    geomShader = new GeomShader(GEOM_VERT, GEOM_FRAG);
    depthShader = new DepthShader(DEPTH_VERT, DEPTH_FRAG);
    shadowShader = new ShadowShader(SHADOW_VERT, SHADOW_FRAG);
    depthBackgroundShader = new DepthBackgroundShader(DEPTHBACKGROUND_VERT, DEPTHBACKGROUND_FRAG);
    blendShader = new BlendShader(BLEND_VERT, BLEND_FRAG);
    displayShader = new DisplayShader(DISPLAY_VERT, DISPLAY_FRAG);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDisable(GL_CULL_FACE);


    ///////////////////////
    //    LOAD MODELS    //
    ///////////////////////
    
    vector<float> vertices;
    vector<unsigned int> indices;
    loadModel(DEFAULT_MODEL, vertices, indices);

    // Add round
    float ground[] = {
         0.9, -0.3,  0.9, 0, 1, 0,
         0.9, -0.3, -0.9, 0, 1, 0,
        -0.9, -0.3,  0.9, 0, 1, 0,
         0.9, -0.3, -0.9, 0, 1, 0,
        -0.9, -0.3,  0.9, 0, 1, 0,
        -0.9, -0.3, -0.9, 0, 1, 0
    };
    for(int i = 0; i < 36; i++)
        vertices.push_back(ground[i]);
    

    cerr<<"initQuad();"<<endl;
    initQuad();
    cerr<<"geomShader->init(vertices, indices);"<<endl;
    geomShader->init(vertices, indices); 
    cerr<<"depthShader->init();"<<endl;
    depthShader->init(vertices);
    cerr<<"depthBackgroundShader->init();"<<endl;
    depthBackgroundShader->init();
    cerr<<"shadowShader->init();"<<endl;
    shadowShader->init();
    cerr<<"blendShader->init();"<<endl;
    blendShader->init();
    cerr<<"displayShader->init();"<<endl;
    displayShader->init();
}


void Shader::updateShader(ShaderArg* arg = nullptr) {

    // Compute params
    const float rotationRate = 0.3;
    float brightness = arg->sliderValue;
    float sceneRotationY = - arg->arrowXstate * rotationRate;
    float sceneRotationX = arg->arrowYstate * rotationRate;

    if(arg) delete arg;

    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    geomShader->render(sceneRotationY, sceneRotationX);
    depthShader->render();
    depthBackgroundShader->render(depthShader->gDepth, depthShader->gDepth2);
    shadowShader->render(geomShader->gPosition, depthBackgroundShader->gDepth, depthBackgroundShader->gDepth2);
    blendShader->render(shadowShader->gShadow);
    // displayShader->render(depthBackgroundShader->gDepth);

}


#endif
