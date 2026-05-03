#include "Model/CollisionGrid.hpp"
#include "Model/CollisionLogic.hpp"


namespace Model {

void CollisionGrid::setGrid(sf::Vector2f start, sf::Vector2f end, float csize, size_t moleculesQTY) {
    cellSize = csize;

    float width = end.x - start.x;
    float height = end.y - start.y;

    gridWidth = static_cast<uint32_t>(std::ceil(width / cellSize));
    gridHeight = static_cast<uint32_t>(std::ceil(height / cellSize));

    startPosition = start;
    endPosition.x = startPosition.x + static_cast<float>(gridWidth * cellSize);
    endPosition.y = startPosition.y + static_cast<float>(gridHeight * cellSize);

    head.assign(gridWidth * gridHeight, EMPTY);
    next.assign(moleculesQTY, EMPTY);

    if ((gridHeight == 0) || (gridWidth == 0)) {
        throw std::runtime_error("[CollisionGrid::setGrid] Attempt to create a grid of zero size");
    }
}


void CollisionGrid::update(const std::vector<Molecule>& molecules) {
    std::fill(head.begin(), head.end(), EMPTY);
    unbound.clear();

    for (uint32_t i = 0; i < molecules.size(); ++i) {
        sf::Vector2f pos = molecules[i].position;

        if (isInside(pos)) {
            uint32_t idx = getGridIndex(pos);

            next[i] = head[idx];
            head[idx] = i;
        } else {
            unbound.push_back(i);
        }
    }
}


void CollisionGrid::checkGrid(const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const {
    for (uint32_t y = 0; y < gridHeight; ++y) {
        for (uint32_t x = 0; x < gridWidth; ++x) {
            uint32_t currentCellInd = getGridIndex(x, y);

            checkInCell(currentCellInd, molecules, events);

            checkCells(currentCellInd, x + 1, y, molecules, events); // Right
            checkCells(currentCellInd, x - 1, y + 1, molecules, events); // Bottom-Left
            checkCells(currentCellInd, x, y + 1, molecules, events); // Bottom
            checkCells(currentCellInd, x + 1, y + 1, molecules, events); // Bottom-Right
        }
    }
    checkUnbound(molecules, events);
}


void CollisionGrid::checkInCell(uint32_t cell, const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const {
    uint32_t i = head[cell];

    while (i != EMPTY) {
        uint32_t j = next[i];
        while (j != EMPTY) {
            if (Geo::isColliding(molecules[i], molecules[j])) {
                events.emplace_back(i, j);
            }
            j = next[j];
        }
        i = next[i];
    }
}


void CollisionGrid::checkCells(uint32_t cellA, int32_t bx, int32_t by, 
                        const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const {
    if (bx < 0 || bx >= (int32_t)gridWidth || by < 0 || by >= (int32_t)gridHeight) return;

    uint32_t cellB = getGridIndex(static_cast<uint32_t>(bx), static_cast<uint32_t>(by));
    
    uint32_t i = head[cellA];
    while (i != EMPTY) {
        uint32_t j = head[cellB];
        while (j != EMPTY) {
            if (Geo::isColliding(molecules[i], molecules[j])) {
                events.emplace_back(i, j);
            }
            j = next[j];
        }
        i = next[i];
    }
}


void CollisionGrid::checkUnbound(const std::vector<Molecule>& molecules, std::vector<EventMM>& events) const {
    // something
}


bool CollisionGrid::isInside(sf::Vector2f position) const {
    return (position.x > startPosition.x &&
        position.x < endPosition.x &&
        position.y > startPosition.y &&
        position.y < endPosition.y);
}


uint32_t CollisionGrid::getGridIndex(sf::Vector2f position) const {
    sf::Vector2f relative = position - startPosition;

    uint32_t x = static_cast<uint32_t>(relative.x / cellSize);
    uint32_t y = static_cast<uint32_t>(relative.y / cellSize);

    if (x >= gridWidth)  x = gridWidth - 1;
    if (y >= gridHeight) y = gridHeight - 1;

    return getGridIndex(x, y);
}

}