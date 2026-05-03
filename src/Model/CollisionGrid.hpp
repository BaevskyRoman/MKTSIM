#pragma once

#include "Model/Molecule.hpp"
#include "Model/Event.hpp"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <cstdint>


namespace Model {

class CollisionGrid {
public:
    void setGrid(sf::Vector2f start, sf::Vector2f end, float csize, size_t moleculesQTY);
    void update(const std::vector<Molecule>& molecules);
    void checkGrid(const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const;

private:
    std::vector<uint32_t> head;
    std::vector<uint32_t> next;
    std::vector<uint32_t> unbound;

    uint32_t gridWidth;
    uint32_t gridHeight;
    sf::Vector2f startPosition;
    sf::Vector2f endPosition;
    float cellSize;

    static constexpr uint32_t EMPTY = std::numeric_limits<uint32_t>::max();

    void checkInCell(uint32_t cell, const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const;
    void checkCells(uint32_t cellA, int32_t bx, int32_t by, 
        const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const;
    void checkUnbound(const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const;

    bool isInside(sf::Vector2f position) const;
    uint32_t getGridIndex(sf::Vector2f position) const;
    uint32_t getGridIndex(uint32_t x, uint32_t y) const { return y * gridWidth + x; }
};

}