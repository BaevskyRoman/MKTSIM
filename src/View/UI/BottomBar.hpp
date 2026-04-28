#pragma once

#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>


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
    float mass = 1;
    float density = 1;
    float radius = 5;
};


struct StaticBodySettings {
    int currentShape = 0;
    const char* shapes[3] = {"Box", "Rectangle", nullptr};
    float thickness = 50.f;
};


struct DynamicBodySettings {
    int currentMassMode = 0;
    const char* massModes[3] = {"Density", "Mass", nullptr};
    float mass = 10;
    float density = 10;
};


struct ToolSettings {
    ToolType activeTool = ToolType::Molecules;
    
    MoleculesSettings mol;
    StaticBodySettings bodyS;
    DynamicBodySettings bodyD;
};


class BottomBar {
public:
    void update(sf::RenderWindow& window);

    ToolSettings settings;
    bool showToolSettings_ = false;
    
private:
    void drawBottomBar(const sf::RenderWindow& window);
    void drawToolButton(const char* label, ToolType tool);
    void drawToolSettings();
};

}
}