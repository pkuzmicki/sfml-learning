#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <optional>
#include <algorithm>

#include "Input.h"

static sf::Clock stoper;

enum Mode {
    Exploration,
    Battle,
    Crafting,
    Equipment
};

class ImageManager {
    static std::unordered_map<std::string, std::shared_ptr<sf::Texture>> images;

public: 

    static void add(std::string name) {
        auto texture = std::make_shared<sf::Texture>();
        if (texture->loadFromFile("../res/"+ name +".png")) {
            images[name] = texture;
        } else {
            std::perror("blad wczytywania obrazu");
        }
    }

    static void remove(std::string name) {
        images.erase(name);
    }

    static void load() {
        add("goblin");
        add("goblin3d");

        add("default_snail");

        add("plant01");

        add("missing");
        add("gravel");
        add("path");

        add("icon_defence");
        add("icon_attack");
        add("icon_flee");

        //TEST
        add("test_animation");
        add("test_character");
        add("test_jasper");
    }

    static std::shared_ptr<sf::Texture> getImageByName(std::string name) {
        auto res = images.find(name);
        if (res != images.end()) {
            return res->second; 
        }
        return nullptr;
    }
};

std::unordered_map<std::string, std::shared_ptr<sf::Texture>> ImageManager::images;


class AnimatedSprite{
    sf::Clock timer;

    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    
    sf::Vector2i currentCell;  
    sf::Vector2i maxCell;
    sf::Vector2i framePosition;
    sf::Vector2i frameSize;

    int fps;
    float timeToWait;

    sf::Time lastFrame;
    int loopCount;

public: 
    AnimatedSprite(std::shared_ptr<sf::Texture> textureSheet, sf::Vector2f position, sf::Vector2i maxCell, int fps):
    texture(textureSheet),
    sprite(*texture),
    currentCell(0, 0),
    maxCell(maxCell),
    frameSize(frameSize),
    fps(fps), 
    loopCount(0) {
        setPosition(position);
        frameSize.x = texture.get()->getSize().x / maxCell.x;
        frameSize.y = texture.get()->getSize().y / maxCell.y;

        timeToWait = 1.f / fps;
        lastFrame = sf::seconds(0);
    }

    sf::Sprite& play(std::optional<int> maxLoopCount = std::nullopt) {
        if (!timer.isRunning()) {
            timer.start();
        }

        if (maxLoopCount.has_value() && loopCount >= maxLoopCount.value()) {
            timer.reset();
        }

        if (timer.getElapsedTime().asSeconds() - lastFrame.asSeconds() > timeToWait || lastFrame == sf::seconds(0)) {
            framePosition.x = currentCell.x * frameSize.x;
            framePosition.y = currentCell.y * frameSize.y;

            sprite.setTextureRect(sf::IntRect(framePosition, frameSize));
            lastFrame = timer.getElapsedTime();

            currentCell.x += 1;
            if (currentCell.x >= maxCell.x) {
                currentCell.x = 0;
                currentCell.y += 1;
                
                if (currentCell.y >= maxCell.y) {
                    currentCell.y = 0;
                    if (maxLoopCount != std::nullopt) {
                        loopCount += 1;
                    }
                }
            }
        }

        return sprite;
    }

    void start() {
        timer.start();
    }

    void stop() {
        timer.reset();
    }

    void setPosition(sf::Vector2f position) {
        sprite.setPosition(position);
    }

    sf::Vector2f getPosition() {
        return sprite.getPosition();
    }

    void setScale(sf::Vector2f scale) {
        sprite.setScale(scale);
    }
};

class DrawingManager{
    static sf::RenderWindow* window;
    //static std::shared_ptr<sf::RenderWindow> window;
    
    static std::vector<std::pair<std::shared_ptr<float>, std::shared_ptr<sf::Sprite>>> toDraw;
    std::vector<std::pair<std::shared_ptr<float>, std::shared_ptr<sf::Sprite>>> toHide;

public:
    static void setWindow(sf::RenderWindow* w) {
        window = w;
    }

    static void add(std::shared_ptr<float> y, std::shared_ptr<sf::Sprite> sprite) {
        toDraw.push_back(std::pair(y, sprite));
    }

    static void draw() {
        std::sort(toDraw.begin(), toDraw.end(), [](auto& a, auto& b) {
            return *a.first < *b.first;
        });
        
        for (auto& s : toDraw) {
            std::cout<<s.first<<std::endl;
            window->draw(*s.second);
        }
        std::cout<<std::endl;
    }
};

sf::RenderWindow* DrawingManager::window;
std::vector<std::pair<std::shared_ptr<float>, std::shared_ptr<sf::Sprite>>> DrawingManager::toDraw;

class Clickable {
    sf::RectangleShape clickableRect;
    
public: 
    Clickable(sf::Vector2f size):
    clickableRect(size) {}

    void setRectScale(sf::Vector2f scale) {
        this->clickableRect.setScale(scale);
    }

    void setRectPosition(sf::Vector2f position) {
        this->clickableRect.setPosition(position);
    }

    sf::Vector2f getRectPosition() {
        return this->clickableRect.getPosition();
    }

    sf::FloatRect getBounds() {
        return this->clickableRect.getGlobalBounds();
    }

    void setRectColor(sf::Color color) {
        this->clickableRect.setOutlineThickness(3.f);
        this->clickableRect.setOutlineColor(color);
    }

    sf::RectangleShape getRect() {
        return this->clickableRect;
    }
};

class InteractionManager {
    static std::vector<std::shared_ptr<Clickable>> clickableObjects;
    static std::vector<std::shared_ptr<Clickable>> collidableObjects;

public:
    static void addToCollidable(std::shared_ptr<Clickable> obj) {
        collidableObjects.push_back(obj);
    }

    static void addToClickable(std::shared_ptr<Clickable> obj) {
        clickableObjects.push_back(obj);
    }

    static std::shared_ptr<Clickable> checkForInteractions(sf::Vector2f clickPosition) {
        for (const auto& c : clickableObjects) {
            if (c.get()->getBounds().contains(clickPosition) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && clickPosition != sf::Vector2f(0.f, 0.f)) {
                //std::cout<<"mors"<<std::endl;
                return c;
            }
        }
        return nullptr;
    }

    static std::shared_ptr<Clickable> checkForCollisions(Clickable obj) {
        for (const auto& c : collidableObjects) {
            if (c.get()->getBounds().findIntersection(obj.getBounds())) {
                return c;
            }
        }
        return nullptr;
    }

    static std::vector<std::shared_ptr<Clickable>> getAllClickable() {
        return clickableObjects;
    }

    static std::vector<std::shared_ptr<Clickable>> getAllCollidable() {
        return collidableObjects;
    }
};

std::vector<std::shared_ptr<Clickable>> InteractionManager::clickableObjects;
std::vector<std::shared_ptr<Clickable>> InteractionManager::collidableObjects;

class Button : public Clickable {
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

public:
    Button(std::shared_ptr<sf::Texture> txt): Clickable(sf::Vector2f(txt->getSize())),
    texture(txt),
    sprite(*texture) {
        sprite.setScale(sf::Vector2f(4.f, 4.f));
        this->setRectScale(sf::Vector2f(4.f, 4.f));
    }

    void setPosition(sf::Vector2f position) {
        this->sprite.setPosition(position);
        this->setRectPosition(position);
    }

    sf::Sprite& getSprite() {
        return this->sprite;
    }
};

class Room {

};

class MapManager {
    std::vector<char> worldMap;
    std::vector<Room> listOfRooms;
};


enum AbilityType {
    ATTACK,
    DEFENCE,
};

class Ability : public Clickable {
    std::string name;
    std::string description;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    AbilityType type;
    float value;

public:
    Ability(std::string name, std::string desc, AbilityType type, float value, std::shared_ptr<sf::Texture> txt): Clickable(sf::Vector2f(txt->getSize())),
    texture(txt),
    sprite(*texture),
    name(name),
    description(desc),
    type(type),
    value(value) {}

    float getValue() {
        return this->value;
    }

    void setScale(sf::Vector2f scale) {
        this->sprite.setScale(scale);
        this->setRectScale(scale);
    }

    void setPosition(sf::Vector2f position) {
        this->sprite.setPosition(position);
        this->setRectPosition(position);
    }

    sf::Sprite& getSprite() {
        return this->sprite;
    }
};

//moze jakas globalna lista ablilty i potem je brac z niej tylko


class Snail : public Clickable {
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;
    
    float maxHp;
    float currentHp;
    std::vector<std::shared_ptr<Ability>> listOfAbilities;

public:
    Snail(float maxHp, std::shared_ptr<sf::Texture> txt): Clickable(sf::Vector2f(txt->getSize())),
        texture(txt),
        sprite(*texture),
        maxHp(maxHp),
        currentHp(maxHp) {
        this->sprite.setScale(sf::Vector2f(4.f, 4.f));
       
        auto ability = std::make_shared<Ability>("Slap", "Slap enemy with your tail", AbilityType::ATTACK, 5.f, ImageManager::getImageByName("icon_attack"));
        InteractionManager::addToClickable(ability);
        this->listOfAbilities.push_back(ability);
        ability = std::make_shared<Ability>("Spit", "Spit on enemy with your slime", AbilityType::ATTACK, 3.f, ImageManager::getImageByName("icon_attack"));
        InteractionManager::addToClickable(ability);
        this->listOfAbilities.push_back(ability);
        ability = std::make_shared<Ability>("Hide", "Defend yourself with your shell", AbilityType::DEFENCE, 4.f, ImageManager::getImageByName("icon_defence"));
        InteractionManager::addToClickable(ability);
        this->listOfAbilities.push_back(ability);
    }

    std::vector<std::shared_ptr<Ability>>& getListOfAbilities() {
        return this->listOfAbilities;
    }

    sf::Sprite& getSprite() {
        return this->sprite;
    }

    void setScale(sf::Vector2f scale) {
        this->sprite.setScale(scale);
        this->setRectScale(scale);
    }

    void setPosition(sf::Vector2f position) {
        this->sprite.setPosition(position);
        this->setRectPosition(position);
    }

    std::string getName() {
        return this->name;
    }
};


class Player : public Clickable{
private:
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    sf::Vector2f currentPosition;
    sf::Vector2f movePosition;
    sf::Vector2f endPosition;

    std::vector<std::shared_ptr<Snail>> listOfSnails;

public: 
    Player(sf::Vector2f position, std::shared_ptr<sf::Texture> txt): Clickable(sf::Vector2f(txt->getSize())),
        texture(txt),
        sprite(*texture),
        currentPosition(position),
        endPosition(position) {
        //this->sprite.setScale(sf::Vector2f(2.f, 2.f));
        this->sprite.setScale(sf::Vector2f(0.25, 0.25));
        this->sprite.setPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->setRectScale(sf::Vector2f(3.f, 3.f));
        this->setRectPosition(sf::Vector2f(position.x - 48, position.y - 30));
        
        for (auto i = 0; i < 3; i++) {
            auto snail = std::make_shared<Snail>(100.f, ImageManager::getImageByName("default_snail"));
            InteractionManager::addToClickable(snail);
            listOfSnails.push_back(snail);
        }
    }
    
    void setPos(sf::Vector2f position) {
        sprite.setPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->setRectPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->currentPosition = position;
        this->endPosition = position;
    }

    void setCurrPos(sf::Vector2f position) {
        sprite.setPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->setRectPosition(sf::Vector2f(position.x - 48, position.y - 30));
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

    std::vector<std::shared_ptr<Snail>>& getListOfSnails() {
        return this->listOfSnails;
    }
};

class Enemy : public Clickable{
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    sf::Vector2f position;
    std::vector<std::shared_ptr<Snail>> listOfSnails;
    
public:
    Enemy(sf::Vector2f position, std::shared_ptr<sf::Texture> txt): Clickable(sf::Vector2f(txt->getSize())),
    texture(txt),
    sprite(*texture),
    position(position) {
        sprite.setScale(sf::Vector2f(3, 3));
        sprite.setPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->setRectScale(sf::Vector2f(3.f, 3.f));
        this->setRectPosition(sf::Vector2f(position.x - 48, position.y - 30));
        this->setRectColor(sf::Color::Red);

        for (auto i = 0; i < 3; i++) {
            auto snail = std::make_shared<Snail>(100.f, ImageManager::getImageByName("default_snail"));
            InteractionManager::addToClickable(snail);
            listOfSnails.push_back(snail);
        }
    }

    std::vector<std::shared_ptr<Snail>>& getListOfSnails() {
        return this->listOfSnails;
    }

    void setPos(sf::Vector2f position) {
        sprite.setPosition(sf::Vector2f(this->position.x - 48, this->position.y - 30));
        this->position = position;
        this->setRectPosition(sf::Vector2f(position.x - 48, position.y - 30));
    }

    sf::Vector2f getPos() {
        return this->position;
    }

    sf::Sprite getSprite() {
        return this->sprite;
    }
};


// enum Element {
//     Cold,
//     Lightning,
//     Fire,
// };

class Plant : public Clickable {
private: 
    std::string name;
    std::shared_ptr<sf::Texture> texture;
    sf::Sprite sprite;

    sf::Vector2f position;

public: 
    Plant(std::shared_ptr<sf::Texture> txt): Clickable(sf::Vector2f(txt->getSize())),
    texture(txt),
    sprite(*texture) {
        this->sprite.setScale(sf::Vector2f(4.f, 4.f));
        this->setRectScale(sf::Vector2f(4.f, 4.f));
    }

    void setPos(sf::Vector2f position) {
        sprite.setPosition(sf::Vector2f(position.x - 64, position.y - 64));
        this->position = position + sf::Vector2f(-64, -64);
        this->setRectPosition(sf::Vector2f(position.x - 64, position.y - 64));
    }

    sf::Vector2f getPos() {
        return this->position;
    }

    sf::Sprite& getSprite() {
        return this->sprite;
    }
};

// class Potion {
// private:
//     std::vector<Plant> ingridients();
// };


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
    int tilesInWidth = 12;
    int tilesInHeight = 7;
    unsigned int roomWidth = tileSize * tilesInWidth;
    unsigned int roomHeight = tileSize * tilesInHeight;

    auto window = sf::RenderWindow(sf::VideoMode({roomWidth , roomHeight}), "karcianka");
    window.setFramerateLimit(60);
    DrawingManager::setWindow(&window);

    Input input;
    input.setWindow(&window);
    
    ImageManager::load();

    //Player player(sf::Vector2f(roomWidth/2, roomHeight/2), ImageManager::getImageByName("goblin"));
    Player player(sf::Vector2f(roomWidth/2, roomHeight/2), ImageManager::getImageByName("goblin3d"));

    Enemy currentEnemy(sf::Vector2f(roomWidth/2 + tileSize*3, roomHeight/2), ImageManager::getImageByName("goblin"));
    InteractionManager::addToCollidable(std::make_shared<Enemy>(currentEnemy));

    std::shared_ptr<Snail> choosenSnail;

    //PLANTS
    std::shared_ptr<Plant> currentPlant;

    auto plant01 = std::make_shared<Plant>(ImageManager::getImageByName("plant01"));
    InteractionManager::addToClickable(plant01);
    plant01.get()->setPos(sf::Vector2f(50, 50));

    auto plant02 = std::make_shared<Plant>(ImageManager::getImageByName("plant01"));
    InteractionManager::addToClickable(plant02);
    plant02.get()->setPos(sf::Vector2f(100, 50));

    auto plant03 = std::make_shared<Plant>(ImageManager::getImageByName("plant01"));
    InteractionManager::addToClickable(plant03);
    plant03.get()->setPos(sf::Vector2f(150, 50));

    std::vector<std::shared_ptr<Plant>> listOfPlants;
    listOfPlants.push_back(plant01);
    listOfPlants.push_back(plant02);
    listOfPlants.push_back(plant03);

    //BUTTONS
    auto buttonFlee =  std::make_shared<Button>(ImageManager::getImageByName("icon_flee"));
    InteractionManager::addToClickable(buttonFlee);

    //Mode gameMode = Mode::Exploration;
    Mode gameMode = Mode::Crafting;
    //Mode gameMode = Mode::Battle;
    
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
    stoper.start();


    sf::Clock changeToExploration;
    changeToExploration.stop();
    bool isChangingToExploration = false;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution rand(1, 6);
    int dice_num;

    int counter = 0;
    sf::Clock counterTime;
    counterTime.start();

    AnimatedSprite animationTest(ImageManager::getImageByName("test_animation"), sf::Vector2f(roomWidth/2, roomHeight/2), sf::Vector2i(24, 4), 60);
    animationTest.setScale(sf::Vector2f(7.f, 2.f));
    AnimatedSprite animationTest2(ImageManager::getImageByName("test_character"), sf::Vector2f(roomWidth/2 - tileSize*4.5, roomHeight/2 - tileSize*2), sf::Vector2i(4, 4), 15);
    animationTest2.setScale(sf::Vector2f(3.f, 3.f));
    AnimatedSprite animationTest3(ImageManager::getImageByName("test_jasper"), sf::Vector2f(roomWidth/2 - tileSize*1.5, roomHeight/2 - tileSize*2.5), sf::Vector2i(9, 9), 15);

    DrawingManager::add(std::make_shared<float>(plant01->getPos().y), std::make_shared<sf::Sprite>(plant01->getSprite()));
    DrawingManager::add(std::make_shared<float>(plant02->getPos().y), std::make_shared<sf::Sprite>(plant02->getSprite()));
    DrawingManager::add(std::make_shared<float>(plant03->getPos().y), std::make_shared<sf::Sprite>(plant03->getSprite()));
    //DrawingManager::add(std::make_shared<float>(animationTest.getPosition().y), std::make_shared<sf::Sprite>(animationTest.play(5)));

    while (window.isOpen()) {
        if (stoper.getElapsedTime() > sf::milliseconds(1001)) {
            stoper.restart();
        }

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            } 
        }

        window.clear(sf::Color::Black);
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
            //         sf::Vector2f pos(sf::Vector2f(tileSize * col, tileSize * row + worldOffset.y));
            //         sf::Sprite tile(ImageManager::getImageByName("missing").get(), sf::IntRect(pos, sf::Vector2u(tileSize, tileSize)));
                    
            //         tile.setPosition(pos);
            //         window.draw(tile);
            //     }
            // }

            // for (int row = 0; row < tilesInHeight; row++) {
            //     for (int col = 0; col < tilesInWidth; col++) {
            //         //std::cout<<layout[col][row]<<"\n";
            //         if (layout[col][row] == 2) {
            //             //tile_path.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));
            //             //window.draw(tile_path);
            //         }
            //         if (layout[col][row] == 1) {
            //             //tile_gravel.setPosition(sf::Vector2f(tileSize * col + worldOffset.x, tileSize * row + worldOffset.y));  
            //             //window.draw(tile_gravel);
            //         }  
            //     }
            // }

            currentEnemy.setPos(sf::Vector2f((roomWidth/2 + tileSize*3) + worldOffset.x, (roomHeight/2) + worldOffset.y));
            
            window.draw(currentEnemy.getRect());
            window.draw(currentEnemy.getSprite());
            

            if (changeToExploration.getElapsedTime() > sf::milliseconds(100)) {
                //changeToExploration.reset();
                changeToExploration.restart();
                changeToExploration.stop();
            }

            if (changeToExploration.getElapsedTime() == sf::seconds(0)) {
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
                
                    player.setCurrY(roomHeight - tileSize/2);
                    player.setEndY(roomHeight - tileSize/2);
                }

                if (player.getCurrPos().y > window.getSize().y - 32) {
                    worldOffset.y -= roomHeight;
                
                    player.setCurrY(tileSize/2);
                    player.setEndY(tileSize/2);
                } 

                if (player.getCurrPos().x < 32) {
                    worldOffset.x += roomWidth;
                
                    player.setCurrX(roomWidth - tileSize/2);
                    player.setEndX(roomWidth - tileSize/2);
                }

                if (player.getCurrPos().x > window.getSize().x - 32) {
                    worldOffset.x -= roomWidth;
                    
                    player.setCurrX(tileSize/2);
                    player.setEndX(tileSize/2);
                }
            }
            
            window.draw(player.getRect());
            window.draw(player.getSprite());

            auto collidedEnemy = InteractionManager::checkForCollisions(player);
            if (collidedEnemy) {
                gameMode = Mode::Battle;
            }
        }

        if (gameMode == Mode::Battle) {
           
            player.getListOfSnails().at(0).get()->setPosition(sf::Vector2f(roomWidth/2 - tileSize*2, tileSize/2));
            player.getListOfSnails().at(0).get()->setScale(sf::Vector2f(4, 4));
            window.draw(player.getListOfSnails().at(0).get()->getSprite());
            player.getListOfSnails().at(1).get()->setPosition(sf::Vector2f(roomWidth/2 - tileSize*3.5, tileSize/2));
            player.getListOfSnails().at(1).get()->setScale(sf::Vector2f(4, 4));
            window.draw(player.getListOfSnails().at(1).get()->getSprite());
            player.getListOfSnails().at(2).get()->setPosition(sf::Vector2f(roomWidth/2 - tileSize*5, tileSize/2));
            player.getListOfSnails().at(2).get()->setScale(sf::Vector2f(4, 4));
            window.draw(player.getListOfSnails().at(2).get()->getSprite());

    
            currentEnemy.getListOfSnails().at(0).get()->setPosition(sf::Vector2f(roomWidth/2 + tileSize*2, tileSize/2));
            currentEnemy.getListOfSnails().at(0).get()->setScale(sf::Vector2f(-4, 4));
            window.draw(currentEnemy.getListOfSnails().at(0).get()->getSprite());
            currentEnemy.getListOfSnails().at(1).get()->setPosition(sf::Vector2f(roomWidth/2 + tileSize*3.5, tileSize/2));
            currentEnemy.getListOfSnails().at(1).get()->setScale(sf::Vector2f(-4, 4));
            window.draw(currentEnemy.getListOfSnails().at(1).get()->getSprite());
            currentEnemy.getListOfSnails().at(2).get()->setPosition(sf::Vector2f(roomWidth/2 + tileSize*5, tileSize/2));
            currentEnemy.getListOfSnails().at(2).get()->setScale(sf::Vector2f(-4, 4));
            window.draw(currentEnemy.getListOfSnails().at(2).get()->getSprite());

        
            sf::RectangleShape table(sf::Vector2f(roomWidth, 6*tileSize));
            table.setFillColor(sf::Color(20, 40, 120));
            table.setPosition(sf::Vector2f(0.0, tileSize*3));
            window.draw(table);


            for (auto s : player.getListOfSnails()) {
                if (s == InteractionManager::checkForInteractions(static_cast<sf::Vector2f>(input.getMousePosition()))) {
                    choosenSnail = s;
                }
            }

            if (choosenSnail != nullptr) {
                std::ostringstream a;
                a<<choosenSnail;
                window.draw(sf::Text(font, a.str(), 30));

                choosenSnail.get()->getListOfAbilities().at(0).get()->setPosition(sf::Vector2f(tileSize, roomHeight - tileSize*2));
                choosenSnail.get()->getListOfAbilities().at(0).get()->setScale(sf::Vector2f(4, 4));
                window.draw(choosenSnail.get()->getListOfAbilities().at(0).get()->getSprite());    

                choosenSnail.get()->getListOfAbilities().at(1).get()->setPosition(sf::Vector2f(tileSize*2.5, roomHeight - tileSize*2));
                choosenSnail.get()->getListOfAbilities().at(1).get()->setScale(sf::Vector2f(4, 4));
                window.draw(choosenSnail.get()->getListOfAbilities().at(1).get()->getSprite());  

                choosenSnail.get()->getListOfAbilities().at(2).get()->setPosition(sf::Vector2f(tileSize*4, roomHeight - tileSize*2));
                choosenSnail.get()->getListOfAbilities().at(2).get()->setScale(sf::Vector2f(4, 4));
                window.draw(choosenSnail.get()->getListOfAbilities().at(2).get()->getSprite());  
            }

            buttonFlee.get()->setPosition(sf::Vector2f(roomWidth - tileSize*2, roomHeight - tileSize*2));
            window.draw(buttonFlee.get()->getSprite());

            if (InteractionManager::checkForInteractions(static_cast<sf::Vector2f>(input.getMousePosition())) != nullptr) {
                window.draw(InteractionManager::checkForInteractions(static_cast<sf::Vector2f>(input.getMousePosition())).get()->getRect());
            }

            if (InteractionManager::checkForInteractions(static_cast<sf::Vector2f>(input.getMousePosition())) == buttonFlee) {
                gameMode = Mode::Exploration;
                player.setPos(sf::Vector2f(roomWidth/2, roomHeight/2));
                changeToExploration.start();
            }


            // sf::Text dice(font, std::to_string(dice_num));
            // dice.setFillColor(sf::Color(255, 255, 255));
            // dice.setPosition(sf::Vector2f(roomWidth/2, roomHeight - tileSize));
            //window.draw(dice);
        }

        if (gameMode == Mode::Crafting) {

            
         
            
            for (auto plant : listOfPlants) {
                if (plant == InteractionManager::checkForInteractions(static_cast<sf::Vector2f>(input.getMousePosition()))) {
                    currentPlant = plant;
                }
            }

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && InteractionManager::checkForInteractions(static_cast<sf::Vector2f>(input.getMousePosition())) != nullptr && currentPlant != nullptr) {
                currentPlant.get()->setPos(static_cast<sf::Vector2f>(input.getMousePosition()));
            }

            // window.draw(plant01->getSprite());
            // window.draw(plant02->getSprite());
            // window.draw(plant03->getSprite());

            // DrawingManager::add(std::make_shared<float>(plant01->getPos().y), std::make_shared<sf::Sprite>(plant01->getSprite()));
            // DrawingManager::add(std::make_shared<float>(plant02->getPos().y), std::make_shared<sf::Sprite>(plant02->getSprite()));
            // DrawingManager::add(std::make_shared<float>(plant03->getPos().y), std::make_shared<sf::Sprite>(plant03->getSprite()));
    
            window.draw(animationTest.play());

             if (clock.getElapsedTime() > sf::seconds(5)) {
                //animationTest.stop();
                ImageManager::remove("test_animation");
                window.draw(sf::Sprite(ImageManager::getImageByName("test_animation")));
            }
            //DrawingManager::add(std::make_shared<float>(animationTest.getPosition().y), std::make_shared<sf::Sprite>(animationTest.play(5)));
            DrawingManager::draw();

        }

        if (gameMode == Mode::Equipment) {

        }

        //std::cout<<changeToExploration.getElapsedTime().asMilliseconds()<<std::endl;

        window.display();
    }
}