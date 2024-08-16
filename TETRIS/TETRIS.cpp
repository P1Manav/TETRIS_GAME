#include <SFML/Graphics.hpp>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>

const int M = 20;
const int N = 10;

int field[M][N] = { 0 };

struct Point
{
    int x, y;
} a[4], b[4];

int figures[7][4] =
{
    1,3,5,7,
    2,4,5,7,
    3,5,4,6,
    3,5,4,7,
    2,3,5,7,
    3,5,7,6,
    2,3,4,5,
};

bool check()
{
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= N || a[i].y >= M) return false;
        else if (field[a[i].y][a[i].x]) return false;

    return true;
};

bool isGameOver()
{
    for (int j = 0; j < N; j++)
    {
        if (field[0][j] != 0)
        {
            return true;
        }
    }
    return false;
}

bool loadFromFileToMemory(const std::string& filename, std::vector<char>& buffer)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    buffer.resize(size);
    if (!file.read(buffer.data(), size))
    {
        std::cerr << "Error reading file: " << filename << std::endl;
        return false;
    }

    return true;
}

int main()
{
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(600, 600), "TETRIS!");

    sf::Texture t1, t2, t3;
    std::vector<char> buffer1, buffer2, buffer3;
    if (!loadFromFileToMemory("D:/Projects/TETRIS/Debug/images/tiles.png", buffer1) ||
        !t1.loadFromMemory(buffer1.data(), buffer1.size()))
    {
        std::cerr << "Error loading tiles.png" << std::endl;
        return -1;
    }
    if (!loadFromFileToMemory("D:/Projects/TETRIS/Debug/images/background.jpeg", buffer2) ||
        !t2.loadFromMemory(buffer2.data(), buffer2.size()))
    {
        std::cerr << "Error loading background.jpeg" << std::endl;
        return -1;
    }
    if (!loadFromFileToMemory("D:/Projects/TETRIS/Debug/images/frame.png", buffer3) ||
        !t3.loadFromMemory(buffer3.data(), buffer3.size()))
    {
        std::cerr << "Error loading frame.png" << std::endl;
        return -1;
    }

    sf::Sprite s(t1), background(t2), frame(t3);

    float backgroundX = (window.getSize().x - background.getLocalBounds().width) / 2;
    float backgroundY = (window.getSize().y - background.getLocalBounds().height) / 2;

    background.setPosition(backgroundX, backgroundY);

    float frameX = (window.getSize().x - frame.getLocalBounds().width) / 2 + 40;
    float frameY = (window.getSize().y - frame.getLocalBounds().height) / 2;

    frame.setPosition(frameX, frameY);

    sf::Font font;
    std::vector<char> fontBuffer;
    if (!loadFromFileToMemory("D:/Projects/TETRIS/Debug/Fonts/Roboto-Black.ttf", fontBuffer))
    {
        std::cerr << "Failed to load font file into memory." << std::endl;
        return -1;
    }
    if (!font.loadFromMemory(fontBuffer.data(), fontBuffer.size()))
    {
        std::cerr << "Failed to load font from memory." << std::endl;
        return -1;
    }

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over!\nPress any key to exit");
    gameOverText.setCharacterSize(64);
    gameOverText.setFillColor(sf::Color::White);

    sf::FloatRect textRect = gameOverText.getLocalBounds();
    gameOverText.setOrigin(textRect.left + textRect.width / 2.0f,
        textRect.top + textRect.height / 2.0f);
    gameOverText.setPosition(sf::Vector2f(window.getSize().x / 2.0f, window.getSize().y / 2.0f));

    int dx = 0;
    bool rotate = false;
    int colorNum = 1;
    float timer = 0, delay = 0.3;

    sf::Clock clock;
    bool gameOver = false;

    while (window.isOpen())
    {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();

            if (e.type == sf::Event::KeyPressed && gameOver)
                window.close();

            if (!gameOver)
            {
                if (e.type == sf::Event::KeyPressed)
                {
                    if (e.key.code == sf::Keyboard::Up)
                        rotate = true;
                    else if (e.key.code == sf::Keyboard::Left)
                        dx = -1;
                    else if (e.key.code == sf::Keyboard::Right)
                        dx = 1;
                }
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            delay = 0.05;

        if (!gameOver)
        {
            for (int i = 0; i < 4; i++)
            {
                b[i] = a[i];
                a[i].x += dx;
            }
            if (!check())
            {
                for (int i = 0; i < 4; i++)
                    a[i] = b[i];
            }

            if (rotate)
            {
                Point p = a[1];
                for (int i = 0; i < 4; i++)
                {
                    int x = a[i].y - p.y;
                    int y = a[i].x - p.x;
                    a[i].x = p.x - x;
                    a[i].y = p.y + y;
                }
                if (!check())
                {
                    for (int i = 0; i < 4; i++)
                        a[i] = b[i];
                }
            }

            if (timer > delay)
            {
                for (int i = 0; i < 4; i++)
                {
                    b[i] = a[i];
                    a[i].y += 1;
                }

                if (!check())
                {
                    for (int i = 0; i < 4; i++)
                        field[b[i].y][b[i].x] = colorNum;

                    if (isGameOver())
                        gameOver = true;

                    colorNum = 1 + rand() % 7;
                    int n = rand() % 7;
                    for (int i = 0; i < 4; i++)
                    {
                        a[i].x = figures[n][i] % 2;
                        a[i].y = figures[n][i] / 2;
                    }
                }

                timer = 0;
            }

            int k = M - 1;
            for (int i = M - 1; i > 0; i--)
            {
                int count = 0;
                for (int j = 0; j < N; j++)
                {
                    if (field[i][j])
                        count++;
                    field[k][j] = field[i][j];
                }
                if (count < N)
                    k--;
            }

            dx = 0;
            rotate = false;
            delay = 0.3;
        }

        window.clear(sf::Color::White);
        window.draw(background);

        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < N; j++)
            {
                if (field[i][j] == 0)
                    continue;

                s.setTextureRect(sf::IntRect(field[i][j] * 18, 0, 18, 18));
                s.setPosition(frame.getPosition().x + j * 18, frame.getPosition().y + i * 18);
                s.move(28, 31);
                window.draw(s);
            }
        }

        float blockX = frame.getPosition().x + (frame.getLocalBounds().width - N * 18) / 2 - 70;
        float blockY = frame.getPosition().y + (frame.getLocalBounds().height - M * 18) / 2 - 70;

        for (int i = 0; i < 4; i++)
        {
            s.setTextureRect(sf::IntRect(colorNum * 18, 0, 18, 18));
            s.setPosition(blockX + a[i].x * 18, blockY + a[i].y * 18);
            s.move(28, 31);
            window.draw(s);
        }

        window.draw(frame);

        if (gameOver)
        {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}
