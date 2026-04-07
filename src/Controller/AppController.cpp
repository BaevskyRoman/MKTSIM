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
        // MOUSE BUTTON PRESSED
        // LEFT TO SPAWN
        // RIGHT TO MOVE CAMERA
        [this](const sf::Event::MouseButtonPressed& e) {
            if (e.button == sf::Mouse::Button::Right) {
                isDragging_ = true;
                lastMousePos_ = e.position; 
            } else if (e.button == sf::Mouse::Button::Left) {
                isSelecting_ = true;
                selectionStart_ = window_.mapPixelToCoords(e.position, renderer_.getCamera());
                selectionEnd_ = selectionStart_;
            }
        },

        // MOSE BUTTON RELEASED
        [this](const sf::Event::MouseButtonReleased& e) {
            if (e.button == sf::Mouse::Button::Right) {
                isDragging_ = false;
            } else if (e.button == sf::Mouse::Button::Left) {
                if (isSelecting_) {
                    isSelecting_ = false;
                    
                    float left = std::min(selectionStart_.x, selectionEnd_.x);
                    float top = std::min(selectionStart_.y, selectionEnd_.y);
                    float width = std::abs(selectionStart_.x - selectionEnd_.x);
                    float height = std::abs(selectionStart_.y - selectionEnd_.y);
                    
                    if (width > 1.0f && height > 1.0f) {
                        sf::FloatRect area(sf::Vector2f(left, top), sf::Vector2f(width, height));
                        engine_.spawnMoleculesInArea(area, 0.5f, 150.0f, 
                            Config::Physics::DEFAULT_MOLECULE_MASS, 
                            Config::Physics::DEFAULT_MOLECULE_RADIUS);
                    }
                }
            }
        },

        // MOUSE MOVED
        [this](const sf::Event::MouseMoved& e) {
            if (isDragging_) {
                sf::Vector2f oldPos = window_.mapPixelToCoords(lastMousePos_, renderer_.getCamera());
                sf::Vector2f newPos = window_.mapPixelToCoords(e.position, renderer_.getCamera());
                
                sf::Vector2f delta = oldPos - newPos;
                
                renderer_.moveCamera(delta.x, delta.y);
                
                lastMousePos_ = e.position;
            }
            if (isSelecting_) {
                selectionEnd_ = window_.mapPixelToCoords(e.position, renderer_.getCamera());
            }
        },

        // CAMERA ZOOM
        [this](const sf::Event::MouseWheelScrolled& e) {
            if (e.wheel == sf::Mouse::Wheel::Vertical) { 
                if (e.delta > 0) {
                    renderer_.zoomCamera(0.9f);
                } else if (e.delta < 0) {
                    renderer_.zoomCamera(1.1f);
                }
            }
        },

        // CLOSING THE WINDOW
        [this](const sf::Event::Closed&) {
            window_.close();
        }
    );
}

void AppController::update(float deltaTime) {
    engine_.update(deltaTime);
}

void AppController::render() {
    window_.clear(Config::Visual::BG_COLOR);
    renderer_.draw(window_, engine_);
    if (isSelecting_) {
        renderer_.drawSelection(window_, selectionStart_, selectionEnd_);
    }
    window_.display();
}

}