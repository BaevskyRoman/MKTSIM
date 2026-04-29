#include "View/UI/TopBar.hpp"
#include "Config/VisualConfig.hpp"
#include <iostream>
#include <filesystem>


namespace View {
namespace UI {

void TopBar::update(sf::RenderWindow& window, sf::Time dt) {
    drawTopBar(window, dt);
    drawSimSettings(window);
    drawFileManager(window);
    drawRecorder(window);
}


void TopBar::drawTopBar(sf::RenderWindow& window, sf::Time dt) {
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(window.getSize().x), Config::Visual::TOP_BAR_HEIGHT));

    ImGuiWindowFlags windowFlags = 
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoScrollWithMouse | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("TopBar", nullptr, windowFlags);
    if (ImGui::Button("SimSettings##T", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) showSimSettings_ = !showSimSettings_;
    ImGui::SameLine();
    if (ImGui::Button("SimFileManager##T", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) showFileManager_ = !showFileManager_;
    ImGui::SameLine();
    if (ImGui::Button("Recorder##T", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) showRecorder_ = !showRecorder_;
    ImGui::SameLine();
    ImGui::Text("Molecules: %zu", engineRef_.getMolecules().size());
    ImGui::SameLine();
    ImGui::Text("FPS: %f", 1 / dt.asSeconds());
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


void TopBar::drawFileManager(sf::RenderWindow& window) {
    if (!showFileManager_) return;
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    if (ImGui::Begin("File Manager", &showFileManager_)) {
        // --- SAVE ---
        static char saveFileName[32] = "";

        ImGui::InputText("##SaveFile", saveFileName, sizeof(saveFileName));
        ImGui::SameLine();
        if (ImGui::Button("Save", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) {
            fileHandler_.saveScene(engineRef_, saveFileName);
        }

        // -- LOAD ---
        static std::vector<std::string> fileList;
        static int selectedItem = -1;
        std::string preview = (selectedItem >= 0 && selectedItem < fileList.size()) 
                    ? fileList[selectedItem] 
                    : "Select a file";

        if (ImGui::BeginCombo("##load", preview.c_str())) {
            if (ImGui::IsWindowAppearing()) { 
                fileList = fileHandler_.getFilesInFolder("saves");
            }

            for (int i = 0; i < fileList.size(); i++) {
                const bool isSelected = (selectedItem == i);
                if (ImGui::Selectable(fileList[i].c_str(), isSelected)) {
                    selectedItem = i;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) {
            fileHandler_.loadScene(engineRef_, fileList[selectedItem]);
        }
    }

    ImGui::End();
}


void TopBar::drawRecorder(sf::RenderWindow& window) {
    if (!showRecorder_) return;

    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_Always);
    if (ImGui::Begin("Recorder", &showRecorder_)) {
        if (ImGui::Button("Record", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) {
            if (onBakeRequested) onBakeRequested();
        }
        if (ImGui::Button("Play", ImVec2(Config::Visual::BUTTON_WIDTH, 20))) {
            if (onPlaybackRequested) onPlaybackRequested();
        }
    }
    ImGui::End();
}

}
}