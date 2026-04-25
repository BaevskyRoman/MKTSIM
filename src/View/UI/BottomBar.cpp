#include "View/UI/BottomBar.hpp"
#include "Config/VisualConfig.hpp"
#include <Utils/Utils.hpp>
#include <cstring>


namespace View {
namespace UI {

void BottomBar::update(sf::RenderWindow& window) {
    drawBottomBar(window);
    drawToolSettings();
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

    float verticalPadding = (ImGui::GetWindowHeight() - Config::Visual::BUTTON_HEIGHT) * 0.5f;
    ImGui::SetCursorPosY(verticalPadding);

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
        ImGui::PushStyleColor(ImGuiCol_Button, Utils::toImVec4(Config::Visual::ACTIVE_BUTTON_COLOR));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Utils::toImVec4(Config::Visual::ACTIVE_BUTTON_HOVER_COLOR));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Utils::toImVec4(Config::Visual::ACTIVE_BUTTON_CLICK_COLOR));
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
    if (!ImGui::Begin("Tool Settings", &showToolSettings_, flags)) {
        ImGui::End();
        return;
    }

    switch (activeTool_) {
    case ToolType::Molecules:

        // --- SHAPE ---
        ImGui::Text("Shape");
        ImGui::RadioButton(molSettings_.shapes[0], &molSettings_.currentShape, 0);
        ImGui::SameLine();
        ImGui::RadioButton(molSettings_.shapes[1], &molSettings_.currentShape, 1);
        ImGui::SameLine();
        ImGui::RadioButton(molSettings_.shapes[2], &molSettings_.currentShape, 2);
        ImGui::Separator();

        // --- QTY MODE ---
        if (ImGui::BeginTable("Settings Table", 2, ImGuiTableFlags_SizingStretchProp)) {
            // CONCENTRATION ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(molSettings_.qtyModes[0], &molSettings_.currentQTYMode, 0);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(molSettings_.currentQTYMode != 0);
            ImGui::SliderFloat("##concentration", &molSettings_.concentration, 0.0f, 1.0f, "%.2f");
            ImGui::EndDisabled();

            // QTY ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(molSettings_.qtyModes[1], &molSettings_.currentQTYMode, 1);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(molSettings_.currentQTYMode != 1);
            ImGui::InputInt("##qty", &molSettings_.qty);
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
        ImGui::Separator();

        // --- SPEED ---
        ImGui::Text("Speed Range");
        ImGui::DragFloatRange2("##speed", &molSettings_.minSpeed, &molSettings_.maxSpeed, 1.0f, 0.0f, 1000.0f, "Min: %.1f", "Max: %.1f");
        ImGui::Separator();

        // --- RADIUS ---
        if (ImGui::SliderFloat("Radius", &molSettings_.radius, 1, 10)) {
            if (strcmp(molSettings_.massModes[molSettings_.currentMassMode], "Mass")) {
                molSettings_.density = molSettings_.mass / (molSettings_.radius * molSettings_.radius * Utils::PI);
            } else if (strcmp(molSettings_.massModes[molSettings_.currentMassMode], "Density")) {
                molSettings_.mass = molSettings_.radius * molSettings_.radius * Utils::PI * molSettings_.density;
            }
        }
        ImGui::Separator();

        // --- MASS ---
        if (ImGui::BeginTable("Settings Table2", 2, ImGuiTableFlags_SizingStretchProp)) {
            // MASS ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(molSettings_.massModes[0], &molSettings_.currentMassMode, 0);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(molSettings_.currentMassMode != 0);
            if (ImGui::SliderFloat("##mass", &molSettings_.mass, 0.0f, 1.0f, "%.2f")) {
                molSettings_.density = molSettings_.mass / (molSettings_.radius * molSettings_.radius * Utils::PI);
            }
            ImGui::EndDisabled();

            // DENSITY
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(molSettings_.massModes[1], &molSettings_.currentMassMode, 1);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(molSettings_.currentMassMode != 1);
            if (ImGui::SliderFloat("##ma7ss", &molSettings_.density, 0.0f, 1.0f, "%.2f")) {
                molSettings_.mass = molSettings_.radius * molSettings_.radius * Utils::PI * molSettings_.density;
            }
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
        
        break;
    case ToolType::StaticBody:
        // --- SHAPE ---
        ImGui::RadioButton(staticBodySettings_.shapes[0], &staticBodySettings_.currentShape, 0);
        ImGui::SameLine();
        ImGui::RadioButton(staticBodySettings_.shapes[1], &staticBodySettings_.currentShape, 1);

        // --- THICKNESS ---
        ImGui::BeginDisabled(staticBodySettings_.currentShape != 0);
        ImGui::SliderFloat("Thickness", &staticBodySettings_.thickness, 10, 100, "%.1f");
        ImGui::EndDisabled();
        
        break;
    case ToolType::DynamicBody:
        if (ImGui::BeginTable("Settings Table22", 2, ImGuiTableFlags_SizingStretchProp)) {
            // DENSUTY ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(dynamicBodySettings_.massModes[0], &dynamicBodySettings_.currentMassMode, 0);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(200);
            ImGui::BeginDisabled(dynamicBodySettings_.currentMassMode != 0);
            ImGui::SliderFloat("##mass", &dynamicBodySettings_.density, 0.0f, 1.0f, "%.2f");
            ImGui::EndDisabled();

            // MASS ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(dynamicBodySettings_.massModes[1], &dynamicBodySettings_.currentMassMode, 1);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(200);
            ImGui::BeginDisabled(dynamicBodySettings_.currentMassMode != 1);
            ImGui::SliderFloat("##mas23s", &dynamicBodySettings_.mass, 0.0f, 1.0f, "%.2f");
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
        break;
    }

    ImGui::End();
}

}
}