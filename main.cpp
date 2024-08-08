#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <list>
#include <vector>
using namespace std;
using namespace sf;

#pragma region PlayerSettings
static const float PLAYERSIZE = 10;
static const int SPEED = 10;
static Vector2i _dir(0, -1 * SPEED);
static const Color PLAYER_COLOR = Color(0, 204, 0, 255);
static vector<RectangleShape> _snake;
static bool _isDead = false;
#pragma endregion

#pragma region EngineSettings
static const int WIDTH = 640;
static const int HEIGHT = 480;
static const int MOVEMENT[4]{ Keyboard::A, Keyboard::D, Keyboard::W, Keyboard::S };
static const string PATHTOFONT = "fibberish.ttf";
static sf::RenderWindow _root(VideoMode(WIDTH, HEIGHT), "Engine");
static bool _game_started = false;
static const int FONT_SIZE = 42;
static Text _gameover_text;
#pragma endregion


static Vector2f _zeroPosition = Vector2f(WIDTH / 2, HEIGHT / 2);
static int _interval;
static int _score;
static Text _scoreText;
static Font font;



static sf::RectangleShape _point;

#pragma region Prototypes
void Graphics();
void MovePlayer();
void GeneratePoint();
RectangleShape CreateShape(Color clr, Vector2f position);
void CheckKeyMovement(int key);
void GameOverSettings();
void Score();
void Pause();
void PauseSettings();
void MenuSettings();
void Menu();
void Restart();
#pragma endregion
#pragma region PauseSettings
static bool _is_paused = false;
static RectangleShape _background;
static Text _pauseTxt;
#pragma endregion
#pragma region MenuSettings
static Text _title_text;
static Text _play_text;
#pragma endregion

#pragma region Threads
static Thread _movementThread{ MovePlayer };
static Thread _generatePoint{ GeneratePoint };
#pragma endregion

int main()
{
    try
    {
        font.loadFromFile(PATHTOFONT);
    }
    catch (...)
    {
        cout << "Font is missing!" << endl;
        return -1;
    }
    PauseSettings();
    MenuSettings();
    GameOverSettings();
    _root.setVerticalSyncEnabled(true);
    Graphics();
}

void Menu()
{
    _root.draw(_title_text);
    _root.draw(_play_text);
}
void MenuSettings()
{
    _title_text.setFont(font);
    _title_text.setString("SNAKE");
    _title_text.setCharacterSize(70);
    _title_text.setFillColor(Color::Yellow);
    _title_text.setPosition(Vector2f(WIDTH / 2 - 2.5 * (70 / 2), 0));

    _play_text.setFont(font);
    _play_text.setString("Play");
    _play_text.setCharacterSize(FONT_SIZE);
    _play_text.setFillColor(Color::Yellow);
    _play_text.setPosition(Vector2f(_zeroPosition.x - (2 * (FONT_SIZE / 2)), _zeroPosition.y));
}

void Game()
{
    _root.draw(_point);
    for (RectangleShape shape : _snake)
        _root.draw(shape);
    if (_isDead)
        _root.draw(_gameover_text);
    Score();
    if (_is_paused)
    {
        _root.draw(_background);
        _root.draw(_pauseTxt);
    }
}

void Graphics()
{
    while (_root.isOpen())
    {
        Event ev;
        while (_root.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
                _root.close();
            if (ev.type == Event::KeyPressed)
                CheckKeyMovement(ev.key.code);
        }
        _root.clear(Color(96, 96, 96, 255));
        if (!_game_started)
            Menu();
        else
            Game();
        _root.display();
    }
    _movementThread.terminate();
}

void Restart()
{
    _movementThread.terminate();
    _snake.clear();
    _score = 0;
    _interval = 100;
    _isDead = false;
    _is_paused = false;
    _snake.push_back(CreateShape(PLAYER_COLOR, Vector2f(_zeroPosition.x - PLAYERSIZE / 2, _zeroPosition.y - PLAYERSIZE / 2)));
    GeneratePoint();
    _movementThread.launch();
}

void GeneratePoint()
{
    Vector2f playerPos = _snake[0].getPosition();
    _point.setSize(Vector2f(PLAYERSIZE, PLAYERSIZE));
    _point.setFillColor(Color::Black);
    srand(time(nullptr));
    int x = playerPos.x, y = playerPos.y;
    bool found_place = false;
    while(!found_place)
    {
        x = 1 + rand() % (WIDTH - (int)PLAYERSIZE);
        y = 1 + rand() % (HEIGHT - (int)PLAYERSIZE);
        if (playerPos.x != x && playerPos.y != y && x % 5 == 0 && y % 5 == 0)
        {
            for (RectangleShape shape : _snake)
            {
                if (shape.getPosition().x != x && shape.getPosition().y != y)
                {
                    found_place = true;
                    break;
                }
            }
        }
    }
    _point.setPosition(x, y);
}

void Pause()
{
    _is_paused = !_is_paused;
    if (_is_paused)
        _movementThread.terminate();
    else
        _movementThread.launch();
}
void PauseSettings()
{
    _background.setSize(Vector2f(WIDTH, HEIGHT));
    _background.setFillColor(Color(0, 0, 0, 128));
    _background.setPosition(Vector2f(_zeroPosition.x - WIDTH / 2, _zeroPosition.y - HEIGHT / 2));
    
    _pauseTxt.setFont(font);
    _pauseTxt.setString("Pause");
    _pauseTxt.setCharacterSize(FONT_SIZE);
    _pauseTxt.setFillColor(Color::Yellow);
    _pauseTxt.setPosition(Vector2f(_zeroPosition.x - (2.5 * (FONT_SIZE / 2)), _zeroPosition.y));
}

void Score()
{
    _scoreText.setFont(font);
    string value = "Score: " + to_string(_score);
    _scoreText.setString(value);
    _scoreText.setCharacterSize(FONT_SIZE);
    _scoreText.setFillColor(Color::Yellow);
    _scoreText.setPosition(Vector2f(WIDTH / 2 - (value.length() / 2) * (FONT_SIZE / 2), 0));
    _root.draw(_scoreText);
}

void GameOverSettings()
{
    _gameover_text.setFont(font);
    _gameover_text.setString("Game over!");
    _gameover_text.setCharacterSize(FONT_SIZE);
    _gameover_text.setFillColor(Color::Red);
    _gameover_text.setPosition(_zeroPosition.x - 5.5 * (FONT_SIZE / 2), _zeroPosition.y);
    
}

RectangleShape CreateShape(Color clr, Vector2f position)
{
    RectangleShape shape;
    shape.setSize(Vector2f(PLAYERSIZE, PLAYERSIZE));
    shape.setFillColor(clr);
    shape.setPosition(position);
    return shape;
}

void MovePlayer()
{
    while (true)
    {
        RectangleShape head = _snake[0];
        Vector2f playerPos = head.getPosition();
        Vector2f newPos(playerPos.x + _dir.x, playerPos.y + _dir.y);
        if (newPos.x <= 0)
            newPos.x = WIDTH;
        else if (newPos.x >= WIDTH)
            newPos.x = 0;

        if (newPos.y <= 0)
            newPos.y = HEIGHT;
        else if (newPos.y >= HEIGHT)
            newPos.y = 0;
        head.setPosition(newPos);
        _snake[0] = head;

        if (abs(_point.getPosition().x - newPos.x) < PLAYERSIZE && abs(_point.getPosition().y - newPos.y) < PLAYERSIZE)
        {
            _score += 10;
            if(_interval > 50)
                _interval--;
            _generatePoint.launch();
            _snake.push_back(CreateShape(Color(0, PLAYER_COLOR.g - _snake.size(), 0, 255), Vector2f(newPos.x - _dir.x, newPos.y - _dir.y)));
        }
        Vector2f prevPos;
        if (_snake.size() > 1)
        {
            for (int i = 1; i < _snake.size(); i++)
            {
                if (_snake[i].getPosition() == _snake[0].getPosition())
                {
                    _isDead = true;
                    return;
                }                 
                RectangleShape shape = _snake[i];
                prevPos = shape.getPosition();
                shape.setPosition(playerPos);
                playerPos = prevPos;
                _snake[i] = shape;
            }
        }       
        sleep(sf::milliseconds(_interval));
    }
}

void CheckKeyMovement(int key)
{
    if (key == Keyboard::Escape)
    {
        if (!_game_started)
            _root.close();
        else if (_isDead)
            _game_started = false;
        else
            Pause();
    }
    else if (key == Keyboard::Enter)
    {
        if (!_game_started)
        {
            _game_started = true;
            Restart();
        }
    }
    else if (key == Keyboard::R)
    {
        if (_game_started)
            Restart();
    }
    else if (key == Keyboard::Q)
    {
        if (_is_paused)
            _game_started = false;
    }
    else if (!_is_paused)
    {
        for (int i = 0; i < 4; i++)
        {
            if (MOVEMENT[i] == key)
            {
                if (_dir.x == 0 && (i == 0 || i == 1))
                    _dir = Vector2i(1 * SPEED, 0);
                else if (_dir.y == 0 && (i == 2 || i == 3))
                    _dir = Vector2i(0, 1 * SPEED);
                else
                    return;
                if ((i + 1) % 2 != 0)
                    _dir = Vector2i(_dir.x * -1, _dir.y * -1);
            }
        }
    }
}