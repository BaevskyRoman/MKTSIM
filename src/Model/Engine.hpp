#pragma once

#include "Model/Molecule.hpp"
#include "Model/DynamicBody.hpp"
#include "Model/CollisionGrid.hpp"
#include "Model/Event.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <vector>
#include <SFML/System/Vector2.hpp>


namespace Model {

class Engine {
public:
    Engine() { grid.setGrid(sf::Vector2f(0.f, 0.f), sf::Vector2f(1000.f, 1000.f), 12.f, 100000); }

    void update(float deltaTime);

    void spawnMoleculesInArea(
        const sf::FloatRect& area, int count, float min_speed, float max_speed, float mass, float radius
    );
    void spawnMoleculesInArea(
        const sf::FloatRect& area, float concentration, float min_speed, float max_speed, float mass, float radius
    );

    const std::vector<Molecule>& getMolecules() const { return molecules_; }
    const std::vector<sf::FloatRect>& getStaticBodies() const { return staticBodies_; }
    void spawnStaticBody(const sf::FloatRect& rect) { staticBodies_.push_back(rect); }
    const std::vector<DynamicBody>& getDynamicBodies() const { return dynamicBodies_; }
    void spawnDynamicBody(sf::Vector2f sz, sf::Vector2f pos, float m) { dynamicBodies_.push_back(DynamicBody(sz, pos, m)); }
    
    bool enableGravity = false;
    float gravityAcceleration = 0;
    
    friend class FileHandler;
    friend class SimulationRecorder;

private:
    std::vector<Molecule> molecules_;
    std::vector<sf::FloatRect> staticBodies_;
    std::vector<DynamicBody> dynamicBodies_;

    std::vector<Event> events;
    CollisionGrid grid;
    
    void handleCollisions();
    void handleCollision(DynamicBody& dBody, const sf::FloatRect& sBody);
    void handleCollision(DynamicBody& bodyA, DynamicBody& bodyB);
    void handleCollision(Molecule& mol, const sf::FloatRect& body);
    void handleCollision(Molecule& mol, DynamicBody& body);
    void handleCollision(Molecule& mol1, Molecule& mol2);
    void handleCollision(Event& event);
};

}