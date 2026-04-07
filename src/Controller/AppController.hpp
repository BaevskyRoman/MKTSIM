#pragma once

#include <SFML/Graphics.hpp>
#include "Model/Engine.hpp"
#include "View/Renderer.hpp"
#include "View/UI/UIManager.hpp"


namespace Controller {

class AppController {
public:
    AppController();
    ~AppController() = default;

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window_;
    Model::Engine engine_;
    View::Renderer renderer_;
    //View::UI::UIManager uiManager_;

    bool isDragging_ = false;
    sf::Vector2i lastMousePos_;
};

}