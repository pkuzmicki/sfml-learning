#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <random>

#include "Input.h"



sf::Vector2f lerp(sf::Vector2f start, sf::Vector2f end, float t) {

    sf::Vector2f distance(end - start);
    std::cout<<distance.x<<distance.y<<std::endl;

    if ((end - start).x < 0.1 && (end - start).y < 0.01) {
        return end;
    }

    return start + (end - start) * t;
}

sf::Vector2f move(sf::Vector2f start, sf::Vector2f end) {
    float speed = 2.f;
    sf::Vector2f direction = (end - start);
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y) ;
    
    return start + direction * speed;
}

int main() {
    int scale = 3;
    int tileSize = 32 * scale;
    int tilesInWidth = 9;
    int tilesInHeight = 7;
    unsigned int roomWidth = tileSize * tilesInWidth;
    unsigned int roomHeight = tileSize * tilesInHeight;

    auto window = sf::RenderWindow(sf::VideoMode({roomWidth , roomHeight}), "karcianka");
    Input input;
    input.setWindow(&window);
    window.setFramerateLimit(60);

    bool isInBattleMode = false;

    float x = 100.f;
    float y = 100.f;

    bool canMove = true;

    
    sf::Vector2f playerPosition(200.f, 200.f);

    sf::Vector2f worldOffset(0.f, 0.f);

    sf::FileInputStream i0("../res/WizardOfTheMoon-YG5y.ttf");
    sf::Font font(i0);

    sf::FileInputStream inputS("../res/goblin.png");
    sf::Texture player1(inputS);
    sf::Sprite sprite1(player1);
    sprite1.setScale(sf::Vector2f(3.f, 3.f));
    sprite1.setPosition(playerPosition);

    sf::FileInputStream in_t01("../res/missing.png");
    sf::Texture tile01(in_t01);
    sf::Sprite tile_missing(tile01);
    tile_missing.setScale(sf::Vector2f(3.f, 3.f));

    sf::FileInputStream in_t02("../res/gravel.png");
    sf::Texture tile02(in_t02);
    sf::Sprite tile_gravel(tile02);
    tile_gravel.setScale(sf::Vector2f(3.f, 3.f));

    sf::FileInputStream in_t03("../res/gravel.png");
    sf::Texture tile03(in_t03);
    sf::Sprite tile_path(tile03);
    tile_path.setScale(sf::Vector2f(3.f, 3.f));

    std::vector<std::vector<int>> layout(roomHeight, std::vector<int>(roomWidth));
    std::ifstream rooms("../res/rooms.txt");
    std::string line;
    std::getline(rooms, line);

    int row = 0;
    while (std::getline(rooms, line)) {
        for (int col = 0; col < line.size(); col++) {  
            layout[col][row] = line.at(col) - '0';
            //std::cout<<layout[col][row];
        } 
        //printf("\n");
        row++;
    }
    rooms.close();

    sf::Clock clock;
    clock.start();
    sf::Clock printLocation;
    printLocation.start();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution rand(1, 6);
    int dice_num;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } 
        }



         if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) && clock.getElapsedTime() > sf::seconds(1.f)) {
                isInBattleMode = !isInBattleMode;
                x = 100.f;
                y = 100.f;
                dice_num = rand(gen);
                clock.restart();
            }

            input.updateKeys();
            input.updateMouse();
        

        window.clear();

        if (!isInBattleMode) {
            // for (int row = 0; row < tilesInHeight; row++) {
            //     for (int col = 0; col < tilesInWidth; col++) {
            //         tile_missing.setPosition(sf::Vector2f(tileSize * col, tileSize * row + worldOffset.y));
            //         window.draw(tile_missing);
            //     }
            // }

            for (int row = 0; row < tilesInHeight; row++) {
                for (int col = 0; col < tilesInWidth; col++) {
                    //std::cout<<layout[col][row]<<"\n";
                    if (layout[col][row] == 1) {
                        tile_path.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));
                        window.draw(tile_path);
                    }
                    if (layout[col][row] == 2) {
                        tile_gravel.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));  
                        window.draw(tile_gravel);
                    }  
                }
            }

        

            sf::Vector2f startPosition(playerPosition);
            sf::Vector2f endPosition(sf::Vector2f(input.getMousePosition().x, input.getMousePosition().y));

           
            if (input.getMousePosition().x < roomWidth && input.getMousePosition().x > 0) {
                if (input.getMousePosition().y < roomWidth && input.getMousePosition().y > 0) {

                    if (input.getMousePosition().x != 0 && input.getMousePosition().y != 0) {
                        if (playerPosition != endPosition) {
                            playerPosition = lerp(startPosition, endPosition, 0.1);
                        } else {
                            //std::cout<<"stoje sztywno\n";
                        } 
                    }
                }
            }
    
            
            
            
            if (playerPosition.y < 32) {
                //worldOffset.y += roomHeight;
                playerPosition.y += roomHeight - tileSize;
            }

            if (playerPosition.y > window.getSize().y - 32) {
                worldOffset.y -= roomHeight;
                playerPosition.y -= roomHeight - tileSize;
            } 

            if (playerPosition.x < 32) {
                worldOffset.x += roomWidth;
                playerPosition.x += roomWidth - tileSize;
            }

            if (playerPosition.x > window.getSize().x - 32) {
                worldOffset.x -= roomWidth;
                playerPosition.x -= roomWidth - tileSize;
            }

            // if (printLocation.getElapsedTime() > sf::seconds(0.5)) {
            //     std::cout<<"Gracz: "<<startPosition.x<<" "<<startPosition.y<<"\n";
            //     printLocation.restart();
            // }
            sprite1.setPosition(playerPosition);
            window.draw(sprite1);


            //printf("%f, %f\n", playerPosition.x, playerPosition.y);
            //printf("%f, %f\n", endPosition.x, endPosition.y);
        }

        if (isInBattleMode) {
            sf::RectangleShape table(sf::Vector2f(roomWidth, 3*tileSize));
            table.setFillColor(sf::Color(20, 40, 120));
            table.setPosition(sf::Vector2f(0.0, roomHeight - 3*tileSize));


            sf::Text dice(font, std::to_string(dice_num));
            dice.setFillColor(sf::Color(255, 255, 255));
            dice.setPosition(sf::Vector2f(roomWidth/2, roomHeight - tileSize));

            window.draw(table);
            window.draw(dice);
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
