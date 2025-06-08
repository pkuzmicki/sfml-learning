#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <optional>

#include "Input.h"

enum Mode {
    Exploration,
    Battle,
    Crafting,
    Equipment
};

class ImageManger {
    static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> images;

    static void add(std::string name) {
        auto texture = std::make_shared<sf::Texture>();
        texture->loadFromFile("../res/"+ name +".png");
        images[name] = texture;
     }
public: 
    static void load() {
        add("goblin");

        add("default_snail");

        add("missing");
        add("gravel");
        add("path");
    }

    static std::shared_ptr<sf::Texture> getImageByName(std::string name) {
        auto res = images.find(name);
        if (res != images.end()) {
            return res->second; 
        }
        return nullptr;
    }

};

std::unordered_map<std::string, std::shared_ptr<sf::Texture>> ImageManger::images;


enum AbilityType {
    ATTACK,
    DEFENCE,
};

class Ability {
    std::string name;
    std::string description;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    AbilityType type;
    float value;

public:
    Ability(std::string name, std::string desc, AbilityType type, float value):
    texture(ImageManger::getImageByName("missing")),
    sprite(*texture),
    name(name),
    description(desc),
    type(type),
    value(value) {}

    float getValue() {
        return this->value;
    }
};


class Snail {
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    
    float maxHp;
    float currentHp;
    std::vector<Ability> listOfAbilities;

public:
    Snail(float maxHp):
        texture(ImageManger::getImageByName("default_snail")),
        sprite(*texture),
        maxHp(maxHp),
        currentHp(maxHp) {
        this->sprite.setScale(sf::Vector2f(5.f, 5.f));
        this->listOfAbilities = {
            Ability("Slap", "Slap enemy with your tail", AbilityType::ATTACK, 5.f),
            Ability("Spit", "Spit on enemy with your slime", AbilityType::ATTACK, 3.f),
            Ability("Hide", "Defend yourself with your shell", AbilityType::DEFENCE, 4.f),
        };
    }

    std::vector<Ability>& getListOfAbilities() {
        return this->listOfAbilities;
    }

    sf::Sprite& getSprite() {
        return this->sprite;
    }
};


class Player {
private:
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    sf::Vector2f currentPosition;
    sf::Vector2f movePosition;
    sf::Vector2f endPosition;

    std::vector<Snail> listOfSnails;

public: 
    Player(sf::Vector2f position):
        texture(ImageManger::getImageByName("goblin")),
        sprite(*texture),
        currentPosition(position),
        endPosition(position) {
        sprite.setPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->sprite.setScale(sf::Vector2f(3.f, 3.f));
        listOfSnails = {Snail(10.f), Snail(10.f), Snail(10.f)};
    }

    void setCurrPos(sf::Vector2f position) {
        sprite.setPosition(sf::Vector2f(this->currentPosition.x - 48, this->currentPosition.y - 30));
        this->currentPosition = position;
    }

    void setMovePos(sf::Vector2f position) {
        this->movePosition = position;
    }

    void setEndPos(sf::Vector2f position) {
        this->endPosition = position;
    }

    void setCurrX(float x) {
        this->currentPosition.x = x;
    }

    void setCurrY(float y) {
        this->currentPosition.y = y;
    }

    void setEndX(float x) {
        this->endPosition.x = x;
    }

    void setEndY(float y) {
        this->endPosition.y = y;
    }

    sf::Vector2f getCurrPos() {
        return this->currentPosition;
    }
    
    sf::Vector2f getMovePos() {
        return this->movePosition;
    }

    sf::Vector2f getEndPos() {
        return this->endPosition;
    }

    sf::Sprite getSprite() {
        return this->sprite;
    }

    std::vector<Snail>& getListOfSnails() {
        return listOfSnails;
    }
};


// enum Element {
//     Cold,
//     Lightning,
//     Fire,
// };

// class Plant {
// private: 
//     std::string name;
//     sf::Sprite sprite;

//     sf::RectangleShape shape;
//     sf::Vector2f size;
//     std::optional<Element> element;
// public: 
    
//     void setSize(sf::Vector2f size) {
//         this->size = size;
//     }
    
// };

// class Potion {
// private:
//     std::vector<Plant> ingridients();
// };


class Enemy {
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    std::vector<Snail> listOfSnails;
    
public:
    Enemy():
    texture(ImageManger::getImageByName("goblin")),
    sprite(*texture) {
        listOfSnails = {Snail(10.f), Snail(10.f), Snail(10.f)};
    }

    std::vector<Snail>& getListOfSnails() {
        return listOfSnails;
    }
};

sf::Vector2f lerp(sf::Vector2f start, sf::Vector2f end, float t) {
    if ((end - start).x < 0.1 && (end - start).x > -0.1 && (end - start).y < 0.1  && (end - start).y > -0.1) {
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
    window.setFramerateLimit(60);

    Input input;
    input.setWindow(&window);
    
    ImageManger::load();

    Player player(sf::Vector2f(roomWidth/2, roomHeight/2));

    //Mode gameMode = Mode::Exploration;
    //Mode gameMode = Mode::Crafting;
    Mode gameMode = Mode::Battle;
    
    sf::Vector2f worldOffset(0.f, 0.f);

    sf::FileInputStream i0("../res/WizardOfTheMoon-YG5y.ttf");
    sf::Font font(i0);  

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
            gameMode = static_cast<Mode>(gameMode+1);
            dice_num = rand(gen);
            clock.restart();
        }
       
        if (gameMode == Mode::Exploration) {
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
                        //tile_path.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));
                        //window.draw(tile_path);
                    }
                    if (layout[col][row] == 1) {
                        //tile_gravel.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));  
                        //window.draw(tile_gravel);
                    }  
                }
            }

        
            player.setMovePos(sf::Vector2f(input.getMousePosition().x, input.getMousePosition().y));
            //std::cout<<player.getMovePos().x<<player.getMovePos().y<<std::endl;
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                player.setEndPos(player.getMovePos());
            }

            if (input.getMousePosition().x < roomWidth && input.getMousePosition().x > 0) {
                if (input.getMousePosition().y < roomHeight && input.getMousePosition().y > 0) {
                    if (input.getMousePosition().x != 0 && input.getMousePosition().y != 0) {
                        if (player.getCurrPos() != player.getEndPos()) {
                            player.setCurrPos(lerp(player.getCurrPos(), player.getEndPos(), 0.1));
                        } 
                    }
                }
            }
    

            if (player.getCurrPos().y < 32) {
                worldOffset.y += roomHeight;
                // player.setCurrPos().y += roomHeight - tileSize;
                // player.setEndPos().y = roomHeight - tileSize/2;
                player.setCurrY(roomHeight - tileSize);
                player.setEndY(roomHeight - tileSize/2);
            }

            if (player.getCurrPos().y > window.getSize().y - 32) {
                worldOffset.y -= roomHeight;
                // player.setCurrPos().y -= roomHeight - tileSize;
                // player.setEndPos().y = tileSize/2;
                player.setCurrY(roomHeight - tileSize);
                player.setEndY(tileSize/2);
            } 

            if (player.getCurrPos().x < 32) {
                worldOffset.x += roomWidth;
                // player.setCurrPos().x += roomWidth - tileSize;
                // player.setEndPos().x = roomWidth - tileSize/2;
                player.setCurrX(roomWidth - tileSize);
                player.setEndX(roomWidth - tileSize/2);
            }

            if (player.getCurrPos().x > window.getSize().x - 32) {
                worldOffset.x -= roomWidth;
                // player.setCurrPos().x -= roomWidth - tileSize;
                // player.setEndPos().x = tileSize/2;
                player.setCurrX(roomWidth - tileSize);
                player.setEndX(tileSize/2);
            }

            // && player.getCurrPos() != player.getEndPos()
            if (printLocation.getElapsedTime() > sf::seconds(0.25)) {
                std::cout<<"Gracz: "<<player.getCurrPos().x<<" "<<player.getCurrPos().y<<"\n";
                std::cout<<"Koniec: "<<player.getEndPos().x<<" "<<player.getEndPos().y<<"\n";
                printLocation.restart();
            }

            
            //playerSprite.setPosition(sf::Vector2f(player.getCurrPos().x - 48, player.getCurrPos().y - 30));
            window.draw(player.getSprite());
        }

        if (gameMode == Mode::Battle) {
            Snail snail1 = player.getListOfSnails().at(0);
            Snail snail2 = player.getListOfSnails().at(1);
            Snail snail3 = player.getListOfSnails().at(2);

            snail1.getSprite().setPosition(sf::Vector2f(50, roomHeight/2));
            window.draw(snail1.getSprite());
            snail2.getSprite().setPosition(sf::Vector2f(240, roomHeight/2));
            window.draw(snail2.getSprite());
            snail3.getSprite().setPosition(sf::Vector2f(430, roomHeight/2));
            window.draw(snail3.getSprite());

            Enemy currentEnemy;
            Snail snail4 = currentEnemy.getListOfSnails().at(0);
            Snail snail5 = currentEnemy.getListOfSnails().at(1);
            Snail snail6 = currentEnemy.getListOfSnails().at(2);

            snail4.getSprite().setRotation(sf::degrees(180));
            snail4.getSprite().setPosition(sf::Vector2f(450, roomHeight/2-tileSize));
            window.draw(snail4.getSprite());
            snail5.getSprite().setRotation(sf::degrees(180));
            snail5.getSprite().setPosition(sf::Vector2f(640, roomHeight/2-tileSize));
            window.draw(snail5.getSprite());
            snail6.getSprite().setRotation(sf::degrees(180));
            snail6.getSprite().setPosition(sf::Vector2f(830, roomHeight/2-tileSize));
            window.draw(snail6.getSprite());

            sf::RectangleShape table(sf::Vector2f(roomWidth, 2*tileSize));
            table.setFillColor(sf::Color(20, 40, 120));
            table.setPosition(sf::Vector2f(0.0, roomHeight - tileSize));

            sf::Text dice(font, std::to_string(dice_num));
            dice.setFillColor(sf::Color(255, 255, 255));
            dice.setPosition(sf::Vector2f(roomWidth/2, roomHeight - tileSize));


            window.draw(table);
            window.draw(dice);
        }

        if (gameMode == Mode::Crafting) {
            //Plant plant();

            // plant.size = sf::Vector2f(tileSize, tileSize);
            // plant.shape = sf::RectangleShape(plant.size);
            // plant.shape.setFillColor(sf::Color::Green);
            // plant.shape.setPosition(sf::Vector2f(roomWidth/2, roomHeight/2));

            // if (plant.shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(input.getMousePosition())) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            //     plant.shape.setPosition(static_cast<sf::Vector2f>(input.getMousePosition()));
            // }
            // window.draw(plant.shape);
        }

        if (gameMode == Mode::Equipment) {

        }

        window.display();
    }
}
