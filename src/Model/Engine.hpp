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
    void spawnMoleculesInArea(const sf::FloatRect& area, int count, float min_speed, float max_speed, 
                                float mass, float radius);
    void spawnMoleculesInArea(const sf::FloatRect& area, float concentration, float min_speed, float max_speed, 
                                float mass, float radius);

    const std::vector<sf::FloatRect>& getStaticBodies() const;
    void spawnStaticBody(const sf::FloatRect& rect);

    const std::vector<DynamicBody>& getDynamicBodies() const;
    void spawnDynamicBody(sf::Vector2f sz, sf::Vector2f pos, float m);
    
    bool enableGravity = false;
    float gravityAcceleration = 0;
    
    friend class FileHandler;
private:
    std::vector<Molecule> molecules_;
    std::vector<sf::FloatRect> staticBodies_;
    std::vector<DynamicBody> dynamicBodies_;
    
    void handleCollision(Molecule& mol, const sf::FloatRect& body);
    void handleCollision(Molecule& mol, DynamicBody& body);
    void handleCollision(DynamicBody& dBody, const sf::FloatRect& sBody);
    void handleCollision(DynamicBody& bodyA, DynamicBody& bodyB);
    void handleCollision(Molecule& mol1, Molecule& mol2);
    void handleCollision();
};

}