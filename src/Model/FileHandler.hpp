#pragma once

#include "Model/Engine.hpp"
#include "Model/Molecule.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <json.hpp>

using json = nlohmann::json;


namespace Model {

// class FileHandler {
// public:
//     static bool saveScene(const Engine& engine, const std::string& filename);
//     static bool loadScene(Engine& engine, const std::string& filemame);
    
//     static std::vector<std::string> getFilesInFolder(const std::string& path);
// };
// }


// namespace sf {
// // --- VECTOR 2f ---
// inline void to_json(json& j, const sf::Vector2f& v) {
//     j = json{
//         {"x", v.x}, 
//         {"y", v.y}
//     };
// }

// inline void from_json(const json& j, Vector2f& v) {
//     j.at("x").get_to(v.x);
//     j.at("y").get_to(v.y);
// }

// // --- FLOAT RECT ---
// inline void to_json(json& j, const FloatRect& r) {
//     j = json{
//         {"left", r.position.x}, 
//         {"top", r.position.y}, 
//         {"width", r.size.x}, 
//         {"height", r.size.y}
//     };
// }

// inline void from_json(const json& j, FloatRect& r) {
//     j.at("left").get_to(r.position.x);
//     j.at("top").get_to(r.position.y);
//     j.at("width").get_to(r.size.x);
//     j.at("height").get_to(r.size.y);
// }
// }


// namespace Model {
// // --- MOLECULE ---
// inline void to_json(json& j, const Molecule& m) {
//     j = json{
//         {"position", m.position}, 
//         {"velocity", m.velocity}, 
//         {"mass", m.mass}, 
//         {"radius", m.radius}
//     };
// }

// inline void from_json(const json& j, Molecule& m) {
//     j.at("position").get_to(m.position);
//     j.at("velocity").get_to(m.velocity);
//     j.at("mass").get_to(m.mass);
//     j.at("radius").get_to(m.radius);
// }

// // --- DYNAMIC BODY ---
// inline void to_json(json& j, const DynamicBody& b) {
//     j = json{
//         {"size", b.size},
//         {"position", b.position},
//         {"mass", b.mass},
//         {"angle", b.angle},
//         {"velocity", b.velocity}
//     };
// }

// inline void from_json(const json& j, DynamicBody& b) {
//     j.at("size").get_to(b.size);
//     j.at("position").get_to(b.position);
//     j.at("mass").get_to(b.mass);
//     j.at("angle").get_to(b.angle);
//     j.at("velocity").get_to(b.velocity);
// }
}