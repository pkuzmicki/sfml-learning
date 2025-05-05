#include <SFML/Graphics.hpp>

#include "Input.h"

sf::Vector2f lerp(sf::Vector2f start, sf::Vector2f end, float t) {
    float x = start.x + (end - start).x * t;
    float y = start.y + (end - start).y * t;
    //return sf::operator*((end - start), 0.1f); 
    return sf::Vector2f(x, y);
}

int main() {
    sf::Clock clock;
    clock.start();

    auto window = sf::RenderWindow(sf::VideoMode({800, 600}), "karcianka");
    Input input;
    input.setWindow(&window);
    window.setFramerateLimit(60);

    bool isInBattleMode = false;

    float x = 100.f;
    float y = 100.f;

    sf::Vector2f playerPosition(10.f, 10.f);

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
            sf::FileInputStream inputS("../res/goblin.png");
            sf::Texture player1(inputS);
            sf::Sprite sprite1(player1);
            sprite1.setScale(sf::Vector2f(3.f, 3.f));
            sprite1.setPosition(playerPosition);
            window.draw(sprite1);
            
            sf::Vector2f startPosition(playerPosition);
            sf::Vector2f endPosition(sf::Vector2f(input.getMousePosition().x, input.getMousePosition().y));
    
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                while (sprite1.getPosition() != endPosition) {
                sprite1.setPosition(lerp(startPosition, endPosition, 0.1));
                printf("%f, %f\n", sprite1.getPosition().x, sprite1.getPosition().y);
                //sprite1.setPosition(sf::operator+=(sprite1.getPosition(), lerp(startPosition, endPosition, 1)));
                window.draw(sprite1);
                }
                playerPosition = sprite1.getPosition();
            }
            
        }
            
        if (!isInBattleMode) {
            sf::Texture player3;
            player3.loadFromFile("../res/goblin.png");
        
            sf::Sprite sprite(player3);
            sprite.setScale(sf::Vector2(4.f, 4.f));
            sprite.setPosition(sf::Vector2(100.f, 100.f));   
    
            //window.draw(sprite);

            sf::FileInputStream input("../res/goblin.png");
            sf::Texture player1(input);
            sf::Sprite sprite1(player1);

            sprite1.setScale(sf::Vector2(3.f, 3.f));
            sprite1.setPosition(sf::Vector2(x, y));
            //window.draw(sprite1);
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
