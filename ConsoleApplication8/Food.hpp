#pragma once

#include <SFML/Graphics.hpp>
#include <deque>
#include <random>

#include "GameObject.hpp"

class Food : public GameObject {
private:
    std::mt19937 rng;

public:
    Food();
    void respawn(const std::deque<sf::Vector2f>& snakeBody);
    void draw(sf::RenderWindow& window) override;
};
                                 