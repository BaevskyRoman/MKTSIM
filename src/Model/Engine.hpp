#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include "Model/Molecule.hpp"
#include "Model/DynamicBody.hpp"


namespace Model {

class Engine {
public:
    Engine() = default;
    ~Engine() = default;

    void update(float deltaTime);

    const std::vector<Molecule>& getMolecules() const;
    void spawnMoleculesInArea(const sf::FloatRect& area, float concentration, float min_speed, float max_speed, 
                                float mass, float radius);
    void spawnMolecule(float x, float y, float vx, float vy);

    const std::vector<sf::FloatRect>& getStaticBodies() const;
    void spawnStaticBody(const sf::FloatRect& rect);

    const std::vector<DynamicBody>& getDynamicBodies() const;
    void spawnDynamicBody(sf::Vector2f sz, sf::Vector2f pos, float m);

private:
    std::vector<Molecule> molecules_;
    std::vector<sf::FloatRect> staticBodies_;
    std::vector<DynamicBody> dynamicBodies_;
};

}