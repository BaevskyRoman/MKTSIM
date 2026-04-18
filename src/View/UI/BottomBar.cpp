#include "View/UI/BottomBar.hpp"
#include "Config/VisualConfig.hpp"


namespace View {
namespace UI {


bool BottomBar::init(sf::RenderWindow& window) {
    return ImGui::SFML::Init(window);
}


void BottomBar::shutdown() {
    ImGui::SFML::Shutdown();
}


void BottomBar::update(sf::RenderWindow& window, sf::Time dt) {
    ImGui::SFML::Update(window, dt);

    drawBottomBar(window);
    drawToolSettings();
}


void BottomBar::render(sf::RenderWindow& window) {
    ImGui::SFML::Render(window);
}


void BottomBar::drawBottomBar(const sf::RenderWindow& window) {
    ImGui::SetNextWindowPos(ImVec2(0.0f, window.getSize().y - Config::Visual::BOTTOM_BAR_HEIGHT));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window.getSize().x), Config::Visual::BOTTOM_BAR_HEIGHT));

    ImGuiWindowFlags windowFlags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoScrollWithMouse | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("BottomBar", nullptr, windowFlags);

    drawToolButton("Molecules", ToolType::Molecules);
    ImGui::SameLine();
    drawToolButton("StaticBody", ToolType::StaticBody);
    ImGui::SameLine();
    drawToolButton("DynamicBody", ToolType::DynamicBody);

    ImGui::End();
}


void BottomBar::drawToolButton(const char* label, ToolType tool) {
    bool isSelected = (activeTool_ == tool);

    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Button, toImVec4(Config::Visual::ACTIVE_BUTTON_COLOR));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, toImVec4(Config::Visual::ACTIVE_BUTTON_HOVER_COLOR));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, toImVec4(Config::Visual::ACTIVE_BUTTON_CLICK_COLOR));
    }

    if (ImGui::Button(label, ImVec2(Config::Visual::BUTTON_WIDTH, Config::Visual::BUTTON_HEIGHT))) {
        if (activeTool_ == tool) {
            showToolSettings_ = !showToolSettings_;
        } else {
            activeTool_ = tool;
        }
    }

    if (isSelected) {
        ImGui::PopStyleColor(3);
    }
}


void BottomBar::drawToolSettings() {
    if (!showToolSettings_) return;

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

    switch (activeTool_) {
    case ToolType::Molecules:
        if (ImGui::Begin("Molecules Settings", &showToolSettings_, flags)) {
        ImGui::Combo("Shape", &molSettings_.currentShape, molSettings_.shapes, IM_ARRAYSIZE(molSettings_.shapes));

        ImGui::Separator();

        ImGui::SliderFloat("Concentration", &molSettings_.concentration, 0.0f, 1.0f, "%.2f");

        ImGui::Text("Speed Range");
        ImGui::DragFloatRange2("##speed", &molSettings_.minSpeed, &molSettings_.maxSpeed, 1.0f, 0.0f, 1000.0f, "Min: %.1f", "Max: %.1f");
        
        ImGui::Separator();

        ImGui::SliderInt("Mass", &molSettings_.mass, Config::Physics::MIN_MOLECULE_MASS, Config::Physics::MAX_MOLECULE_MASS);
        ImGui::SliderInt("Radius", &molSettings_.radius, Config::Physics::MIN_MOLECULE_RADIUS, Config::Physics::MAX_MOLECULE_RADIUS);
        }
        break;
    case ToolType::StaticBody:
        if (ImGui::Begin("StaticBody Settings", &showToolSettings_, flags)) {
            ImGui::Combo("Shape", &staticBodySettings_.currentShape, staticBodySettings_.shapes, IM_ARRAYSIZE(staticBodySettings_.shapes));
            if (staticBodySettings_.currentShape == 1) {
                ImGui::SliderFloat("Thickness", &staticBodySettings_.thickness, Config::Physics::MIN_BOX_THICKNESS, 
                                    Config::Physics::MAX_BOX_THICNESS, "%.1f");
            }
        }
        break;
    case ToolType::DynamicBody:
        (ImGui::Begin("DynamicBody Settings", &showToolSettings_, flags));
        break;
    }

    ImGui::End();
}

}
}