#pragma once

#include <cmath>
#include <random>

namespace Utils::Math {

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


class Random {
public:
    Random() = delete;

    static float getFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(generator_);
    }

    static int getInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(generator_);
    }

private:
    inline static std::mt19937 generator_{std::random_device{}()};
};

}