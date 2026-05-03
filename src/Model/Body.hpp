#pragma once

#include <SFML/System/Vector2.hpp>
#include <array>
#include <cmath>


namespace Model {

struct Body {
    sf::Vector2f size;
    sf::Vector2f position;
    float angle;
    
    std::array<sf::Vector2f, 4> vertices;

    void updateVertices() {
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        sf::Vector2f halfSize = size / 2.0f;

        sf::Vector2f dx(cosA * halfSize.x, sinA * halfSize.x);
        sf::Vector2f dy(-sinA * halfSize.y, cosA * halfSize.y);

        vertices[0] = position - dx - dy;
        vertices[1] = position + dx - dy;
        vertices[2] = position + dx + dy;
        vertices[3] = position - dx + dy;
    }
};


struct StaticBody : public Body {
    StaticBody(sf::Vector2f sz, sf::Vector2f pos, float a = 0.f) {
        size = sz;
        position = pos;
        angle = a;
        updateVertices();
    }
};


struct DynamicBody : public Body {
    sf::Vector2f velocity;
    float angularVelocity;

    float mass;
    float inertia;

    DynamicBody(sf::Vector2f sz, float m, sf::Vector2f pos, float a = 0.f) {
        size = sz;
        mass = m;
        inertia = (mass / 12.0f) * (size.x * size.x + size.y * size.y);
        
        position = pos;
        angle = a;

        velocity = {0.f, 0.f};
        angularVelocity = 0.f;

        updateVertices();
    }

    void move(float deltaTime) {
        position += velocity * deltaTime;
        angle += angularVelocity * deltaTime;
        
        updateVertices(); 
    }
};

}