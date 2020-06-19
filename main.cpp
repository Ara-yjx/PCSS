#include <nanogui/nanogui.h>
#include <iostream>
#include <string>
#include "shader.h"
#include "scene.h"

using namespace nanogui;
using std::cout;
using std::endl;
using Eigen::Vector3f;


class App : public Screen, public Shader {
public:
    double sliderValue = 0.5;
    float arrowXstate = 0;
    float arrowXStateSmooth = 0;
    float arrowYstate = 0;
    float arrowYStateSmooth = 0;
    float bracketState = 0;
    float bracketStateSmooth = 0;
    bool switchState1 = true;
    bool switchState2 = true;
    // nanogui::GLShader mShader;
    unsigned int shaderProgram;
    unsigned int VAO;

    
    Shader* shader;
    Scene scene;

    App() : Screen(Eigen::Vector2i(768, 768), "PCSS"), Shader() {
        
        scene.light[0].position = Vector3f(0,2,0);
        // scene.light[0].displacement = Vector3f(0,0,0);
        scene.light[0].size = 0.2;
        scene.light[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        scene.light[0].on = true;

        scene.light[1].position = Vector3f(1,2,0);
        // scene.light[1].displacement = Vector3f(0,0,0);
        scene.light[1].size = 0.2;
        scene.light[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        scene.light[1].on = true;

        scene.light[2].position = Vector3f(0,2,1);
        // scene.light[2].displacement = Vector3f(0,0,0);
        scene.light[2].size = 0.2;
        scene.light[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
        scene.light[2].on = true;

        scene.shadowOn = true;


        Window* window = new Window(this, "-");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());

        // Slider *slider = new Slider(window);
        // slider->setFixedSize(Vector2i(100, 20));
        // slider->setValue(this->sliderValue);
        // slider->setCallback([&](float value) {
        //     this->sliderValue = value;
        //     // cout << sliderValue << endl;
        // });
        // slider->setFinalCallback([&](float value) {
        //     cout << "INFO slider value: " << value << endl;
        // });

        CheckBox *checkboxShadow = new CheckBox(window, "Shadow");
        checkboxShadow->setFontSize(16);
        checkboxShadow->setChecked(true);
        checkboxShadow->setCallback([&](bool value) {
            this->scene.shadowOn = value;
        });

        new Label(window, "Light 0", "sans-bold");

        ColorPicker *cp = new ColorPicker(window, {255, 120, 0, 255});
        cp->setFixedSize({100, 25});
        cp->setFinalCallback([&](const Color &c) {
            // this->scene.light[0].color = c;
        });


        


        initShader();
    }


    bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        // esc
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            setVisible(false);
            return true;
        }
        // arrow
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        	this->arrowYstate += 1;
            return true;
        }
        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        	this->arrowYstate -= 1;
            return true;
        }
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        	this->arrowXstate -= 1;
            return true;
        }
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        	this->arrowXstate += 1;
            return true;
        }
        // < >
        if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS) {
        	this->bracketState -= 1;
            return true;
        }
        if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
        	this->bracketState += 1;
            return true;
        }
        return false;
    }



    virtual void drawContents() {
        float rotationRate = 0.3;
        arrowXStateSmooth = arrowXStateSmooth * 0.9 + arrowXstate * 0.1;
        arrowYStateSmooth = arrowYStateSmooth * 0.9 + arrowYstate * 0.1;
        // bracketStateSmooth = bracketStateSmooth * 0.9 + bracketState * 0.1;


        scene.rotationX = arrowYStateSmooth * rotationRate;
        scene.rotationY = arrowXStateSmooth * rotationRate;

        updateShader(scene);
    }

};


int main(int argc, char const *argv[]) {

    nanogui::init();

    App* screen = new App();

    // Launch window
    screen->setVisible(true);
    screen->performLayout();
    // window->center();

    nanogui::mainloop();
    nanogui::shutdown();
    return 0;
}