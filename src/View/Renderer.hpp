#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

namespace Model {
    class Engine;
}

namespace View {

class Renderer {
public:
    Renderer(float windowWidth, float windowHeight);
    ~Renderer() = default;

    const sf::View& getCamera() const { return camera_; }

    void draw(sf::RenderWindow& window, const Model::Engine& engine);
    void drawSelection(sf::RenderWindow& window, const sf::Vector2f& start, const sf::Vector2f& end);
    
    void moveCamera(float dx, float dy);
    void zoomCamera(float factor);

private:
    sf::View camera_;

    sf::VertexArray moleculeVertices_;
    std::vector<sf::Vector2f> unitCircle_;
    
    void initUnitCircle(int pointsPerCircle);
};

}