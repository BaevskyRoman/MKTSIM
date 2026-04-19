#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Config/PhysicsConfig.hpp"

namespace View {
namespace UI {

enum class ToolType {
    Molecules,
    StaticBody,
    DynamicBody,
};


struct MoleculesSettings {
    int currentShape = 0;
    const char* shapes[2] = {"Rectangle", "Circle"};
    float concentration = 0.5f;
    float minSpeed = 0.0f;
    float maxSpeed = 100.0f;
    int mass = Config::Physics::MIN_MOLECULE_MASS;
    int radius = 5;
};


struct StaticBodySettings {
    int currentShape = 0;
    const char* shapes[2] = {"Rectangle", "Box"};
    float thickness = 10.f;
};


struct DynamicBodySettings {
    int currentCalcMode = 0;
    const char* CalcModes[2] = {"Density", "Mass"};
    float mass = 10;
    float density = 10;
};


class BottomBar {
public:
    BottomBar() = default;
    ~BottomBar() = default;

    bool init(sf::RenderWindow& window);
    void shutdown();

    void update(sf::RenderWindow& window, sf::Time dt);
    void render(sf::RenderWindow& window);

    ToolType getActiveTool() const { return activeTool_; }

    MoleculesSettings molSettings_;
    StaticBodySettings staticBodySettings_;
    DynamicBodySettings dynamicBodySettings_;

private:
    void drawBottomBar(const sf::RenderWindow& window);
    void drawToolButton(const char* label, ToolType tool);
    void drawToolSettings();

    ToolType activeTool_ = ToolType::Molecules;
    bool showToolSettings_ = false;
};


inline ImVec4 toImVec4(const sf::Color& color) {
    return ImVec4(
        static_cast<float>(color.r) / 255.0f,
        static_cast<float>(color.g) / 255.0f,
        static_cast<float>(color.b) / 255.0f,
        static_cast<float>(color.a) / 255.0f
    );
}

}
}