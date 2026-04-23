#pragma once

#include <SFML/Graphics.hpp>

#include "Model/Engine.hpp"
#include "View/Renderer.hpp"
#include "View/UI/Manager.hpp"
#include "Model/FileHandler.hpp"


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

    void selectionStart(const sf::Event::MouseButtonPressed* e);
    sf::FloatRect selectionEnd();

    sf::RenderWindow window_;
    Model::Engine engine_;
    View::Renderer renderer_;
    View::UI::Manager manager_;
    Model::FileHandler fileHandler_;

    bool isDragging_ = false;
    sf::Vector2i lastMousePos_;

    bool isSelecting_ = false;
    sf::Vector2f selectionStartPos_;
    sf::Vector2f selectionEndPos_;
};

}