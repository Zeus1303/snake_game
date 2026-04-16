#include "Game.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <vector>

#include "Constants.hpp"

bool Game::loadFont() {
    const std::vector<std::string> paths = {
        "arial.ttf",
        "Arial.ttf",
        "font.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/tahoma.ttf",
    };

    for (const auto& path : paths) {
        if (std::filesystem::exists(path) && font.openFromFile(path)) {
            std::cout << "Loaded font: " << path << "\n";
            return true;
        }
    }
    std::cerr << "Warning: Could not load font, text UI disabled.\n";
    return false;
}

void Game::setupUi() {
    fontLoaded = loadFont();
    if (!fontLoaded) return;

    titleText.setCharacterSize(72);
    titleText.setStyle(sf::Text::Bold);
    titleText.setString("SNAKE");
    titleText.setFillColor(sf::Color(0, 220, 0));
    {
        const sf::FloatRect bounds = titleText.getLocalBounds();
        titleText.setOrigin(
            { bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
        titleText.setPosition({ WIDTH / 2.f, 150.f });
    }

    menuHintText.setCharacterSize(24);
    menuHintText.setString("ENTER: Start   |   ESC: Exit");
    menuHintText.setFillColor(sf::Color::White);
    {
        const sf::FloatRect bounds = menuHintText.getLocalBounds();
        menuHintText.setOrigin(
            { bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
        menuHintText.setPosition({ WIDTH / 2.f, 300.f });
    }

    hudText.setCharacterSize(22);
    hudText.setFillColor(sf::Color::White);
    hudText.setPosition({ 10.f, 8.f });

    gameOverText.setCharacterSize(56);
    gameOverText.setStyle(sf::Text::Bold);
    gameOverText.setString("GAME OVER");
    gameOverText.setFillColor(sf::Color(230, 70, 70));
    {
        const sf::FloatRect bounds = gameOverText.getLocalBounds();
        gameOverText.setOrigin(
            { bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
        gameOverText.setPosition({ WIDTH / 2.f, HEIGHT / 2.f - 70.f });
    }

    gameOverHintText.setCharacterSize(22);
    gameOverHintText.setString("R: Choi lai   |   M: Menu");
    gameOverHintText.setFillColor(sf::Color(220, 220, 220));
    {
        const sf::FloatRect bounds = gameOverHintText.getLocalBounds();
        gameOverHintText.setOrigin(
            { bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
        gameOverHintText.setPosition({ WIDTH / 2.f, HEIGHT / 2.f + 30.f });
    }
}

void Game::startNewGame() {
    std::lock_guard<std::mutex> lock(mtx);
    snake.reset();
    food.respawn(snake.getBody());
    score = 0;
    nextDir = RIGHT;
    state = GameState::PLAYING;
}

void Game::logicUpdate() {
    while (isRunning.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(mtx);

        if (state != GameState::PLAYING) continue;

        snake.setDirection(nextDir);
        sf::Vector2f nextHead = snake.getHead();
        switch (nextDir) {
            case UP: nextHead.y -= GRID_SIZE; break;
            case DOWN: nextHead.y += GRID_SIZE; break;
            case LEFT: nextHead.x -= GRID_SIZE; break;
            case RIGHT: nextHead.x += GRID_SIZE; break;
        }

        const bool grow = (nextHead == food.getPosition());
        if (grow) {
            ++score;
            highScore = std::max(highScore, score);
            food.respawn(snake.getBody());
        }

        snake.move(grow);

        if (snake.checkCollision()) {
            state = GameState::GAMEOVER;
        }
    }
}

void Game::handleEvents() {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            isRunning.store(false);
            window.close();
            return;
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                isRunning.store(false);
                window.close();
                return;
            }

            if (state == GameState::MENU) {
                if (keyPressed->code == sf::Keyboard::Key::Enter) startNewGame();
                continue;
            }

            if (state == GameState::GAMEOVER) {
                if (keyPressed->code == sf::Keyboard::Key::R) startNewGame();
                else if (keyPressed->code == sf::Keyboard::Key::M) state = GameState::MENU;
                continue;
            }

            std::lock_guard<std::mutex> lock(mtx);
            const Direction currentDir = snake.getDirection();
            if (keyPressed->code == sf::Keyboard::Key::Up && currentDir != DOWN) nextDir = UP;
            else if (keyPressed->code == sf::Keyboard::Key::Down && currentDir != UP) nextDir = DOWN;
            else if (keyPressed->code == sf::Keyboard::Key::Left && currentDir != RIGHT) nextDir = LEFT;
            else if (keyPressed->code == sf::Keyboard::Key::Right && currentDir != LEFT) nextDir = RIGHT;
        }
    }
}

void Game::drawGrid() {
    sf::RectangleShape line;
    line.setFillColor(sf::Color(255, 255, 255, 12));

    line.setSize({ static_cast<float>(WIDTH), 1.f });
    for (int y = 0; y < HEIGHT; y += GRID_SIZE) {
        line.setPosition({ 0.f, static_cast<float>(y) });
        window.draw(line);
    }
    line.setSize({ 1.f, static_cast<float>(HEIGHT) });
    for (int x = 0; x < WIDTH; x += GRID_SIZE) {
        line.setPosition({ static_cast<float>(x), 0.f });
        window.draw(line);
    }
}

void Game::render() {
    window.clear(sf::Color(30, 30, 30));
    std::lock_guard<std::mutex> lock(mtx);

    if (state == GameState::MENU) {
        drawGrid();
        if (fontLoaded) {
            window.draw(titleText);
            window.draw(menuHintText);
        } else {
            sf::RectangleShape panel({ 420.f, 180.f });
            panel.setFillColor(sf::Color(60, 60, 60));
            panel.setOutlineThickness(3.f);
            panel.setOutlineColor(sf::Color(0, 180, 0));
            panel.setPosition({ WIDTH / 2.f - 210.f, HEIGHT / 2.f - 90.f });
            window.draw(panel);
        }
    } else {
        drawGrid();
        GameObject* objects[] = { &food, &snake };
        for (auto* obj : objects) obj->draw(window);

        if (fontLoaded) {
            hudText.setString("Score: " + std::to_string(score) + "   |   Best: " +
                              std::to_string(highScore));
            window.draw(hudText);
        }
    }

    if (state == GameState::GAMEOVER) {
        sf::RectangleShape overlay({ static_cast<float>(WIDTH), static_cast<float>(HEIGHT) });
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);

        if (fontLoaded) {
            sf::Text finalScore(font, "");
            finalScore.setCharacterSize(28);
            finalScore.setFillColor(sf::Color::Yellow);
            finalScore.setString("Score: " + std::to_string(score));
            {
                const sf::FloatRect bounds = finalScore.getLocalBounds();
                finalScore.setOrigin(
                    { bounds.position.x + bounds.size.x / 2.f, bounds.position.y + bounds.size.y / 2.f });
                finalScore.setPosition({ WIDTH / 2.f, HEIGHT / 2.f - 10.f });
            }

            window.draw(gameOverText);
            window.draw(finalScore);
            window.draw(gameOverHintText);
        }
    }

    window.display();
}

Game::Game()
    : window(sf::VideoMode({ static_cast<unsigned>(WIDTH), static_cast<unsigned>(HEIGHT) }),
             "Snake Game SFML3") {
    window.setFramerateLimit(60);
    setupUi();
    logicThread = std::thread(&Game::logicUpdate, this);
}

Game::~Game() {
    isRunning.store(false);
    if (logicThread.joinable()) logicThread.join();
}

void Game::run() {
    while (window.isOpen()) {
        handleEvents();
        render();
    }
}
