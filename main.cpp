#include <iostream>
#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"
#include <vector>
#include <cmath>
#include <cstdlib>

class Bullet {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;

    Bullet(float x, float y, float targetX, float targetY, float speed) {
        shape.setRadius(5.f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(x, y);

        sf::Vector2f direction(targetX - x, targetY - y);
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        direction /= length;

        velocity = direction * speed;
    }

    void move() {
        shape.move(velocity);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

class Enemy {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;

    Enemy(float startX, float startY, float centerX, float centerY, float speed) {
        shape.setRadius(15.f);
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(startX, startY);

        sf::Vector2f direction(centerX - startX, centerY - startY);
        float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        direction /= length;

        velocity = direction * speed;
    }

    void move() {
        shape.move(velocity);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

void restartGame(sf::RenderWindow& window, bool& gameOver, int& baseLife, std::vector<Enemy>& enemies, std::vector<Bullet>& bullets, sf::CircleShape& circ) {
    gameOver = false;
    baseLife = 10;
    enemies.clear();
    bullets.clear();
    circ.setPosition(600, 350);

    for (int i = 0; i < 5; ++i) {
        int spawnSide = rand() % 4;
        float startX, startY;

        if (spawnSide == 0) startX = -30, startY = rand() % 800;
        else if (spawnSide == 1) startX = 1230, startY = rand() % 800;
        else if (spawnSide == 2) startX = rand() % 1200, startY = -30;
        else startX = rand() % 1200, startY = 830;

        enemies.push_back(Enemy(startX, startY, 600, 400, 2.f));
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Base Defense");
    window.setFramerateLimit(60);

    sf::RectangleShape base;
    sf::Vector2f posibase(500, 280);
    base.setPosition(posibase);
    base.setSize(sf::Vector2f(200, 200));
    base.setFillColor(sf::Color::Green);

    sf::CircleShape circ(10.f);
    circ.setFillColor(sf::Color::Red);

    sf::Vector2f CirclePosition(posibase.x + base.getSize().x / 2, posibase.y + base.getSize().y / 2);
    circ.setPosition(CirclePosition);

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    float bulletSpeed = 10.f;
    sf::Vector2f targetPosition;

    bool movingToTarget = false;
    sf::Vector2f center(600, 400);

    srand(time(0));

    auto spawnEnemy = [&]() {
        int spawnSide = rand() % 4;
        float startX, startY;

        if (spawnSide == 0) startX = -30, startY = rand() % 800;
        else if (spawnSide == 1) startX = 1230, startY = rand() % 800;
        else if (spawnSide == 2) startX = rand() % 1200, startY = -30;
        else startX = rand() % 1200, startY = 830;

        enemies.push_back(Enemy(startX, startY, center.x, center.y, 2.f));
        };

    for (int i = 0; i < 5; ++i) {
        spawnEnemy();
    }

    int baseLife = 10;
    bool gameOver = false;

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "fonte nao carregada. Usando fonte padrao do SFML.\n";
        if (!font.loadFromFile("C:\\Windows\\Fonts\\arial.ttf")) {
            std::cout << "falhou.\n";
            return -1;
        }
    }

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Fim de Jogo");
    gameOverText.setCharacterSize(50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(450, 350);

    sf::Text restartText;
    restartText.setFont(font);
    restartText.setString("Pressione 'R' para reiniciar ou 'ESC' para sair.");
    restartText.setCharacterSize(30);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(400, 450);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        if (gameOver) {
            window.clear();
            window.draw(gameOverText);
            window.draw(restartText);
            window.display();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                restartGame(window, gameOver, baseLife, enemies, bullets, circ);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
            continue;
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            targetPosition = sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(window).x),
                static_cast<float>(sf::Mouse::getPosition(window).y));
            movingToTarget = true;
        }

        if (movingToTarget) {
            sf::Vector2f direction = targetPosition - circ.getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            if (length > 1) {
                direction /= length;
                circ.move(direction * 3.f);
            }
            else {
                movingToTarget = false;
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
            bullets.push_back(Bullet(circ.getPosition().x + circ.getRadius() * 2, circ.getPosition().y + circ.getRadius(),
                static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y), bulletSpeed));
        }

        for (auto& bullet : bullets) {
            bullet.move();
        }

        for (auto it = enemies.begin(); it != enemies.end();) {
            bool bulletHit = false;
            for (auto& bullet : bullets) {
                if (it->getBounds().intersects(bullet.getBounds())) {
                    it = enemies.erase(it);
                    bulletHit = true;
                    break;
                }
            }

            if (!bulletHit) {
                it->move();
                ++it;
            }
        }

        for (auto it = enemies.begin(); it != enemies.end();) {
            if (it->getBounds().intersects(base.getGlobalBounds())) {
                baseLife -= 1;
                it = enemies.erase(it);
            }
            else {
                ++it;
            }
        }

        if (baseLife <= 0) {
            gameOver = true;
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](Bullet& b) { return b.shape.getPosition().x > 1200; }), bullets.end());

        if (rand() % 100 < 2) {
            spawnEnemy();
        }

        window.clear();
        window.draw(base);
        window.draw(circ);

        for (auto& bullet : bullets) {
            window.draw(bullet.shape);
        }

        for (auto& enemy : enemies) {
            window.draw(enemy.shape);
        }

        window.display();
    }

    return 0;
}