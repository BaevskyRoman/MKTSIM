#include <cmath>
#include <random>

#include "Model/Engine.hpp"
#include "Config/VisualConfig.hpp"
#include "Utils/Math.hpp"


namespace Model {

Engine::Engine()
{
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
                
                // COLLISION HANDLING
                float massSum = m1.mass + m2.mass;
                
                float ratio1 = m2.mass / massSum; 
                float ratio2 = m1.mass / massSum;

                m1.position -= normal * (overlap * ratio1);
                m2.position += normal * (overlap * ratio2);

                // IMPULSE CALCULATION
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

void Engine::spawnMoleculesInArea(const sf::FloatRect& area, float concentration, float speed, float mass, float radius) {
    float areaSize = area.size.x * area.size.y;
    float maxMolecules = areaSize / (4.0f * radius * radius);
    
    int count = static_cast<int>(maxMolecules * concentration);

    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::uniform_real_distribution<float> xDist(area.position.x + radius, area.position.x + area.size.x - radius);
    std::uniform_real_distribution<float> yDist(area.position.y + radius, area.position.y + area.size.y - radius);
    
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * Utils::Math::PI);

    for (int i = 0; i < count; ++i) {
        float px = xDist(gen);
        float py = yDist(gen);
        float angle = angleDist(gen);
        
        float vx = std::cos(angle) * speed;
        float vy = std::sin(angle) * speed;

        molecules_.push_back({sf::Vector2f(px, py), sf::Vector2f(vx, vy), mass, radius});
    }
}

void Engine::spawnMolecule(float x, float y, float vx, float vy) {
    molecules_.push_back({sf::Vector2f(x, y), sf::Vector2f(vx, vy)});
}

}