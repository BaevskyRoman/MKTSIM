#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "Model/Molecule.hpp"


namespace Model {

class Engine {
public:
    Engine() = default;
    ~Engine() = default;

    void update(float deltaTime);

    const std::vector<Molecule>& getMolecules() const;

    const std::vector<sf::FloatRect>& getHardMO() const;
    void addHardMO(const sf::FloatRect& rect);

    void spawnMoleculesInArea(const sf::FloatRect& area, float concentration, float min_speed, float max_speed, 
        float mass, float radius);
    void spawnMolecule(float x, float y, float vx, float vy);

private:
    std::vector<Molecule> molecules_;
    std::vector<sf::FloatRect> hardMO_;
};

}