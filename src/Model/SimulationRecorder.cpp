#include "SimulationRecorder.hpp"
#include <iostream>


namespace Model {

bool SimulationRecorder::startRecording(const Engine& engine, const std::string& filename) {
    // fileHandler.saveScene(engine, "_" + filename);
    file_.open("bake/" + filename + ".bin", std::ios::out | std::ios::binary);
    return file_.is_open();
}


void SimulationRecorder::recordFrame(const Engine& engine) {
    std::cout << engine.molecules_[0].position.x << std::endl;
    if (!file_.is_open()) return;
    const auto& molecules = engine.getMolecules();
    size_t molCount = molecules.size();
    file_.write(reinterpret_cast<const char*>(&molCount), sizeof(size_t));
    for (const auto& mol : molecules) {
        file_.write(reinterpret_cast<const char*>(&mol.position), sizeof(sf::Vector2f));
    }

    const auto& bodiesD = engine.getDynamicBodies();
    size_t bodiesDcount = bodiesD.size();
    file_.write(reinterpret_cast<const char*>(&bodiesDcount), sizeof(size_t));
    for (const auto& body : bodiesD) {
        BodyDState state{body.position, body.angle};
        file_.write(reinterpret_cast<const char*>(&state), sizeof(BodyDState));
    }
}


void SimulationRecorder::stopRecording() {
    if (file_.is_open()) file_.close();
}


bool SimulationRecorder::startPlayback(Engine& engine, const std::string& filename) {
    // fileHandler.loadScene(engine, "_" + filename);
    file_.open("bake/" + filename + ".bin", std::ios::in | std::ios::binary);
    return file_.is_open();
}


bool SimulationRecorder::playNextFrame(Engine& engine) {
    if (!file_.is_open() || file_.peek() == EOF) return false;
    // 1. Читаем количество молекул
    size_t molCount = 0;
    file_.read(reinterpret_cast<char*>(&molCount), sizeof(size_t));
    
    if (file_.gcount() < sizeof(size_t)) return false; // Недочитали заголовок

    // Проверка: соответствует ли сцена количеству данных в кадре
    // Если в векторе engine.molecules_ другое кол-во, будет краш или баг
    auto& molecules = const_cast<std::vector<Molecule>&>(engine.getMolecules()); 

    for (size_t i = 0; i < molCount; ++i) {
        // Читаем только позицию (как мы договаривались для экономии места)
        sf::Vector2f pos;
        file_.read(reinterpret_cast<char*>(&pos), sizeof(sf::Vector2f));
        if (i < molecules.size()) {
            molecules[i].position = pos;
        }
    }

    // 2. Читаем динамические тела
    size_t dynCount = 0;
    file_.read(reinterpret_cast<char*>(&dynCount), sizeof(size_t));
    
    auto& dynBodies = const_cast<std::vector<DynamicBody>&>(engine.getDynamicBodies());

    for (size_t i = 0; i < dynCount; ++i) {
        BodyDState state;
        file_.read(reinterpret_cast<char*>(&state), sizeof(BodyDState));
        if (i < dynBodies.size()) {
            dynBodies[i].position = state.position;
            dynBodies[i].angle = state.angle;
        }
    }

    return !file_.fail(); 
}


void SimulationRecorder::stopPlayback() {
    if (file_.is_open()) file_.close();
}


SimulationRecorder::~SimulationRecorder() {
    stopPlayback();
    stopRecording();
}

}