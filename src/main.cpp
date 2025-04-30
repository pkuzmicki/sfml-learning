#include <SFML/Graphics.hpp>

#include "Input.h"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({800, 600}), "karcianka");
    Input input;
    input.setWindow(&window);
    window.setFramerateLimit(60);

    bool isInBattleMode = false;

    float x = 100.f;
    float y = 100.f;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
        
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } 
        }

         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X)) {
                isInBattleMode = !isInBattleMode;
                x = 100.f;
                y = 100.f;
            }

            input.updateKeys();
            input.updateMouse();
        

        window.clear();

        if (!isInBattleMode) {
            sf::CircleShape shape1(25.f);
            shape1.setFillColor(sf::Color(0, 255, 0));
            //shape1.setPosition(sf::Vector2f(window.getSize().x/2, window.getSize().y/2));
            shape1.setPosition(sf::Vector2f(input.getMousePosition().x, input.getMousePosition().y));
            window.draw(shape1);
        }
            
        if (!isInBattleMode) {
            sf::CircleShape shape(25.f);
            shape.setFillColor(sf::Color(255, 0, 0));
            shape.setPosition(sf::Vector2(x, y));
            window.draw(shape);
        }

        if (isInBattleMode) {
            sf::RectangleShape panel(sf::Vector2(800.f, 150.f));
            panel.setFillColor(sf::Color(0, 0, 255));
            panel.setPosition(sf::Vector2(0.f, 450.f));
            window.draw(panel);
        }

        float speed = 5.f;

        if (!isInBattleMode) {
            if (input.get_isUp()) y -= speed;
            if (input.get_isDown()) y += speed;
            if (input.get_isLeft()) x -= speed;
            if (input.get_isRight()) x += speed;
        }

        window.display();
    }
}
