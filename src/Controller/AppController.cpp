#include "Controller/AppController.hpp"
#include "Config/VisualConfig.hpp"


namespace Controller {

AppController::AppController()
    : window_(sf::VideoMode({Config::Visual::WINDOW_WIDTH, Config::Visual::WINDOW_HEIGHT}), 
              Config::Visual::WINDOW_TITLE),
    renderer_(Config::Visual::WINDOW_WIDTH, Config::Visual::WINDOW_HEIGHT)
{
    window_.setFramerateLimit(Config::Visual::FPS_LIMIT);
    
    if (!bottomBar_.init(window_)) {
        throw std::runtime_error("[AppController::AppController] Failed bottomBar init");
    }
}


AppController::~AppController() { 
    bottomBar_.shutdown(); 
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
    while (const std::optional<sf::Event> event = window_.pollEvent()) {
        ImGui::SFML::ProcessEvent(window_, *event);

        if (event->is<sf::Event::Closed>()) {
            window_.close();
        }
        
        else if (const auto* e = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (ImGui::GetIO().WantCaptureMouse) continue;

            if (e->button == sf::Mouse::Button::Right) {
                isDragging_ = true;
                lastMousePos_ = e->position; 
            } else if (e->button == sf::Mouse::Button::Left) {
                if (bottomBar_.getActiveTool() == View::UI::ToolType::Molecules) {
                    isSelecting_ = true;
                    selectionStart_ = window_.mapPixelToCoords(e->position, renderer_.getCamera());
                    selectionEnd_ = selectionStart_;
                }
            }
        }
        
        else if (const auto* e = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (e->button == sf::Mouse::Button::Right) {
                isDragging_ = false;
            } else if (e->button == sf::Mouse::Button::Left) {
                if (isSelecting_ && bottomBar_.getActiveTool() == View::UI::ToolType::Molecules) {
                    isSelecting_ = false;
                    
                    if (!ImGui::GetIO().WantCaptureMouse) {
                        float left = std::min(selectionStart_.x, selectionEnd_.x);
                        float top = std::min(selectionStart_.y, selectionEnd_.y);
                        float width = std::abs(selectionStart_.x - selectionEnd_.x);
                        float height = std::abs(selectionStart_.y - selectionEnd_.y);
                        
                        if (width > 1.0f && height > 1.0f) {
                            View::UI::MoleculesSettings settings = bottomBar_.getMolSettings();
                            sf::FloatRect area(sf::Vector2f(left, top), sf::Vector2f(width, height));
                            engine_.spawnMoleculesInArea(area, settings.concentration, settings.maxSpeed, 
                                settings.mass, settings.radius);
                        }
                    }
                }
            }
        }
        
        else if (const auto* e = event->getIf<sf::Event::MouseMoved>()) {
            if (isDragging_) {
                sf::Vector2f oldPos = window_.mapPixelToCoords(lastMousePos_, renderer_.getCamera());
                sf::Vector2f newPos = window_.mapPixelToCoords(e->position, renderer_.getCamera());
                
                sf::Vector2f delta = oldPos - newPos;
                renderer_.moveCamera(delta.x, delta.y);
                
                lastMousePos_ = e->position;
            }
            
            if (isSelecting_) {
                selectionEnd_ = window_.mapPixelToCoords(e->position, renderer_.getCamera());
            }
        }
        
        else if (const auto* e = event->getIf<sf::Event::MouseWheelScrolled>()) {
            if (ImGui::GetIO().WantCaptureMouse) continue;

            if (e->wheel == sf::Mouse::Wheel::Vertical) { 
                if (e->delta > 0) {
                    renderer_.zoomCamera(0.9f);
                } else if (e->delta < 0) {
                    renderer_.zoomCamera(1.1f);
                }
            }
        }
    }
}


void AppController::update(float deltaTime) {
    bottomBar_.update(window_, sf::seconds(deltaTime));
    engine_.update(deltaTime);
}


void AppController::render() {
    window_.clear(Config::Visual::BG_COLOR);
    renderer_.draw(window_, engine_);
    if (isSelecting_) {
        renderer_.drawSelection(window_, selectionStart_, selectionEnd_);
    }
    bottomBar_.render(window_);
    window_.display();
}

}