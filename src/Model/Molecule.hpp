#pragma once

#include <SFML/System/Vector2.hpp>
#include "Config/PhysicsConfig.hpp"

namespace Model {

struct Molecule {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass = Config::Physics::DEFAULT_MOLECULE_MASS;
    float radius = Config::Physics::DEFAULT_MOLECULE_RADIUS;

    void move(float deltaTime) {
        position += velocity * deltaTime;
    }
};

}