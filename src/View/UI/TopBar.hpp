#pragma once

#include "Model/Engine.hpp"
#include "Model/FileHandler.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>


namespace View {
namespace UI {

class TopBar {
public:
    TopBar(Model::Engine& engine) : engineRef_(engine) {}
    void update(sf::RenderWindow& window, sf::Time dt);

private:
    void drawTopBar(sf::RenderWindow& window);
    void drawSimSettings(sf::RenderWindow& window);
    void drawSimStats(sf::RenderWindow& window, sf::Time dt);
    void drawFileManager(sf::RenderWindow& window);

    bool showSimSettings_ = false;
    bool showSimStats_ = false;
    bool showFileManager_ = false;
    Model::Engine& engineRef_;
    Model::FileHandler fileHandler_;
};

}
}