#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

#include "Food.hpp"
#include "Snake.hpp"
#include "Types.hpp"

class Game {
private:
    sf::RenderWindow window;
    Snake snake;
    Food food;

    sf::Font font;
    bool fontLoaded = false;
    sf::Text titleText{ font, "" };
    sf::Text menuHintText{ font, "" };
    sf::Text hudText{ font, "" };
    sf::Text gameOverText{ font, "" };
    sf::Text gameOverHintText{ font, "" };

    std::thread logicThread;
    std::mutex mtx;

    std::atomic<bool> isRunning{ true };
    Direction nextDir{ RIGHT };
    GameState state{ GameState::MENU };
    int score = 0;
    int highScore = 0;

    bool loadFont();
    void setupUi();
    void startNewGame();
    void logicUpdate();
    void handleEvents();
    void drawGrid();
    void render();

public:
    Game();
    ~Game();
    void run();
};
