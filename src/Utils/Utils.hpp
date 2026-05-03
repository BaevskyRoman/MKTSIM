#pragma once

#include <Model/DynamicBody.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>
#include <imgui.h>
#include <filesystem>

namespace Utils {

    constexpr float EPSILON = 0.0001f;
    constexpr float PI = 3.14159265f;

    inline bool isEqual(float a, float b) {
        return std::abs(a - b) < EPSILON;
    }

    inline bool isZero(float a) {
        return std::abs(a) < EPSILON;
    }

    inline float cross(const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.y - a.y * b.x;
    }

    inline sf::Vector2f cross(float s, const sf::Vector2f& a) {
        return sf::Vector2f(-s * a.y, s * a.x);
    }

    inline float dot(const sf::Vector2f& a, const sf::Vector2f& b) {
        return a.x * b.x + a.y * b.y;
    }

    inline ImVec4 toImVec4(const sf::Color& color) {
        return ImVec4(
            static_cast<float>(color.r) / 255.0f,
            static_cast<float>(color.g) / 255.0f,
            static_cast<float>(color.b) / 255.0f,
            static_cast<float>(color.a) / 255.0f
        );
    }
    
    inline void getVertices(const Model::DynamicBody& body, sf::Vector2f out[4]) {
        float hw = body.size.x / 2.0f;
        float hh = body.size.y / 2.0f;
        float cosA = std::cos(body.angle);
        float sinA = std::sin(body.angle);
        
        sf::Vector2f corners[4] = {{-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};
        for (int i = 0; i < 4; ++i) {
            out[i] = sf::Vector2f(
                corners[i].x * cosA - corners[i].y * sinA + body.position.x,
                corners[i].x * sinA + corners[i].y * cosA + body.position.y
            );
        }
    }


    inline void getVertices(const sf::FloatRect& rect, sf::Vector2f out[4]) {
        out[0] = sf::Vector2f(rect.position.x, rect.position.y);
        out[1] = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y);
        out[2] = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
        out[3] = sf::Vector2f(rect.position.x, rect.position.y + rect.size.y);
    }


    inline void project(const sf::Vector2f vertices[4], const sf::Vector2f& axis, float& min, float& max) {
        min = max = Utils::dot(vertices[0], axis);
        for (int i = 1; i < 4; ++i) {
            float proj = Utils::dot(vertices[i], axis);
            if (proj < min) min = proj;
            if (proj > max) max = proj;
        }
    }

class Random {
public:
    Random() = delete;

    static float getFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator_);
    }

    template <typename T>
        static void shuffle(std::vector<T>& vec) {
            std::shuffle(vec.begin(), vec.end(), generator_);
        }
        
private:
    inline static std::mt19937 generator_{std::random_device{}()};
};
}


namespace View {
namespace UI {
    inline void makeRadioSwitch(const char *name, const char **options, int& current) {
        ImGui::Text("%s", name);
        for (int i = 0; options[i] != nullptr; i++) {
            if (i != 0) ImGui::SameLine();
            ImGui::RadioButton(options[i], &current, i);
        }
    }
}
}