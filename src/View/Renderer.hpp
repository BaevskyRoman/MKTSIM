#pragma once

#include <SFML/Graphics.hpp>

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
    
    void moveCamera(float dx, float dy);
    void zoomCamera(float factor);

private:
    sf::View camera_;
};

}