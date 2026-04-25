#pragma once

#include "Model/Engine.hpp"
#include "Model/FileHandler.hpp"
#include "View/Renderer.hpp"
#include "View/UI/Manager.hpp"
#include <SFML/Graphics.hpp>


namespace Controller {

class AppController {
public:
    AppController();
    
    void run();

private:
    void update(float deltaTime);
    void render();
    void processEvents();

    void selectionStart(const sf::Event::MouseButtonPressed* e);
    sf::FloatRect selectionEnd();

    sf::RenderWindow window_;
    Model::Engine engine_;
    Model::FileHandler fileHandler_;
    View::Renderer renderer_;
    View::UI::Manager manager_;

    bool isDragging_ = false;
    sf::Vector2i lastMousePos_;

    bool isSelecting_ = false;
    sf::Vector2f selectionStartPos_;
    sf::Vector2f selectionEndPos_;
};

}