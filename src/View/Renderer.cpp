#include "View/Renderer.hpp"
#include "Model/Engine.hpp"
#include "Model/Molecule.hpp"
#include "Config/VisualConfig.hpp"


namespace View {

Renderer::Renderer(float windowWidth, float windowHeight) {
    camera_.setSize(sf::Vector2f(windowWidth, windowHeight));
    camera_.setCenter(sf::Vector2f(windowWidth / 2.f, windowHeight / 2.f));
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

// CAMERA CONTROL
void Renderer::moveCamera(float dx, float dy) {
    camera_.move(sf::Vector2f(dx, dy));
}

void Renderer::zoomCamera(float factor) {
    camera_.zoom(factor);
}

}
