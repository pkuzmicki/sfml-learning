#define INPUT_H

#include <SFML/Graphics.hpp>

class Input {
    public:
        void updateKeys();
        void updateMouse();
        void setWindow(sf::RenderWindow* window);

        bool get_isUp()  { return isUp; }
        bool get_isDown()  { return isDown; }
        bool get_isLeft()  { return isLeft; }
        bool get_isRight()  { return isRight; }
        sf::Vector2i getMousePosition() {return mousePosition;}

    private:
        sf::RenderWindow* window;
        bool isUp = false;
        bool isDown = false;
        bool isLeft = false;
        bool isRight = false;
        sf::Vector2i mousePosition;
    };