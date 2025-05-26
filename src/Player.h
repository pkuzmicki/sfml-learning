#define PLAYER_H

#include <SFML/Graphics.hpp>

class Player {
    public:
        sf::Vector2f playerPosition;
        
    private:
        sf::Vector2f spritePosition;
        void setSpiritePosition();
};