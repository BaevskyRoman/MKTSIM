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

    drawToolButton("Molecules##Bbutton", ToolType::Molecules);
    ImGui::SameLine();
    drawToolButton("StaticBody##Bbutton", ToolType::StaticBody);
    ImGui::SameLine();
    drawToolButton("DynamicBody##Bbutton", ToolType::DynamicBody);

    ImGui::End();
}


void BottomBar::drawToolButton(const char* label, ToolType tool) {
    bool isSelected = (settings.activeTool == tool);

    if (isSelected) {
        ImGui::PushStyleColor(ImGuiCol_Button, Utils::toImVec4(Config::Visual::ACTIVE_BUTTON_COLOR));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Utils::toImVec4(Config::Visual::ACTIVE_BUTTON_HOVER_COLOR));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, Utils::toImVec4(Config::Visual::ACTIVE_BUTTON_CLICK_COLOR));
    }

    if (ImGui::Button(label, ImVec2(Config::Visual::BUTTON_WIDTH, Config::Visual::BUTTON_HEIGHT))) {
        if (settings.activeTool == tool) {
            showToolSettings_ = !showToolSettings_;
        } else {
            settings.activeTool = tool;
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

    switch (settings.activeTool) {
    case ToolType::Molecules:

        // --- SHAPE ---
        makeRadioSwitch("Shape", settings.mol.shapes, settings.mol.currentShape);
        ImGui::Separator();

        // --- QTY MODE ---
        if (ImGui::BeginTable("mol/table/qty", 2, ImGuiTableFlags_SizingStretchProp)) {
            // CONCENTRATION ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(settings.mol.qtyModes[0], &settings.mol.currentQTYMode, 0);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(settings.mol.currentQTYMode != 0);
            ImGui::SliderFloat("##mol/concentration", &settings.mol.concentration, 0.0f, 1.0f, "%.2f");
            ImGui::EndDisabled();

            // QTY ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(settings.mol.qtyModes[1], &settings.mol.currentQTYMode, 1);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(settings.mol.currentQTYMode != 1);
            ImGui::InputInt("##mol/qty", &settings.mol.qty);
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
        ImGui::Separator();

        // --- SPEED ---
        ImGui::Text("Speed Range");
        ImGui::DragFloatRange2("##mol/speed", &settings.mol.minSpeed, &settings.mol.maxSpeed, 1.0f, 0.0f, 1000.0f, "Min: %.1f", "Max: %.1f");
        ImGui::Separator();

        // --- RADIUS ---
        if (ImGui::SliderFloat("Radius##mol", &settings.mol.radius, 1, 10)) {
            if (strcmp(settings.mol.massModes[settings.mol.currentMassMode], "Mass")) {
                settings.mol.density = settings.mol.mass / (settings.mol.radius * settings.mol.radius * Utils::PI);
            } else if (strcmp(settings.mol.massModes[settings.mol.currentMassMode], "Density")) {
                settings.mol.mass = settings.mol.radius * settings.mol.radius * Utils::PI * settings.mol.density;
            }
        }
        ImGui::Separator();

        // --- MASS ---
        if (ImGui::BeginTable("mol/table/mass", 2, ImGuiTableFlags_SizingStretchProp)) {
            // MASS ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(settings.mol.massModes[0], &settings.mol.currentMassMode, 0);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(settings.mol.currentMassMode != 0);
            if (ImGui::SliderFloat("##mol/mass", &settings.mol.mass, 0.0f, 1.0f, "%.2f")) {
                settings.mol.density = settings.mol.mass / (settings.mol.radius * settings.mol.radius * Utils::PI);
            }
            ImGui::EndDisabled();

            // DENSITY
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(settings.mol.massModes[1], &settings.mol.currentMassMode, 1);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::BeginDisabled(settings.mol.currentMassMode != 1);
            if (ImGui::SliderFloat("##mol/density", &settings.mol.density, 0.0f, 1.0f, "%.2f")) {
                settings.mol.mass = settings.mol.radius * settings.mol.radius * Utils::PI * settings.mol.density;
            }
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
        
        break;
    case ToolType::StaticBody:
        // --- SHAPE ---
        makeRadioSwitch("Shape", settings.bodyS.shapes, settings.bodyS.currentShape);

        // --- THICKNESS ---
        ImGui::BeginDisabled(settings.bodyS.currentShape != 0);
        ImGui::SliderFloat("Thickness", &settings.bodyS.thickness, 10, 100, "%.1f");
        ImGui::EndDisabled();
        
        break;
    case ToolType::DynamicBody:
        if (ImGui::BeginTable("bodyd/table/mass", 2, ImGuiTableFlags_SizingStretchProp)) {
            // DENSUTY ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(settings.bodyD.massModes[0], &settings.bodyD.currentMassMode, 0);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(200);
            ImGui::BeginDisabled(settings.bodyD.currentMassMode != 0);
            ImGui::SliderFloat("##bodyd/density", &settings.bodyD.density, 0.0f, 1.0f, "%.2f");
            ImGui::EndDisabled();

            // MASS ROW
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::RadioButton(settings.bodyD.massModes[1], &settings.bodyD.currentMassMode, 1);
            
            ImGui::TableNextColumn();
            ImGui::SetNextItemWidth(200);
            ImGui::BeginDisabled(settings.bodyD.currentMassMode != 1);
            ImGui::SliderFloat("##bodyd/mass", &settings.bodyD.mass, 0.0f, 1.0f, "%.2f");
            ImGui::EndDisabled();

            ImGui::EndTable();
        }
        break;
    }

    ImGui::End();
}

}
}