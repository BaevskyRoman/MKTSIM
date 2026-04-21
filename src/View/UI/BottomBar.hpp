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
    const char* shapes[4] = {"Rectangle", "Circle", "Brush", nullptr};
    int currentShape = 0;
    const char* qtyModes[3] = {"Concentration", "QTY", nullptr};
    int currentQTYMode = 0;
    const char* massModes[3] = {"Mass", "Density", nullptr};
    int currentMassMode = 0;

    float concentration = 0.5f;
    int qty = 0;
    float minSpeed = 0.0f;
    float maxSpeed = 200.0f;
    float mass = Config::Physics::MIN_MOLECULE_MASS;
    float density = 1;
    float radius = 5;
};


struct StaticBodySettings {
    int currentShape = 0;
    const char* shapes[2] = {"Box", "Rectangle"};
    float thickness = 50.f;
};


struct DynamicBodySettings {
    int currentMassMode = 0;
    const char* massModes[2] = {"Density", "Mass"};
    float mass = 10;
    float density = 10;
};


class BottomBar {
public:
    void update(sf::RenderWindow& window);


    MoleculesSettings molSettings_;
    StaticBodySettings staticBodySettings_;
    DynamicBodySettings dynamicBodySettings_;

    ToolType activeTool_ = ToolType::Molecules;
    bool showToolSettings_ = false;

private:
    void drawBottomBar(const sf::RenderWindow& window);
    void drawToolButton(const char* label, ToolType tool);
    void drawToolSettings();
};

}
}