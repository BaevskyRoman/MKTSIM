#pragma once

#include <SFML/Graphics.hpp>

#include "Model/Engine.hpp"
#include "View/Renderer.hpp"
#include "View/UI/BottomBar.hpp"


namespace Controller {

class AppController {
public:
    AppController();
    ~AppController();

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window_;
    Model::Engine engine_;
    View::Renderer renderer_;
    View::UI::BottomBar bottomBar_;

    bool isDragging_ = false;
    sf::Vector2i lastMousePos_;

    bool isSelecting_ = false;
    sf::Vector2f selectionStart_;
    sf::Vector2f selectionEnd_;
};

}