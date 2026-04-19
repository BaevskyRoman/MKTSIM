#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Config::Physics {

    constexpr float MIN_MOLECULE_MASS = 1.0f;
    constexpr float MAX_MOLECULE_MASS = 10.0f;

    constexpr float MIN_MOLECULE_RADIUS = 1.0f;
    constexpr float MAX_MOLECULE_RADIUS = 10.0f;

    constexpr float MIN_BOX_THICKNESS = 10.f;
    constexpr float MAX_BOX_THICNESS = 100.f;

    constexpr bool GRAVITY = true;
    constexpr float GRAVITY_ACCELERATION = 50.f;
}