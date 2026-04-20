#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "View/UI/BottomBar.hpp"


namespace View {
namespace UI {

class Manager {
public:
    bool init(sf::RenderWindow& window);
    ~Manager();

    void update(sf::RenderWindow& window, sf::Time dt);
    void render(sf::RenderWindow& window);

    View::UI::ToolType getActiveTool() { return bottomBar_.activeTool_; }

    template <typename T>
    T& getToolSettings() {
        if constexpr (std::is_same_v<T, View::UI::MoleculesSettings>) {
            return bottomBar_.molSettings_;
        } else if constexpr (std::is_same_v<T, View::UI::StaticBodySettings>) {
            return bottomBar_.staticBodySettings_;
        } else if constexpr (std::is_same_v<T, View::UI::DynamicBodySettings>) {
            return bottomBar_.dynamicBodySettings_;
        }
    }

private:
    BottomBar bottomBar_;
};

}
}