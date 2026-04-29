#pragma once

#include "Model/Engine.hpp"
#include "FileHandler.hpp"
#include <fstream>
#include <vector>
#include <string>


namespace Model {

class SimulationRecorder {
public:
    bool startRecording(const Engine& engine, const std::string& filename);
    void recordFrame(const Engine& engine);
    void stopRecording();

    bool startPlayback(Engine& engine, const std::string& filename);
    bool playNextFrame(Engine& engine);
    void stopPlayback();

    ~SimulationRecorder();

private:
    std::fstream file_;
    FileHandler fileHandler;
    
    struct BodyDState {
        sf::Vector2f position;
        float angle;
    };
};

}