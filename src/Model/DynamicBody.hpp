#pragma once

#include <SFML/System/Vector2.hpp>

namespace Model {

struct DynamicBody {
    sf::Vector2f size;
    sf::Vector2f position;
    sf::Vector2f velocity;
    float mass;
    float inertia;

    float angle;
    float angularVelocity;

    
    DynamicBody(sf::Vector2f sz, sf::Vector2f pos, float m) 
        : size(sz), position(pos), velocity(0.f, 0.f), mass(m), angle(0.f), angularVelocity(0.f)
    {
        inertia = (mass / 12.0f) * (size.x * size.x + size.y * size.y);
    }

    void move(float deltaTime) {
        position += velocity * deltaTime;
        angle += angularVelocity * deltaTime;
    }
};

}