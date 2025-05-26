#include "Input.h"

void Input::setWindow(sf::RenderWindow* window) {
    this->window = window;
}

void Input::updateKeys() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        isUp = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        isDown = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        isLeft = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        isRight = true;
    }

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        isUp = false;
    }
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        isDown = false;
    }
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        isLeft = false;
    }
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        isRight = false;
    }
}

void Input::updateMouse() {
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        this->mousePosition = sf::Mouse::getPosition(*window);
        //printf("%i, %i\n", this->getMousePosition().x, this->getMousePosition().y); 
    } 
}


