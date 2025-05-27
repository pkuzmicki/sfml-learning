#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <random>

#include "Input.h"



sf::Vector2f lerp(sf::Vector2f start, sf::Vector2f end, float t) {
    //sf::Vector2f distance(end - start);
    //std::cout<<distance.x<<distance.y<<std::endl;

    if ((end - start).x < 0.1 && (end - start).x > -0.1 && (end - start).y < 0.1  && (end - start).y > -0.1){
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
    
    sf::Vector2f playerPosition(roomWidth/2, roomHeight/2);
    sf::Vector2f movePosition;
    sf::Vector2f endPosition(playerPosition);

    sf::Vector2f worldOffset(0.f, 0.f);

    sf::FileInputStream i0("../res/WizardOfTheMoon-YG5y.ttf");
    sf::Font font(i0);

    sf::FileInputStream iS0("../res/goblin.png");
    sf::Texture pTxt(iS0);
    sf::Sprite playerSprite(pTxt);
    playerSprite.setScale(sf::Vector2f(3.f, 3.f));
    playerSprite.setPosition(sf::Vector2f(playerPosition.x - 48, playerPosition.y - 30));

    sf::FileInputStream iT0("../res/missing.png");
    sf::Texture tile01(iT0);
    sf::Sprite tile_missing(tile01);
    tile_missing.setScale(sf::Vector2f(3.f, 3.f));

    sf::FileInputStream iT1("../res/gravel.png");
    sf::Texture tile02(iT1);
    sf::Sprite tile_gravel(tile02);
    tile_gravel.setScale(sf::Vector2f(3.f, 3.f));

    sf::FileInputStream iT2("../res/path.png");
    sf::Texture tile03(iT2);
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

        window.clear();
        input.updateKeys();
        input.updateMouse();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::X) && clock.getElapsedTime() > sf::seconds(1.f)) {
            isInBattleMode = !isInBattleMode;
            dice_num = rand(gen);
            clock.restart();
        }
       
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
                    if (layout[col][row] == 2) {
                        tile_path.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));
                        window.draw(tile_path);
                    }
                    if (layout[col][row] == 1) {
                        tile_gravel.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));  
                        window.draw(tile_gravel);
                    }  
                }
            }

        
            
            movePosition = sf::Vector2f(input.getMousePosition().x, input.getMousePosition().y);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                endPosition = movePosition;
            }

            if (input.getMousePosition().x < roomWidth && input.getMousePosition().x > 0) {
                if (input.getMousePosition().y < roomHeight && input.getMousePosition().y > 0) {
                    if (input.getMousePosition().x != 0 && input.getMousePosition().y != 0) {
                        if (playerPosition != endPosition) {
                            playerPosition = lerp(playerPosition, endPosition, 0.1);
                        } 
                    }
                }
            }
    
            
            
            
            if (playerPosition.y < 32) {
                worldOffset.y += roomHeight;
                playerPosition.y += roomHeight - tileSize;
                endPosition.y = roomHeight - tileSize/2;
            }

            if (playerPosition.y > window.getSize().y - 32) {
                worldOffset.y -= roomHeight;
                playerPosition.y -= roomHeight - tileSize;
                endPosition.y = tileSize/2;
            } 

            if (playerPosition.x < 32) {
                worldOffset.x += roomWidth;
                playerPosition.x += roomWidth - tileSize;
                endPosition.x = roomWidth - tileSize/2;
            }

            if (playerPosition.x > window.getSize().x - 32) {
                worldOffset.x -= roomWidth;
                playerPosition.x -= roomWidth - tileSize;
                endPosition.x = tileSize/2;
            }

            if (printLocation.getElapsedTime() > sf::seconds(0.25) && playerPosition != endPosition) {
                std::cout<<"Gracz: "<<playerPosition.x<<" "<<playerPosition.y<<"\n";
                std::cout<<"Koniec: "<<endPosition.x<<" "<<endPosition.y<<"\n";
                printLocation.restart();
            }

            
            playerSprite.setPosition(sf::Vector2f(playerPosition.x - 48, playerPosition.y - 30));
            window.draw(playerSprite);
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

        window.display();
    }
}
