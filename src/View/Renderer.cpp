#include "View/Renderer.hpp"
#include "Model/Engine.hpp"
#include "Model/Molecule.hpp"
#include "Config/VisualConfig.hpp"

namespace View {

Renderer::Renderer(float windowWidth, float windowHeight) {
    camera_.setSize(sf::Vector2f(windowWidth, windowHeight));
    camera_.setCenter(sf::Vector2f(windowWidth / 2.f, windowHeight / 2.f));
}

void Renderer::moveCamera(float dx, float dy) {
    camera_.move(sf::Vector2f(dx, dy));
}

void Renderer::zoomCamera(float factor) {
    camera_.zoom(factor);
}

void Renderer::draw(sf::RenderWindow& window, const Model::Engine& engine) {
    window.setView(camera_);
    sf::CircleShape circle;
    
    circle.setFillColor(Config::Visual::MOLECULE_COLOR); 

    const auto& molecules = engine.getMolecules();

    for (const auto& mol : molecules) {
        circle.setRadius(mol.radius);
        circle.setOrigin({mol.radius, mol.radius});
        circle.setPosition(mol.position);
        window.draw(circle);
    }
    window.setView(window.getDefaultView());
}

}
