#include "Snake.hpp"

#include "Constants.hpp"

Snake::Snake() {
    shape.setSize(sf::Vector2f(GRID_SIZE, GRID_SIZE));
    shape.setOutlineThickness(-1.f);
    shape.setOutlineColor(sf::Color(20, 20, 20));
    reset();
}

void Snake::reset() {
    body.clear();
    body.push_back(sf::Vector2f(WIDTH / 2.f, HEIGHT / 2.f));
    body.push_back(sf::Vector2f(WIDTH / 2.f - GRID_SIZE, HEIGHT / 2.f));
    body.push_back(sf::Vector2f(WIDTH / 2.f - 2 * GRID_SIZE, HEIGHT / 2.f));
    position = body.front();
    currentDir = RIGHT;
}

Direction Snake::getDirection() const { return currentDir; }

void Snake::setDirection(Direction dir) { currentDir = dir; }

sf::Vector2f Snake::getHead() const { return body.front(); }

const std::deque<sf::Vector2f>& Snake::getBody() const { return body; }

void Snake::move(bool grow) {
    sf::Vector2f newHead = body.front();
    switch (currentDir) {
        case UP: newHead.y -= GRID_SIZE; break;
        case DOWN: newHead.y += GRID_SIZE; break;
        case LEFT: newHead.x -= GRID_SIZE; break;
        case RIGHT: newHead.x += GRID_SIZE; break;
    }
    body.push_front(newHead);
    position = newHead;
    if (!grow) body.pop_back();
}

bool Snake::checkCollision() const {
    const sf::Vector2f head = body.front();
    if (head.x < 0 || head.x >= WIDTH || head.y < 0 || head.y >= HEIGHT) return true;
    for (size_t i = 1; i < body.size(); ++i) {
        if (head == body[i]) return true;
    }
    return false;
}

void Snake::draw(sf::RenderWindow& window) {
    for (size_t i = 0; i < body.size(); ++i) {
        shape.setFillColor(i == 0 ? sf::Color(0, 210, 0) : sf::Color(0, 160, 0));
        shape.setPosition(body[i]);
        window.draw(shape);
    }
}
