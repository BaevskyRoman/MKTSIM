#include "View/UI/TopBar.hpp"
#include "Config/VisualConfig.hpp"
#include <iostream>


namespace View {
namespace UI {

void TopBar::update(sf::RenderWindow& window, sf::Time dt) {
    drawTopBar(window);
    drawSimSettings(window);
    drawSimStats(window, dt);
    drawFileManager(window);
}


void TopBar::drawTopBar(sf::RenderWindow& window) {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window.getSize().x), Config::Visual::TOP_BAR_HEIGHT));

    ImGuiWindowFlags windowFlags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoScrollWithMouse | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("TopBar", nullptr, windowFlags);
    if (ImGui::Button("SimSettings##B", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) showSimSettings_ = !showSimSettings_;
    ImGui::SameLine();
    if (ImGui::Button("SimStats##B", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) showSimStats_ = !showSimStats_;
    ImGui::SameLine();
    if (ImGui::Button("SimFileManager##B", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) showFileManager_ = !showFileManager_;
    ImGui::End();
}


void TopBar::drawSimSettings(sf::RenderWindow& window) {
    if (!showSimSettings_) return;

    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    if (ImGui::Begin("Sim Settings", &showSimSettings_)) {
        ImGui::Checkbox("Gravity", &engineRef_.enableGravity);

        ImGui::BeginDisabled(!engineRef_.enableGravity);
        ImGui::SliderFloat("##g", &engineRef_.gravityAcceleration, -100.0f, 100.0f, "%.2f");

        ImGui::EndDisabled();
    }

    ImGui::End();
}


void TopBar::drawSimStats(sf::RenderWindow& window, sf::Time dt) {
    if (!showSimStats_) return;
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    if (ImGui::Begin("Sim Stats", &showSimStats_)) {
        ImGui::LabelText("Molecules", "%zu J", engineRef_.getMolecules().size());
        ImGui::LabelText("Molecules", "%f J", 1 / dt.asSeconds());
    }

    ImGui::End();
}


void TopBar::drawFileManager(sf::RenderWindow& window) {
    if (!showFileManager_) return;
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    if (ImGui::Begin("File Manager", &showFileManager_)) {
        if (ImGui::Button("Save", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) {
            fileHandler_.saveScene(engineRef_, "save.json");
        }

        if (ImGui::Button("Load", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) {
            fileHandler_.loadScene(engineRef_, "save.json");
        }
    }

    ImGui::End();
}

}
}