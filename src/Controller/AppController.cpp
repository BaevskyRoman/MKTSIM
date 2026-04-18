#include "Controller/AppController.hpp"
#include "Config/VisualConfig.hpp"
#include <cstring>


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
            continue;
        }
        
        // --- MOUSE BUTTON PRESSED ---
        if (const auto* e = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (ImGui::GetIO().WantCaptureMouse) continue;

            if (e->button == sf::Mouse::Button::Right) {
                isDragging_ = true;
                lastMousePos_ = e->position;
            } else if (e->button == sf::Mouse::Button::Left) {
                switch (bottomBar_.getActiveTool()) {
                case View::UI::ToolType::Molecules:
                    selectionStart(e);
                    break;
                case View::UI::ToolType::HardMacroObject:
                    selectionStart(e);
                    break;
                case View::UI::ToolType::SoftMacroObject:
                    break;
                }
            }

            continue;
        }
        
        // --- MOUSE BUTTON RELEASED ---
        if (const auto* e = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (e->button == sf::Mouse::Button::Right) {
                isDragging_ = false;
            } else if (e->button == sf::Mouse::Button::Left) {
                if (!isSelecting_) continue;
                if (ImGui::GetIO().WantCaptureMouse) continue;

                switch (bottomBar_.getActiveTool()) {
                case View::UI::ToolType::Molecules: {
                    View::UI::MoleculesSettings settings = bottomBar_.molSettings_;
                    sf::FloatRect area = selectionEnd();
                    if (area.size.x > 1.0f && area.size.y > 1.0f) {
                        engine_.spawnMoleculesInArea(area, settings.concentration, 
                            settings.minSpeed, settings.maxSpeed, settings.mass, settings.radius);
                    }
                    break;
                }
                case View::UI::ToolType::HardMacroObject: {
                    View::UI::HMOSettings settings = bottomBar_.hmoSettings_;
                    sf::FloatRect area = selectionEnd();
                    if (std::strcmp(settings.shapes[settings.currentShape], "Rectangle") == 0) {
                        if (area.size.x > 1.0f && area.size.y > 1.0f) {
                            engine_.addHardMO(area);
                        }
                    } else if (std::strcmp(settings.shapes[settings.currentShape], "Box") == 0) {
                        float th = settings.thickness;
                        engine_.addHardMO(sf::FloatRect(area.position, sf::Vector2f(area.size.x, th)));
                        engine_.addHardMO(sf::FloatRect(area.position + sf::Vector2f(area.size.x - th, 0), 
                                        sf::Vector2f(th, area.size.y)));
                        engine_.addHardMO(sf::FloatRect(area.position, sf::Vector2f(th, area.size.y)));
                        engine_.addHardMO(sf::FloatRect(area.position + sf::Vector2f(0, area.size.y - th),
                    sf::Vector2f(area.size.x, th)));
                    }
                    break;
                }
                case View::UI::ToolType::SoftMacroObject: {
                    break;
                }
                }
            }

            continue;
        }
        
        // --- MOUSE MOVED ---
        if (const auto* e = event->getIf<sf::Event::MouseMoved>()) {
            if (isDragging_) {
                sf::Vector2f oldPos = window_.mapPixelToCoords(lastMousePos_, renderer_.getCamera());
                sf::Vector2f newPos = window_.mapPixelToCoords(e->position, renderer_.getCamera());
                
                sf::Vector2f delta = oldPos - newPos;
                renderer_.moveCamera(delta.x, delta.y);
                
                lastMousePos_ = e->position;
            }
            
            if (isSelecting_) {
                selectionEndPos_ = window_.mapPixelToCoords(e->position, renderer_.getCamera());
            }

            continue;
        }
        
        // --- SCROLL ---
        if (const auto* e = event->getIf<sf::Event::MouseWheelScrolled>()) {
            if (ImGui::GetIO().WantCaptureMouse) continue;

            if (e->wheel == sf::Mouse::Wheel::Vertical) { 
                if (e->delta > 0) {
                    renderer_.zoomCamera(0.9f);
                } else if (e->delta < 0) {
                    renderer_.zoomCamera(1.1f);
                }
            }

            continue;
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
        renderer_.drawSelection(window_, selectionStartPos_, selectionEndPos_);
    }
    bottomBar_.render(window_);
    window_.display();
}


void AppController::selectionStart(const sf::Event::MouseButtonPressed* e) {
    isSelecting_ = true;
    selectionStartPos_ = window_.mapPixelToCoords(e->position, renderer_.getCamera());
    selectionEndPos_ = selectionStartPos_;
}


sf::FloatRect AppController::selectionEnd() {
    isSelecting_ = false;
                    
    float left = std::min(selectionStartPos_.x, selectionEndPos_.x);
    float top = std::min(selectionStartPos_.y, selectionEndPos_.y);
    float width = std::abs(selectionStartPos_.x - selectionEndPos_.x);
    float height = std::abs(selectionStartPos_.y - selectionEndPos_.y);
    
    return sf::FloatRect(sf::Vector2f(left, top), sf::Vector2f(width, height));
}
}