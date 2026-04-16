#include "Food.hpp"

#include <algorithm>

#include "Constants.hpp"

Food::Food() : rng(std::random_device{}()) {
    shape.setSize(sf::Vector2f(GRID_SIZE, GRID_SIZE));
    shape.setFillColor(sf::Color(220, 50, 50));
    respawn({});
}

void Food::respawn(const std::deque<sf::Vector2f>& snakeBody) {
    std::uniform_int_distribution<int> distX(0, WIDTH / GRID_SIZE - 1);
    std::uniform_int_distribution<int> distY(0, HEIGHT / GRID_SIZE - 1);

    sf::Vector2f candidate;
    do {
        candidate = sf::Vector2f(
            static_cast<float>(distX(rng) * GRID_SIZE),
            static_cast<float>(distY(rng) * GRID_SIZE));
    } while (std::any_of(snakeBody.begin(), snakeBody.end(),
                         [&](const sf::Vector2f& segment) { return segment == candidate; }));

    setPosition(candidate);
}

void Food::draw(sf::RenderWindow& window) { window.draw(shape); }
