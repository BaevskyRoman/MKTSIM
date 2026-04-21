#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Config/PhysicsConfig.hpp"
#include "Model/Engine.hpp"


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

    bool showSimSettings_ = false;
    bool showSimStats_ = false;
    Model::Engine& engineRef_;
};

}
}