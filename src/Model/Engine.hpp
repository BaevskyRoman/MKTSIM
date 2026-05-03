#pragma once

#include "Model/Molecule.hpp"
#include "Model/Body.hpp"
#include "Model/CollisionGrid.hpp"
#include "Model/Event.hpp"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>


namespace Model {

class Engine {
public:
    Engine() { grid.setGrid(sf::Vector2f(0.f, 0.f), sf::Vector2f(1000.f, 1000.f), 12.f, 100000); }

    void update(float deltaTime);

    void spawnMoleculesInArea(
        const sf::FloatRect& area, int count, 
        float min_speed, float max_speed, 
        float mass, float radius
    );
    void spawnMoleculesInArea(
        const sf::FloatRect& area, float concentration, 
        float min_speed, float max_speed, 
        float mass, float radius
    );

    const std::vector<StaticBody>& getStaticBodies() const { return staticBodies_; }
    const std::vector<DynamicBody>& getDynamicBodies() const { return dynamicBodies_; }
    const std::vector<Molecule>& getMolecules() const { return molecules_; }

    void spawnStaticBody(sf::Vector2f size, sf::Vector2f position) { 
        staticBodies_.push_back(StaticBody(size, position)); 
    }
    void spawnStaticBody(const sf::FloatRect rect) {
        spawnStaticBody(rect.size, rect.position + rect.size / 2.f);
    }
    void spawnDynamicBody(sf::Vector2f size, sf::Vector2f position, float m) { 
        dynamicBodies_.push_back(DynamicBody(size, m, position)); 
    }
    void spawnDynamicBody(const sf::FloatRect rect, float m) {
        spawnDynamicBody(rect.size, rect.position + rect.size/2.0f, m);
    }
    
    bool enableGravity = false;
    float gravityAcceleration = 0;
    bool useGrid = true;
    
    friend class FileHandler;
    friend class SimulationRecorder;

private:
    std::vector<StaticBody> staticBodies_;
    std::vector<DynamicBody> dynamicBodies_;
    std::vector<Molecule> molecules_;

    std::vector<EventSD> eventsSD;
    std::vector<EventDD> eventsDD;
    std::vector<EventSM> eventsSM;
    std::vector<EventDM> eventsDM;
    std::vector<EventMM> eventsMM;
    CollisionGrid grid;
    
    void detectCollisions();
    void resolveCollisions();

    void resolveCollision(EventSD& event);
    void resolveCollision(EventDD& event);
    void resolveCollision(EventSM& event);
    void resolveCollision(EventDM& event);
    void resolveCollision(EventMM& event);
};

}