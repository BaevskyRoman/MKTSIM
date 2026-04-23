#include "Model/FileHandler.hpp"
#include <fstream>


namespace Model {

bool FileHandler::saveScene(const Engine& engine, const std::string& filepath) {
    json j;
    j["enableGravity"] = engine.enableGravity;
    j["gravityAcceleration"] = engine.gravityAcceleration;
    j["molecules"] = engine.getMolecules();
    j["staticBodies"] = engine.getStaticBodies();
    j["dynamicBodies"] = engine.getDynamicBodies();

    std::ofstream file(filepath);
    if (!file.is_open()) return false;

    file << j.dump(4);
    return true;
}

bool FileHandler::loadScene(Engine& engine, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    json j;
    try {
        file >> j;
        
        engine.enableGravity = j.value("enableGravity", false);
        engine.gravityAcceleration = j.value("gravityAcceleration", 0.0f);

        engine.molecules_.clear();
        engine.staticBodies_.clear();
        engine.dynamicBodies_.clear();

        if (j.contains("molecules")) {
            engine.molecules_ = j["molecules"].get<std::vector<Molecule>>();
        }
        if (j.contains("staticBodies")) {
            engine.staticBodies_ = j["staticBodies"].get<std::vector<sf::FloatRect>>();
        }
        if (j.contains("dynamicBodies")) {
            engine.dynamicBodies_.clear();
            
            for (const auto& item : j["dynamicBodies"]) {
                sf::Vector2f sz = item.at("size").get<sf::Vector2f>();
                sf::Vector2f pos = item.at("position").get<sf::Vector2f>();
                float m = item.at("mass").get<float>();

                DynamicBody body(sz, pos, m);

                if (item.contains("velocity")) body.velocity = item.at("velocity").get<sf::Vector2f>();
                if (item.contains("angle")) body.angle = item.at("angle").get<float>();
                if (item.contains("angularVelocity")) body.angularVelocity = item.at("angularVelocity").get<float>();

                engine.dynamicBodies_.push_back(body);
            }
        }
        
        return true;
    } catch (json::exception& e) {
        return false;
    }
}
}