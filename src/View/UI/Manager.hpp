#pragma once

#include "Model/Engine.hpp"
#include "View/UI/BottomBar.hpp"
#include "View/UI/TopBar.hpp"
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>


namespace View {
namespace UI {

class Manager {
public:
    bool init(sf::RenderWindow& window);
    Manager(Model::Engine& engine) : topBar_(engine) {}
    ~Manager();

    void update(sf::RenderWindow& window, sf::Time dt);
    void render(sf::RenderWindow& window);

    ToolSettings& getToolSettings() { return bottomBar_.settings; }

    bool bottomBarVisible_ = true, topBarVisible_ = true;

private:
    BottomBar bottomBar_;
    TopBar topBar_;
};

}
}