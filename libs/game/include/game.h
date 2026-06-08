#ifndef GAME_H
#define GAME_H
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gamelevel.h>
#include <resource_manager.h>
#include <BallObject.h>
#include <ParticleGenerator.h>
#include <PostProcessor.h>


enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum Direction {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> Collision;

const float PLAYER_RADIUS = 18.5f;
const float PLAYER_VELOCITY(300.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(150.0f, 150.0f);
const float BALL_RADIUS = 12.5f;

class Game
{
public:
    GameState               State;	
    bool                    Keys[1024];
    bool                    KeysProcessed[1024];
    unsigned int            Width, Height;
    std::vector<GameLevel>  Levels;
    unsigned int            Level;
    unsigned int            Lives;


    Game(unsigned int width, unsigned int height);
    ~Game();
    void Init();
    void Cleanup();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    void ResetLevel();
    void ResetPlayer();
};

#endif