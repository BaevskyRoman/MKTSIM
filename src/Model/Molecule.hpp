#pragma once

#include <SFML/System/Vector2.hpp>
#include "Config/PhysicsConfig.hpp"

namespace Model {

struct Molecule {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    float radius;

    void move(float deltaTime) {
        position += velocity * deltaTime;
    }
};

}