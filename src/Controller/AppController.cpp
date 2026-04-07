#include "Controller/AppController.hpp"
#include "Config/VisualConfig.hpp"

namespace Controller {

AppController::AppController()
    : window_(sf::VideoMode({Config::Visual::WINDOW_WIDTH, Config::Visual::WINDOW_HEIGHT}), 
              Config::Visual::WINDOW_TITLE),
    renderer_(Config::Visual::WINDOW_WIDTH, Config::Visual::WINDOW_HEIGHT)
{
    window_.setFramerateLimit(Config::Visual::FPS_LIMIT); 
}

void AppController::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        processEvents();
        update(deltaTime);
        render();
    }
}

void AppController::processEvents() {
    window_.handleEvents(
        [this](const sf::Event::Closed&) {
            window_.close();
        },
        
        [this](const sf::Event::MouseWheelScrolled& e) {
            if (e.wheel == sf::Mouse::Wheel::Vertical) { 
                if (e.delta > 0) {
                    renderer_.zoomCamera(0.9f);
                } else if (e.delta < 0) {
                    renderer_.zoomCamera(1.1f);
                }
            }
        },

        [this](const sf::Event::MouseButtonPressed& e) {
            if (e.button == sf::Mouse::Button::Right) {
                isDragging_ = true;
                lastMousePos_ = e.position; 
            }
        },

        [this](const sf::Event::MouseButtonReleased& e) {
            if (e.button == sf::Mouse::Button::Right) {
                isDragging_ = false;
            }
        },

        [this](const sf::Event::MouseMoved& e) {
            if (isDragging_) {
                sf::Vector2f oldPos = window_.mapPixelToCoords(lastMousePos_, renderer_.getCamera());
                sf::Vector2f newPos = window_.mapPixelToCoords(e.position, renderer_.getCamera());
                
                sf::Vector2f delta = oldPos - newPos;
                
                renderer_.moveCamera(delta.x, delta.y);
                
                lastMousePos_ = e.position;
            }
        }
    );
}

void AppController::update(float deltaTime) {
    engine_.update(deltaTime);
}

void AppController::render() {
    window_.clear(Config::Visual::BG_COLOR);
    renderer_.draw(window_, engine_);
    // uiManager_.draw(window_);
    window_.display();
}

}