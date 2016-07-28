#ifndef game_hpp
#define game_hpp
#pragma once

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "gameLevel.hpp"

enum gameState {
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

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const GLfloat BALL_RADIUS = 12.5f;

class Game {
public:
    gameState  state;
    GLboolean  keys[1024];
    GLuint     width, height;
    GLuint     level;
    
    std::vector<GameLevel>  levels;
    std::vector<PowerUp>    PowerUps;
    
    Game(GLuint width, GLuint height);
    ~Game();
    void init(); // (load all shaders/textures/levels)
    void processInput(GLfloat dt); // Game Loop
    void doCollisions();
    void update(GLfloat dt); 
    void render();
    void spawnPowerUps(GameObject &block);
    void updatePowerUps(GLfloat dt);
    
    void resetLevel();
    void resetPlayer();
    
    static GLboolean checkCollision(GameObject &one, GameObject &two); // AABB - AABB collision testing
    static Collision checkCollision(BallObject &one, GameObject &two); // Circle - AABB collision testing
    static Direction vectorDirection(glm::vec2 target);
};

#endif