#pragma once

#include "Model/Engine.hpp"
#include "Model/FileHandler.hpp"
#include "Model/SimulationRecorder.hpp"
#include "View/Renderer.hpp"
#include "View/UI/Manager.hpp"
#include <SFML/Graphics.hpp>
#include <thread>
#include <atomic>


namespace Controller {

class AppController {
public:
    AppController();
    ~AppController();
    
    void run();

private:
    void update(float deltaTime);
    void render();
    void processEvents();

    void selectionStart(const sf::Event::MouseButtonPressed* e);
    sf::FloatRect selectionEnd();

    sf::RenderWindow window_;
    Model::Engine engine_;
    // Model::FileHandler fileHandler_;
    Model::SimulationRecorder recorder_;
    View::Renderer renderer_;
    View::UI::Manager manager_;

    enum class State {
        Realtime,
        Playback,
        Baking
    };
    State currentState_ = State::Realtime;
    bool isPlaying = false;

    std::thread bakeThread_;
    std::atomic<bool> isBaking_{false};
    std::atomic<float> bakeProgress_{0.0f};

    void startBaking();
    void startPlayback();
    void bakeTask(float durationSeconds, float fixedDeltaTime);

    bool isDragging_ = false;
    sf::Vector2i lastMousePos_;

    bool isSelecting_ = false;
    sf::Vector2f selectionStartPos_;
    sf::Vector2f selectionEndPos_;
};

}