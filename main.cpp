#include <nanogui/nanogui.h>
#include <iostream>
#include <string>
#include "shader.h"
// #include "shader.cpp"

using namespace nanogui;
using std::cout;
using std::endl;


class App : public Screen, public Shader {
public:
    double sliderValue = 0.5;
    float arrowXstate = 0;
    float arrowXStateSmooth = 0;
    float arrowYstate = 0;
    float bracketState = 0;
    float bracketStateSmooth = 0;
    bool switchState1 = true;
    bool switchState2 = true;
    // nanogui::GLShader mShader;
    unsigned int shaderProgram;
    unsigned int VAO;

    
    Shader* shader;

    App() : Screen(Eigen::Vector2i(768, 768), "PRT"), Shader() {

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

        CheckBox *checkbox1 = new CheckBox(window, "Shadow");
        checkbox1->setFontSize(16);
        checkbox1->setChecked(this->switchState1);
        checkbox1->setCallback([&](bool value) {
            this->switchState1 = value;
        });

        CheckBox *checkbox2 = new CheckBox(window, "filter");
        checkbox2->setFontSize(16);
        checkbox2->setChecked(this->switchState2);
        checkbox2->setCallback([&](bool value) {
            this->switchState2 = value;
        });

        initShader(nullptr);
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
        arrowXStateSmooth = arrowXStateSmooth * 0.9 + arrowXstate * 0.1;
        bracketStateSmooth = bracketStateSmooth * 0.9 + bracketState * 0.1;
        updateShader(new ShaderArg(sliderValue, arrowXStateSmooth, arrowYstate, bracketStateSmooth, switchState1, switchState2));
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