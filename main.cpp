#include <nanogui/nanogui.h>
#include <iostream>
#include <string>
#include "shader.h"
#include "scene.h"

using namespace nanogui;
using std::cout;
using std::endl;
using std::string;
using std::to_string;
using Eigen::Vector3f;


class App : public Screen, public Shader {
public:
    float arrowXstate = 0;
    float arrowXStateSmooth = 0;
    float arrowYstate = 0;
    float arrowYStateSmooth = 0;
    float bracketState = 0;
    float bracketStateSmooth = 0;
    int moveTarget = 0;
    Label *label[3];

    // nanogui::GLShader mShader;
    
    
    Shader* shader;
    Scene scene;

    App() : Screen(Eigen::Vector2i(768, 768), "PCSS"), Shader() {
        
        scene.light[0].position = Vector3f(0,2,0);
        // scene.light[0].displacement = Vector3f(0,0,0);
        scene.light[0].size = 0.05;
        scene.light[0].color = Color(0.8f, 0.8f, 0.8f, 1.0f);
        scene.light[0].on = true;

        scene.light[1].position = Vector3f(0,3,0);
        // scene.light[1].displacement = Vector3f(0,0,0);
        scene.light[1].size = 0.05;
        scene.light[1].color = Color(0.8f, 0.8f, 0.8f, 1.0f);
        scene.light[1].on = true;

        scene.light[2].position = Vector3f(0,5,0);
        // scene.light[2].displacement = Vector3f(0,0,0);
        scene.light[2].size = 0.05;
        scene.light[2].color = Color(0.8f, 0.8f, 0.8f, 1.0f);
        scene.light[2].on = true;

        scene.shadowOn = true;


        Window* window = new Window(this, "-");
        window->setPosition(Vector2i(15, 15));
        window->setLayout(new GroupLayout());


        CheckBox *checkboxShadow = new CheckBox(window, "Shadow");
        checkboxShadow->setFontSize(16);
        checkboxShadow->setChecked(true);
        checkboxShadow->setCallback([&](bool value) {
            this->scene.shadowOn = value;
        });

        new Label(window, "Light 0", "sans-bold");
        label[0] = new Label(window, "Pos (" + to_string(scene.light[0].position.x()) + "," + 
            to_string(scene.light[0].position.y()) + "," + 
            to_string(scene.light[0].position.z()) + ")" , "sans");

        ColorPicker *color0 = new ColorPicker(window, {255, 120, 0, 255});
        color0->setFixedSize({100, 20});
        color0->setFinalCallback([&](const Color &c) {
            this->scene.light[0].color = c;
            cout<<scene.light[0].color<<endl;
        });

        new Label(window, "size", "sans");
        Slider *slider0 = new Slider(window);
        slider0->setFixedSize(Vector2i(100, 20));
        slider0->setValue(this->scene.light[0].size / 0.2);
        slider0->setCallback([&](float value) {
            this->scene.light[0].size = value * 0.5;
        });

        CheckBox *checkbox0 = new CheckBox(window, "On/Off");
        checkbox0->setFontSize(16);
        checkbox0->setChecked(true);
        checkbox0->setCallback([&](bool value) {
            this->scene.light[0].on = value;
        });
        

        new Label(window, "Light 1", "sans-bold");
        label[1] = new Label(window, "Pos (" + to_string(scene.light[1].position.x()) + "," + 
            to_string(scene.light[1].position.y()) + "," + 
            to_string(scene.light[1].position.z()) + ")" , "sans");

        

        ColorPicker *color1 = new ColorPicker(window, {255, 120, 0, 255});
        color1->setFixedSize({100, 20});
        color1->setFinalCallback([&](const Color &c) {
            this->scene.light[1].color = c;
        });

        new Label(window, "size", "sans");
        Slider *slider1 = new Slider(window);
        slider1->setFixedSize(Vector2i(100, 20));
        slider1->setValue(this->scene.light[1].size / 0.2);
        slider1->setCallback([&](float value) {
            this->scene.light[1].size = value * 0.5;
        });

        CheckBox *checkbox1 = new CheckBox(window, "On/Off");
        checkbox1->setFontSize(16);
        checkbox1->setChecked(true);
        checkbox1->setCallback([&](bool value) {
            this->scene.light[1].on = value;
        });


        new Label(window, "Light 2", "sans-bold");
        label[2] = new Label(window, "Pos (" + to_string(scene.light[2].position.x()) + "," + 
            to_string(scene.light[2].position.y()) + "," + 
            to_string(scene.light[2].position.z()) + ")" , "sans");

        ColorPicker *color2 = new ColorPicker(window, {255, 120, 0, 255});
        color2->setFixedSize({100, 20});
        color2->setFinalCallback([&](const Color &c) {
            this->scene.light[2].color = c;
        });

        new Label(window, "size", "sans");
        Slider *slider2 = new Slider(window);
        slider2->setFixedSize(Vector2i(100, 20));
        slider2->setValue(this->scene.light[2].size / 0.2);
        slider2->setCallback([&](float value) {
            this->scene.light[2].size = value * 0.2;
        });

        CheckBox *checkbox2 = new CheckBox(window, "On/Off");
        checkbox2->setFontSize(16);
        checkbox2->setChecked(true);
        checkbox2->setCallback([&](bool value) {
            this->scene.light[2].on = value;
        });


        (new Label(window, "Select a target and move with WASD keys", "sans-bold"))
            ->setFixedSize(Vector2i(120,30));
        ComboBox *cobo =
            new ComboBox(window, { "Light 0", "Light 1", "Light 2" });
        cobo->setFontSize(16);
        cobo->setFixedSize(Vector2i(100,20));
        cobo->setCallback([&](int value) {
            moveTarget = value;
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
        // WASD
        if (key == GLFW_KEY_W && action == GLFW_PRESS) {
            auto pos = &(scene.light[moveTarget].position);
            pos->z() += pos->y() / 3.0;
            label[moveTarget]->setCaption("Pos (" + to_string(scene.light[moveTarget].position.x()) + "," + 
            to_string(scene.light[moveTarget].position.y()) + "," + 
            to_string(scene.light[moveTarget].position.z()) + ")");
            return true;
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS) {
            auto pos = &(scene.light[moveTarget].position);
            pos->z() -= pos->y() / 3;
            label[moveTarget]->setCaption("Pos (" + to_string(scene.light[moveTarget].position.x()) + "," + 
            to_string(scene.light[moveTarget].position.y()) + "," + 
            to_string(scene.light[moveTarget].position.z()) + ")");
            return true;
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS) {
            auto pos = &(scene.light[moveTarget].position);
            pos->x() -= pos->y() / 3;
            label[moveTarget]->setCaption("Pos (" + to_string(scene.light[moveTarget].position.x()) + "," + 
            to_string(scene.light[moveTarget].position.y()) + "," + 
            to_string(scene.light[moveTarget].position.z()) + ")");
            return true;
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            auto pos = &(scene.light[moveTarget].position);
            pos->x() += pos->y() / 3;
            label[moveTarget]->setCaption("Pos (" + to_string(scene.light[moveTarget].position.x()) + "," + 
            to_string(scene.light[moveTarget].position.y()) + "," + 
            to_string(scene.light[moveTarget].position.z()) + ")");
            return true;
        }
        // // < >
        // if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS) {
        // 	this->bracketState -= 1;
        //     return true;
        // }
        // if (key == GLFW_KEY_COMMA && action == GLFW_PRESS) {
        // 	this->bracketState += 1;
        //     return true;
        // }
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