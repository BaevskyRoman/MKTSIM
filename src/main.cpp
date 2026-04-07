#include "Controller/AppController.hpp"
#include <iostream>
#include <exception>

int main() {
    try {
        Controller::AppController app;
        
        app.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Unknown error!" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
