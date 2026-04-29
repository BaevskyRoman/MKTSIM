#pragma once

#include "Model/Engine.hpp"
#include "Model/FileHandler.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <functional>


namespace View {
namespace UI {

class TopBar {
public:
    TopBar(Model::Engine& engine) : engineRef_(engine) {}
    void update(sf::RenderWindow& window, sf::Time dt);

    std::function<void()> onBakeRequested;
    std::function<void()> onPlaybackRequested;
    
private:
    void drawTopBar(sf::RenderWindow& window, sf::Time dt);
    void drawSimSettings(sf::RenderWindow& window);
    void drawFileManager(sf::RenderWindow& window);
    void drawRecorder(sf::RenderWindow& window);

    bool showSimSettings_ = false;
    bool showFileManager_ = false;
    bool showRecorder_ = false;
    Model::Engine& engineRef_;
    Model::FileHandler fileHandler_;
};

}
}