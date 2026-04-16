#pragma once

#include <SFML/Graphics.hpp>

class GameObject {
protected:
    sf::Vector2f position;
    sf::RectangleShape shape;

public:
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual ~GameObject() = default;

    sf::Vector2f getPosition() const { return position; }

    void setPosition(const sf::Vector2f& pos) {
        position = pos;
        shape.setPosition(position);
    }
};
