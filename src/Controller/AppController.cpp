#include "Controller/AppController.hpp"
#include "Config/VisualConfig.hpp"
#include <cstring>
#include <iostream>


namespace Controller {

AppController::AppController()
    : window_(sf::VideoMode({Config::Visual::WINDOW_WIDTH, Config::Visual::WINDOW_HEIGHT}), 
              Config::Visual::WINDOW_TITLE),
    renderer_(Config::Visual::WINDOW_WIDTH, Config::Visual::WINDOW_HEIGHT),
    manager_(engine_)
{
    window_.setFramerateLimit(Config::Visual::FPS_LIMIT);
    
    if (!manager_.init(window_)) {
        throw std::runtime_error("[AppController::AppController] Failed UI Manager init");
    }

    manager_.setOnBake([this]() { this->startBaking(); });
    manager_.setOnPlayback([this]() { this->startPlayback(); });
}


void AppController::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        update(deltaTime);
        render();
        processEvents();
    }
}


void AppController::update(float deltaTime) {
    manager_.update(window_, sf::seconds(deltaTime));

    if (!isPlaying) return;
    if (currentMode_ == AppMode::Realtime) {
        engine_.update(deltaTime);
    }
    else if (currentMode_ == AppMode::Playback) {
        if (!recorder_.playNextFrame(engine_)) {
            currentMode_ = AppMode::Realtime;
            isPlaying = false;
            recorder_.stopPlayback();
        }
    }
    else if (currentMode_ == AppMode::Recording) {
        engine_.update(1.f/60);
        recorder_.recordFrame(engine_);
    }
}


void AppController::render() {
    window_.clear(Config::Visual::BG_COLOR);
    renderer_.draw(window_, engine_);
    if (isSelecting_) {
        renderer_.drawSelection(window_, selectionStartPos_, selectionEndPos_);
    }
    manager_.render(window_);
    window_.display();
}


void AppController::processEvents() {
    View::UI::ToolSettings overallSettings = manager_.getToolSettings();

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
                switch (manager_.getToolSettings().activeTool) {
                case View::UI::ToolType::Molecules:
                    selectionStart(e);
                    break;
                case View::UI::ToolType::StaticBody:
                    selectionStart(e);
                    break;
                case View::UI::ToolType::DynamicBody:
                    selectionStart(e);
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

                switch (overallSettings.activeTool) {
                case View::UI::ToolType::Molecules: {
                    View::UI::MoleculesSettings& settings = overallSettings.mol;
                    sf::FloatRect area = selectionEnd();
                    if (area.size.x < 1.0f || area.size.y < 1.0f) continue;
                    if (settings.currentQTYMode == 0) {
                        engine_.spawnMoleculesInArea(area, settings.concentration, 
                                                settings.minSpeed, settings.maxSpeed, settings.mass, settings.radius);
                    } else if (settings.currentQTYMode == 1) {
                        engine_.spawnMoleculesInArea(area, settings.qty, 
                                                settings.minSpeed, settings.maxSpeed, settings.mass, settings.radius);
                    }
                    break;
                }
                case View::UI::ToolType::StaticBody: {
                    View::UI::StaticBodySettings& settings = overallSettings.bodyS;
                    sf::FloatRect area = selectionEnd();
                    if (area.size.x < 1.0f || area.size.y < 1.0f) continue;
                    if (std::strcmp(settings.shapes[settings.currentShape], "Rectangle") == 0) {
                        engine_.spawnStaticBody(area);
                    } else if (std::strcmp(settings.shapes[settings.currentShape], "Box") == 0) {
                        float th = settings.thickness;
                        engine_.spawnStaticBody(sf::FloatRect(area.position, sf::Vector2f(area.size.x, th)));
                        engine_.spawnStaticBody(sf::FloatRect(area.position + sf::Vector2f(area.size.x - th, 0), 
                                        sf::Vector2f(th, area.size.y)));
                        engine_.spawnStaticBody(sf::FloatRect(area.position, sf::Vector2f(th, area.size.y)));
                        engine_.spawnStaticBody(sf::FloatRect(area.position + sf::Vector2f(0, area.size.y - th),
                    sf::Vector2f(area.size.x, th)));
                    }
                    break;
                }
                case View::UI::ToolType::DynamicBody: {
                    View::UI::DynamicBodySettings& settings = overallSettings.bodyD;
                    sf::FloatRect area = selectionEnd();
                    if (area.size.x < 1.0f || area.size.y < 1.0f) continue;

                    if (strcmp(settings.massModes[settings.currentMassMode], "Mass") == 0) {
                        engine_.spawnDynamicBody(area.size, area.position + sf::Vector2f(area.size.x/2, area.size.y/2), settings.mass);
                    } else if (strcmp(settings.massModes[settings.currentMassMode], "Density") == 0) {
                        engine_.spawnDynamicBody(area.size, area.position + sf::Vector2f(area.size.x/2, area.size.y/2), settings.density*area.size.x*area.size.y);
                    }
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

        // --- BARS HIDING (T, B) ---
        // --- STOP/PLAY (ENTER) ---
        if (const auto* e = event->getIf<sf::Event::KeyPressed>()) {
            if (ImGui::GetIO().WantCaptureKeyboard) continue;
            
            if (e->code == sf::Keyboard::Key::T) {
                manager_.topBarVisible_ = !manager_.topBarVisible_;
            } else if (e->code == sf::Keyboard::Key::B) {
                manager_.bottomBarVisible_ = !manager_.bottomBarVisible_;
            } else if (e->code == sf::Keyboard::Key::Enter) {
                isPlaying = !isPlaying;
            }

            continue;
        }
    }
}


void AppController::startBaking() {
    if (recorder_.startRecording(engine_, "simulation")) {
        currentMode_ = AppMode::Recording;
        isPlaying = true;
        std::cout << "fas" << std::endl;
    }
}


void AppController::startPlayback() {
    if (recorder_.startPlayback(engine_, "simulation")) {
        currentMode_ = AppMode::Playback;
    }
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
