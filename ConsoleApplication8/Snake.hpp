#pragma once

#include <SFML/Graphics.hpp>
#include <deque>

#include "GameObject.hpp"
#include "Types.hpp"

class Snake : public GameObject {
private:
    std::deque<sf::Vector2f> body;
    Direction currentDir;

public:
    Snake();

    void reset();
    Direction getDirection() const;
    void setDirection(Direction dir);
    sf::Vector2f getHead() const;
    const std::deque<sf::Vector2f>& getBody() const;

    void move(bool grow);
    bool checkCollision() const;
    void draw(sf::RenderWindow& window) override;
};
