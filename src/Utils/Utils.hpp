#pragma once


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