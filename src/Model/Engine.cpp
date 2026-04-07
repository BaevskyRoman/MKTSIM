#include "Model/Engine.hpp"
#include "Config/VisualConfig.hpp"
#include "Utils/Math.hpp"
#include <cmath>

namespace Model {

Engine::Engine()
{
    spawnMolecule(200.0f, 300.0f, 150.0f, 0.0f);
    spawnMolecule(500.0f, 320.0f, -150.0f, 0.0f);
    spawnMolecule(350.0f, 100.0f, 0.0f, 200.0f);
    spawnMolecule(360.0f, 500.0f, 0.0f, -200.0f);
}

void Engine::spawnMolecule(float x, float y, float vx, float vy) {
    molecules_.push_back({sf::Vector2f(x, y), sf::Vector2f(vx, vy)});
}

const std::vector<Molecule>& Engine::getMolecules() const {
    return molecules_;
}

void Engine::update(float deltaTime) {
    for (auto& mol : molecules_) {
        mol.move(deltaTime);
    }

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
                    normal = {1.0f, 0.0f};
                    overlap = minDist;
                }
                else {
                    normal = delta / distance;
                    overlap = minDist - distance;
                }
                

                float massSum = m1.mass + m2.mass;
                
                float ratio1 = m2.mass / massSum; 
                float ratio2 = m1.mass / massSum;

                m1.position -= normal * (overlap * ratio1);
                m2.position += normal * (overlap * ratio2);

                sf::Vector2f relativeVelocity = m1.velocity - m2.velocity;
                float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;


                if (velocityAlongNormal < 0) {
                    continue;
                }

                float impulseScalar = (2.0f * velocityAlongNormal) / massSum;
                m1.velocity -= normal * (impulseScalar * m2.mass);
                m2.velocity += normal * (impulseScalar * m1.mass);
            }
        }
    }
}

}