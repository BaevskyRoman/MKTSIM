#pragma once

#include "Model/Molecule.hpp"
#include "Model/DynamicBody.hpp"
#include "Utils/Utils.hpp"
#include <SFML/Graphics/Rect.hpp>


namespace Model::Geo {

inline bool isColliding(const sf::FloatRect& sBody, const DynamicBody& dBody) {
    sf::Vector2f dynVerts[4];
    Utils::getVertices(dBody, dynVerts);
    
    sf::Vector2f statVerts[4];
    Utils::getVertices(sBody, statVerts);
    
    float cosA = std::cos(dBody.angle);
    float sinA = std::sin(dBody.angle);
    sf::Vector2f axes[4] = {
        {1.0f, 0.0f}, {0.0f, 1.0f},
        {cosA, sinA}, {-sinA, cosA}
    };
    
    bool out = true;
    
    for (int i = 0; i < 4; ++i) {
        float min1, max1, min2, max2;
        Utils::project(dynVerts, axes[i], min1, max1);
        Utils::project(statVerts, axes[i], min2, max2);
        
        if (max1 < min2 || max2 < min1) {
            out = false; 
            break;
        }
    }

    return out;
}


inline bool isColliding(const DynamicBody& bodyA, const DynamicBody& bodyB) {
    sf::Vector2f vertsA[4];
    Utils::getVertices(bodyA, vertsA);
    
    sf::Vector2f vertsB[4];
    Utils::getVertices(bodyB, vertsB);

    // Получаем 4 оси для проверки (2 от тела A, 2 от тела B)
    float cosA = std::cos(bodyA.angle);
    float sinA = std::sin(bodyA.angle);
    float cosB = std::cos(bodyB.angle);
    float sinB = std::sin(bodyB.angle);
    
    sf::Vector2f axes[4] = {
        {cosA, sinA}, {-sinA, cosA}, // Локальные оси тела A
        {cosB, sinB}, {-sinB, cosB}  // Локальные оси тела B
    };

    bool out = true;

    // 1. Поиск пересечений по алгоритму SAT
    for (int k = 0; k < 4; ++k) {
        float min1, max1, min2, max2;
        Utils::project(vertsA, axes[k], min1, max1);
        Utils::project(vertsB, axes[k], min2, max2);

        if (max1 < min2 || max2 < min1) {
            out = false; // Нашли разделяющую ось, столкновения нет
            break;
        }
    }

    return out;
}


inline bool isColliding(const sf::FloatRect& body, const Molecule& mol) {
    float closestX = std::clamp(mol.position.x, body.position.x, body.position.x + body.size.x);
    float closestY = std::clamp(mol.position.y, body.position.y, body.position.y + body.size.y);

    float deltaX = mol.position.x - closestX;
    float deltaY = mol.position.y - closestY;

    float distancePow2 = deltaX * deltaX + deltaY * deltaY;
    return (distancePow2 < mol.radius * mol.radius);
}


inline bool isColliding(const DynamicBody& body, const Molecule& mol) {
    float dx = mol.position.x - body.position.x;
    float dy = mol.position.y - body.position.y;

    float cosA = std::cos(body.angle);
    float sinA = std::sin(body.angle);

    float localX = dx * cosA + dy * sinA;
    float localY = -dx * sinA + dy * cosA;

    float halfW = body.size.x / 2.0f;
    float halfH = body.size.y / 2.0f;

    float closestLocalX = std::clamp(localX, -halfW, halfW);
    float closestLocalY = std::clamp(localY, -halfH, halfH);

    float distX = localX - closestLocalX;
    float distY = localY - closestLocalY;
    float distancePow2 = distX * distX + distY * distY;

    return (distancePow2 < mol.radius * mol.radius);
}


inline bool isColliding(const Molecule& molA, const Molecule& molB) {
    sf::Vector2f delta = molA.position - molB.position;
    float deltaPow2 = delta.x * delta.x + delta.y * delta.y;
    float minDist = molA.radius + molB.radius;

    return deltaPow2 < minDist * minDist;
}

}