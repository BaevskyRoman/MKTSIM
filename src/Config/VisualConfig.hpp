#pragma once

#include <SFML/Graphics/Color.hpp>

namespace Config::Visual {

    constexpr unsigned int WINDOW_WIDTH = 1024;
    constexpr unsigned int WINDOW_HEIGHT = 768;
    constexpr unsigned int FPS_LIMIT = 60;
    
    constexpr const char* WINDOW_TITLE = "MKTSIM";

    inline const sf::Color BG_COLOR = sf::Color(30, 30, 30);
    inline const sf::Color MOLECULE_COLOR = sf::Color(0, 150, 255);
    //inline const sf::Color UI_PANEL_COLOR = sf::Color(50, 50, 50);

}