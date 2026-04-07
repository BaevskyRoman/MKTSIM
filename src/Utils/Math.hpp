#pragma once

#include <cmath>

namespace Utils::Math {

    constexpr float EPSILON = 0.0001f;

    inline bool isEqual(float a, float b) {
        return std::abs(a - b) < EPSILON;
    }

    inline bool isZero(float a) {
        return std::abs(a) < EPSILON;
    }

}