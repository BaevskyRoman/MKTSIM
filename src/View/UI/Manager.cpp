#include "View/UI/Manager.hpp"


namespace View {
namespace UI {


bool Manager::init(sf::RenderWindow& window) {
    return ImGui::SFML::Init(window);
}


Manager::~Manager() {
    ImGui::SFML::Shutdown();
}


void Manager::update(sf::RenderWindow& window, sf::Time dt) {
    ImGui::SFML::Update(window, dt);
    if (bottomBarVisible_) bottomBar_.update(window);
    if (topBarVisible_) topBar_.update(window, dt);
    //ImGui::ShowDemoWindow();
}


void Manager::render(sf::RenderWindow& window) {
    ImGui::SFML::Render(window);
}

}
}