#include "View/Renderer.hpp"
#include "Model/Engine.hpp"
#include "Model/Molecule.hpp"
#include "Config/VisualConfig.hpp"
#include "Utils/Math.hpp"
#include <cmath>


namespace View {

Renderer::Renderer(float windowWidth, float windowHeight) {
    camera_.setSize(sf::Vector2f(windowWidth, windowHeight));
    camera_.setCenter(sf::Vector2f(windowWidth / 2.f, windowHeight / 2.f));

    moleculeVertices_.setPrimitiveType(sf::PrimitiveType::Triangles);
    initUnitCircle(Config::Visual::MOLECULE_VERTICES_QTY);
}


void Renderer::draw(sf::RenderWindow& window, const Model::Engine& engine) {
    window.setView(camera_);
    const auto& molecules = engine.getMolecules();

    moleculeVertices_.clear();

    int points = static_cast<int>(unitCircle_.size()) - 1;
    for (const auto& mol : molecules) {
        sf::Vector2f center = mol.position;
        float r = mol.radius;
        
        for (int i = 0; i < points; ++i) {
            sf::Vertex vCenter;
            vCenter.position = center;
            vCenter.color = Config::Visual::MOLECULE_COLOR;

            sf::Vertex vCurrent;
            vCurrent.position = center + unitCircle_[i] * r;
            vCurrent.color = Config::Visual::MOLECULE_COLOR;

            sf::Vertex vNext;
            vNext.position = center + unitCircle_[i + 1] * r;
            vNext.color = Config::Visual::MOLECULE_COLOR;

            moleculeVertices_.append(vCenter);
            moleculeVertices_.append(vCurrent);
            moleculeVertices_.append(vNext);
        }
    }
    window.draw(moleculeVertices_);

    window.setView(window.getDefaultView());
}


void Renderer::drawSelection(sf::RenderWindow& window, const sf::Vector2f& start, const sf::Vector2f& end) {
    window.setView(camera_);
    
    sf::RectangleShape rect;

    float left = std::min(start.x, end.x);
    float top = std::min(start.y, end.y);
    float width = std::abs(start.x - end.x);
    float height = std::abs(start.y - end.y);
    
    rect.setPosition({left, top});
    rect.setSize({width, height});
    
    rect.setFillColor(Config::Visual::SELECTION_RECT_COLOR); 
    rect.setOutlineColor(Config::Visual::SELECTION_RECT_BORDER_COLOR);
    rect.setOutlineThickness(1.0f * camera_.getSize().x / window.getSize().x); 
    
    window.draw(rect);
    window.setView(window.getDefaultView());
}


void Renderer::initUnitCircle(int points) {
    unitCircle_.reserve(points + 1);
    for (int i = 0; i <= points; ++i) {
        float angle = i * 2.0f * Utils::Math::PI / points;
        unitCircle_.push_back(sf::Vector2f(std::cos(angle), std::sin(angle)));
    }
}


// CAMERA CONTROL
void Renderer::moveCamera(float dx, float dy) {
    camera_.move(sf::Vector2f(dx, dy));
}

void Renderer::zoomCamera(float factor) {
    camera_.zoom(factor);
}

}
