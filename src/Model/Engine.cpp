#include <cmath>
#include <algorithm>
#include "Model/Engine.hpp"
#include "Config/VisualConfig.hpp"
#include "Utils/Math.hpp"


namespace Model {

const std::vector<Molecule>& Engine::getMolecules() const { return molecules_; }
const std::vector<sf::FloatRect>& Engine::getStaticBodies() const { return staticBodies_; }
void Engine::addStaticBody(const sf::FloatRect& rect) { staticBodies_.push_back(rect); }


void Engine::update(float deltaTime) {
    // --- MOVE ---
    for (auto& mol : molecules_) {
        mol.move(deltaTime);
    }

    // --- MOL TO STATIC BODY ---
    for (auto& mol : molecules_) {
        for (const auto& rect : staticBodies_) {
            float closestX = std::clamp(mol.position.x, rect.position.x, rect.position.x + rect.size.x);
            float closestY = std::clamp(mol.position.y, rect.position.y, rect.position.y + rect.size.y);

            float deltaX = mol.position.x - closestX;
            float deltaY = mol.position.y - closestY;

            float distancePow2 = deltaX * deltaX + deltaY * deltaY;
            if (distancePow2 < mol.radius * mol.radius) {
                float distance = std::sqrt(distancePow2);
                sf::Vector2f normal;
                float penetration = 0.0f;

                if (distance > 0.0f) {
                    normal = sf::Vector2f(deltaX / distance, deltaY / distance);
                    penetration = mol.radius - distance;
                } else {
                    float distToLeft = mol.position.x - rect.position.x;
                    float distToRight = (rect.position.x + rect.size.x) - mol.position.x;
                    float distToTop = mol.position.y - rect.position.y;
                    float distToBottom = (rect.position.y + rect.size.y) - mol.position.y;

                    float minDist = std::min({distToLeft, distToRight, distToTop, distToBottom});

                    if (minDist == distToLeft) { normal = {-1.0f, 0.0f}; penetration = distToLeft + mol.radius; }
                    else if (minDist == distToRight) { normal = {1.0f, 0.0f}; penetration = distToRight + mol.radius; }
                    else if (minDist == distToTop) { normal = {0.0f, -1.0f}; penetration = distToTop + mol.radius; }
                    else { normal = {0.0f, 1.0f}; penetration = distToBottom + mol.radius; }
                }

                float velocityAlongNormal = mol.velocity.x * normal.x + mol.velocity.y * normal.y;
                
                if (velocityAlongNormal < 0) {
                    mol.velocity -= normal * (2.0f * velocityAlongNormal);
                }
            }
        }
    }

    // --- MOL TO MOL ---
    for (size_t i = 0; i < molecules_.size(); ++i) {
        for (size_t j = i + 1; j < molecules_.size(); ++j) {
            Molecule& m1 = molecules_[i];
            Molecule& m2 = molecules_[j];

            sf::Vector2f delta = m2.position - m1.position;
            
            float deltaPow2 = delta.x * delta.x + delta.y * delta.y;
            float minDist = m1.radius + m2.radius;

            if (deltaPow2 < minDist * minDist) {
                float distance = std::sqrt(deltaPow2);
                
                sf::Vector2f normal;
                float overlap;
                if (Utils::Math::isZero(distance)) {
                    float angle = Utils::Math::Random::getFloat(0.0f, 2.0f * Utils::Math::PI);
                    normal = {std::cos(angle), std::sin(angle)};
                    overlap = minDist;
                }
                else {
                    normal = delta / distance;
                    overlap = minDist - distance;
                }

                sf::Vector2f relativeVelocity = m1.velocity - m2.velocity;
                float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

                if (velocityAlongNormal < 0) {
                    continue;
                }

                float impulseScalar = (2.0f * velocityAlongNormal) / (m1.mass + m2.mass);
                m1.velocity -= normal * (impulseScalar * m2.mass);
                m2.velocity += normal * (impulseScalar * m1.mass);
            }
        }
    }
}

void Engine::spawnMoleculesInArea(const sf::FloatRect& area, float concentration, 
                        float min_speed, float max_speed, float mass, float radius) {
    float areaSize = (area.size.x - 2*radius) * (area.size.y - 2*radius);
    float maxMolecules = areaSize / (4.0f * radius * radius);
    int count = static_cast<int>(maxMolecules * concentration);
    
    for (int i = 0; i < count; ++i) {
        float px = Utils::Math::Random::getFloat(area.position.x + radius, area.position.x + area.size.x - radius);
        float py = Utils::Math::Random::getFloat(area.position.y + radius, area.position.y + area.size.y - radius);
        float angle = Utils::Math::Random::getFloat(0.0f, 2.0f * Utils::Math::PI);
        float speed = Utils::Math::Random::getFloat(min_speed, max_speed);
        
        float vx = std::cos(angle) * speed;
        float vy = std::sin(angle) * speed;

        molecules_.push_back({sf::Vector2f(px, py), sf::Vector2f(vx, vy), mass, radius});
    }
}

void Engine::spawnMolecule(float x, float y, float vx, float vy) {
    molecules_.push_back({sf::Vector2f(x, y), sf::Vector2f(vx, vy)});
}

}