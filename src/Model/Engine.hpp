#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>
#include "Model/Molecule.hpp"

namespace Model {

class Engine {
public:
    Engine();
    ~Engine() = default;

    void update(float deltaTime);

    const std::vector<Molecule>& getMolecules() const;

    void spawnMolecule(float x, float y, float vx, float vy);

private:
    std::vector<Molecule> molecules_;
};

}