#pragma once

#include "Model/Molecule.hpp"
#include "Model/Body.hpp"
#include "Utils/Utils.hpp"
#include <SFML/Graphics/Rect.hpp>


namespace Model::Geo {

inline bool isColliding(const Body& bodyA, const Body& bodyB) {
    sf::Vector2f axes[4] = {
        bodyA.vertices[1] - bodyA.vertices[0],
        bodyA.vertices[3] - bodyA.vertices[0],
        bodyB.vertices[1] - bodyB.vertices[0],
        bodyB.vertices[3] - bodyB.vertices[0]
    };

    for (int i = 0; i < 4; ++i) {
        const sf::Vector2f& axis = axes[i];

        if (axis.x == 0 && axis.y == 0) continue;

        float minA, maxA, minB, maxB;
        Utils::project(bodyA.vertices.data(), axis, minA, maxA);
        Utils::project(bodyB.vertices.data(), axis, minB, maxB);

        if (maxA < minB || maxB < minA) {
            return false; 
        }
    }
    return true;
}


inline bool isColliding(const Body& body, const Molecule& mol) {
    // 1. Вектор от центра тела к центру молекулы
    float dx = mol.position.x - body.position.x;
    float dy = mol.position.y - body.position.y;

    // 2. Поворот вектора в локальную систему координат тела
    float cosA = std::cos(body.angle);
    float sinA = std::sin(body.angle);
    float localX = dx * cosA + dy * sinA;
    float localY = -dx * sinA + dy * cosA;

    float halfW = body.size.x / 2.0f;
    float halfH = body.size.y / 2.0f;

    // 3. Поиск ближайшей точки на локальном (неповернутом) прямоугольнике
    float closestLocalX = std::clamp(localX, -halfW, halfW);
    float closestLocalY = std::clamp(localY, -halfH, halfH);

    // 4. Вектор от ближайшей точки к центру молекулы (в локальных координатах)
    float distX = localX - closestLocalX;
    float distY = localY - closestLocalY;

    // 5. Проверка квадрата расстояния
    return (distX * distX + distY * distY) < (mol.radius * mol.radius);
}


inline bool isColliding(const Molecule& molA, const Molecule& molB) {
    sf::Vector2f delta = molA.position - molB.position;
    float deltaPow2 = delta.x * delta.x + delta.y * delta.y;
    float minDist = molA.radius + molB.radius;

    return deltaPow2 < minDist * minDist;
}

}