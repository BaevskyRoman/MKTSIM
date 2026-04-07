#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Config::Visual {

    constexpr const char* WINDOW_TITLE = "MKTSIM";

    constexpr unsigned int WINDOW_WIDTH = 1024;
    constexpr unsigned int WINDOW_HEIGHT = 768;
    constexpr unsigned int FPS_LIMIT = 60;

    
    inline const sf::Color BG_COLOR = sf::Color(30, 30, 30);
    inline const sf::Color MOLECULE_COLOR = sf::Color(0, 150, 255);

    inline const sf::Color SELECTION_RECT_COLOR = sf::Color(100, 150, 255, 80);
    inline const sf::Color SELECTION_RECT_BORDER_COLOR = sf::Color(100, 150, 255, 200);

}