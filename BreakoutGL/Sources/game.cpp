#include "game.hpp"
#include "resourceManager.hpp"
#include "particleGenerator.hpp"
#include "postProcessor.hpp"
#include "spriteRenderer.hpp"

#include <iostream>

// Game-related State data
SpriteRenderer      *Renderer;
GameObject          *Player;
BallObject          *Ball;
ParticleGenerator   *Particles;
PostProcessor       *Effects;
GLfloat             ShakeTime = 0.0f;

// Game Asset Paths
std::string shaderDir   = PROJECT_SOURCE_DIR "/BreakoutGL/Shaders/";
std::string textureDir  = PROJECT_SOURCE_DIR "/BreakoutGL/Textures/";
std::string levelDir    = PROJECT_SOURCE_DIR "/BreakoutGL/Levels/";

GameLevel levelLoadHelper(std::string levelName, Game *game) {
    GameLevel level;
    level.load((levelDir+levelName).c_str(), game->width, game->height * 0.5);
    return level;
}

void levelSetLoadHelper(std::vector<std::string> levelNames, Game *game) {
    for (int i = 0; i < levelNames.size(); i++)
        game->levels.push_back(levelLoadHelper(levelNames[i], game));
}

Game::Game(GLuint width, GLuint height)
: state(GAME_ACTIVE), keys(), width(width), height(height) {
}

Game::~Game() {
    delete Renderer;
    delete Player;
    delete Ball;
    delete Particles;
    delete Effects;
}

void Game::init() {
    
    // Textures
    ResourceManager::loadTexture((textureDir+"awesomeface.png"          ).c_str(), GL_TRUE,  "face");
    ResourceManager::loadTexture((textureDir+"background.jpg"           ).c_str(), GL_FALSE, "background");
    ResourceManager::loadTexture((textureDir+"awesomeface.png"          ).c_str(), GL_TRUE,  "face");
    ResourceManager::loadTexture((textureDir+"block.png"                ).c_str(), GL_FALSE, "block");
    ResourceManager::loadTexture((textureDir+"block_solid.png"          ).c_str(), GL_FALSE, "block_solid");
    ResourceManager::loadTexture((textureDir+"paddle.png"               ).c_str(), GL_TRUE,  "paddle");
    ResourceManager::loadTexture((textureDir+"particle.png"             ).c_str(), GL_TRUE,  "particle");
    ResourceManager::loadTexture((textureDir+"powerup_chaos.png"        ).c_str(), GL_TRUE,  "tex_chaos");
    ResourceManager::loadTexture((textureDir+"powerup_confuse.png"      ).c_str(), GL_TRUE,  "tex_confuse");
    ResourceManager::loadTexture((textureDir+"powerup_increase.png"     ).c_str(), GL_TRUE,  "tex_size");
    ResourceManager::loadTexture((textureDir+"powerup_passthrough.png"  ).c_str(), GL_TRUE,  "tex_passthrough");
    ResourceManager::loadTexture((textureDir+"powerup_speed.png"        ).c_str(), GL_TRUE,  "tex_speed");
    ResourceManager::loadTexture((textureDir+"powerup_sticky.png"       ).c_str(), GL_TRUE,  "tex_sticky");

    // Shaders
    Shader spriteShader = ResourceManager::loadShader((shaderDir+"sprite.vs").c_str(), (shaderDir+"sprite.fs").c_str(), nullptr, "sprite");
    Shader particleShader = ResourceManager::loadShader((shaderDir+"particle.vs").c_str(), (shaderDir+"particle.fs").c_str(), nullptr, "particle");
    Shader ppShader = ResourceManager::loadShader((shaderDir+"postprocess.vs").c_str(), (shaderDir+"postprocess.fs").c_str(), nullptr, "postprocess");
    
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
    spriteShader.activate().setInteger("sprite", 0);
    spriteShader.setMatrix4("projection", projection);
    particleShader.activate().setInteger("sprite", 0);
    particleShader.setMatrix4("projection", projection);
    
    // Render Controls
    Renderer = new SpriteRenderer(spriteShader);
    Particles = new ParticleGenerator(particleShader, ResourceManager::getTexture("particle"), 500);
    Effects = new PostProcessor(ppShader, this->width, this->height);
    
    // Levels
    levelSetLoadHelper({"one.lvl", "two.lvl", "three.lvl", "four.lvl"}, this);
    this->level = 0;
    
    // Game objects
    glm::vec2 playerPos = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
    Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));
    
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));
}

GLboolean Game::checkCollision(GameObject &one, GameObject &two) // AABB - AABB collision testing
{
    // Collision x-axis?
    bool collisionX = one.position.x + one.size.x >= two.position.x &&
    two.position.x + two.size.x >= one.position.x;
    
    // Collision y-axis?
    bool collisionY = one.position.y + one.size.y >= two.position.y &&
    two.position.y + two.size.y >= one.position.y;
    
    // Collision only if on both axes
    return collisionX && collisionY;
}

Collision Game::checkCollision(BallObject &one, GameObject &two) // Circle - AABB collision
{
    // Get center point circle
    glm::vec2 center(one.position + one.radius);
    
    // Calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.size.x / 2, two.size.y / 2);
    glm::vec2 aabb_center(
        two.position.x + aabb_half_extents.x,
        two.position.y + aabb_half_extents.y
    );
    
    // Get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    
    // Add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    
    // Retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    
    if (glm::length(difference) <= one.radius)
        return std::make_tuple(GL_TRUE, vectorDirection(difference), difference);
    else
        return std::make_tuple(GL_FALSE, UP, glm::vec2(0, 0));
}

Direction Game::vectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++)
    {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void activatePowerUp(PowerUp &powerUp)
{
    // Initiate a powerup based type of powerup
    if (powerUp.type == "speed") {
        Ball->velocity *= 1.2;
    }
    else if (powerUp.type == "sticky") {
        Ball->sticky = GL_TRUE;
        Player->color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.type == "pass-through") {
        Ball->passThrough = GL_TRUE;
        Ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.type == "pad-size-increase") {
        Player->size.x += 50;
    }
    else if (powerUp.type == "confuse") {
        if (!Effects->chaos)
            Effects->confuse = GL_TRUE; // Only activate if chaos wasn't already active
    }
    else if (powerUp.type == "chaos") {
        if (!Effects->confuse)
            Effects->chaos = GL_TRUE;
    }
}

void Game::doCollisions() {
    for (GameObject &box : this->levels[this->level].bricks) {
        if (!box.destroyed) {
            Collision collision = checkCollision(*Ball, box);
            if (std::get<0>(collision)) {
                
                if (!box.isSolid) {
                    box.destroyed = GL_TRUE;
                    this->spawnPowerUps(box);
                }
                else {   // if block is solid, enable shake effect
                    ShakeTime = 0.05f;
                    Effects->shake = true;
                }
                
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if (!(Ball->passThrough && !box.isSolid)) {
                    if (dir == LEFT || dir == RIGHT) // Horizontal collision
                    {
                        Ball->velocity.x *= -1.0f;
                        GLfloat penetration = Ball->radius - std::abs(diff_vector.x);
                        if (dir == LEFT)
                            Ball->position.x += penetration; // Move ball to right
                        else
                            Ball->position.x -= penetration; // Move ball to left;
                    }
                    else // Vertical collision
                    {
                        Ball->velocity.y *= -1.0f; // Reverse vertical velocity
                        GLfloat penetration = Ball->radius - std::abs(diff_vector.y);
                        if (dir == UP)
                            Ball->position.y -= penetration; // Move ball back up
                        else
                            Ball->position.y += penetration; // Move ball back down
                    }
                }
            }
        }
        
    for (PowerUp &powerUp : this->PowerUps)
        {
            if (!powerUp.destroyed)
            {
                if (powerUp.position.y >= this->height)
                    powerUp.destroyed = GL_TRUE;
                if (checkCollision(*Player, powerUp))
                {	// Collided with player, now activate powerup
                    activatePowerUp(powerUp);
                    powerUp.destroyed = GL_TRUE;
                    powerUp.activated = GL_TRUE;
                }
            }
        }
    }
    
    Collision result = checkCollision(*Ball, *Player);
    if (!Ball->stuck && std::get<0>(result)){
        
        // Check where it hit the board, and change velocity based on where it hit the board
        GLfloat centerBoard = Player->position.x + Player->size.x / 2;
        GLfloat distance = (Ball->position.x + Ball->radius) - centerBoard;
        GLfloat percentage = distance / (Player->size.x / 2);
        
        // Move accordingly
        GLfloat strength = 2.0f;
        glm::vec2 oldVelocity = Ball->velocity;
        Ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        Ball->velocity.y = -1 * fabs(Ball->velocity.y);
        Ball->velocity = glm::normalize(Ball->velocity) * glm::length(oldVelocity);
        
        Ball->stuck = Ball->sticky;
    }
}

GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type) {
    for (const PowerUp &powerUp : powerUps) {
        if (powerUp.activated)
            if (powerUp.type == type)
                return GL_TRUE;
    }
    return GL_FALSE;
}

void Game::updatePowerUps(GLfloat dt) {
    for (PowerUp &powerUp : this->PowerUps) {
        powerUp.position += powerUp.velocity * dt;
        if (powerUp.activated) {
            powerUp.duration -= dt;
            
            if (powerUp.duration <= 0.0f) {
                powerUp.activated = GL_FALSE;
                
                if (powerUp.type == "sticky") {
                    if (!isOtherPowerUpActive(this->PowerUps, "sticky")) {
                        Ball->sticky = GL_FALSE;
                        Player->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "pass-through") {
                    if (!isOtherPowerUpActive(this->PowerUps, "pass-through")) {
                        Ball->passThrough = GL_FALSE;
                        Ball->color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.type == "confuse") {
                    if (!isOtherPowerUpActive(this->PowerUps, "confuse")) {
                        Effects->confuse = GL_FALSE;
                    }
                }
                else if (powerUp.type == "chaos") {
                    if (!isOtherPowerUpActive(this->PowerUps, "chaos")) {
                        Effects->chaos = GL_FALSE;
                    }
                }
            }
        }
    }
    
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
                                        [](const PowerUp &powerUp) { return powerUp.destroyed && !powerUp.activated; }
                                        ), this->PowerUps.end());
}

void Game::update(GLfloat dt) {
    Ball->move(dt, this->width);
    this->doCollisions();
    Particles->update(dt, *Ball, 2, glm::vec2(Ball->radius / 2));
    this->updatePowerUps(dt);
    
    if (ShakeTime > 0.0f) {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->shake = false;
    }
    
    if (Ball->position.y >= this->height) {
        this->resetLevel();
        this->resetPlayer();
    }
}

void Game::processInput(GLfloat dt) {
    if (this->state == GAME_ACTIVE)
    {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        
        // Move paddle
        if (this->keys[GLFW_KEY_A])
        {
            if (Player->position.x >= 0) {
                Player->position.x -= velocity;
                if (Ball->stuck)
                    Ball->position.x -= velocity;
            }
        }
        if (this->keys[GLFW_KEY_D])
        {
            if (Player->position.x <= this->width - Player->size.x) {
                Player->position.x += velocity;
                if (Ball->stuck)
                    Ball->position.x += velocity;
            }
        }
        if (this->keys[GLFW_KEY_SPACE])
            Ball->stuck = false;
    }
}

void Game::render() {
    if (this->state == GAME_ACTIVE)
    {
        Effects->beginRender();
        
        glViewport(0, 0, width, height);
        Texture2D texBackground = ResourceManager::getTexture("background");
        Renderer->drawSprite(texBackground, glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f);
        this->levels[this->level].draw(*Renderer);
        Player->draw(*Renderer);
        Particles->draw();
        Ball->draw(*Renderer);
        for (PowerUp &powerUp : this->PowerUps)
            if (!powerUp.destroyed)
                powerUp.draw(*Renderer);
        
        Effects->endRender();
        glViewport(0, 0, width * 2.0f, height * 2.0f); // Fixed the bug, still don't know why or how.
        Effects->render(glfwGetTime());
    }
}

void Game::resetLevel()
{
    switch (level) {
        case 0: levels[level] = levelLoadHelper("one.lvl",   this); break;
        case 1: levels[level] = levelLoadHelper("two.lvl",   this); break;
        case 2: levels[level] = levelLoadHelper("three.lvl", this); break;
        case 3: levels[level] = levelLoadHelper("four.lvl",  this); break;
    }
}

void Game::resetPlayer()
{
    Player->size = PLAYER_SIZE;
    Player->position = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
    Ball->reset(Player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
    
    Effects->chaos = Effects->confuse = GL_FALSE;
    Ball->passThrough = Ball->sticky = GL_FALSE;
    Player->color = glm::vec3(1.0f);
    Ball->color = glm::vec3(1.0f);
}

GLboolean shouldSpawn(GLuint chance)
{
    GLuint random = rand() % chance;
    return random == 0;
}
void Game::spawnPowerUps(GameObject &block) {
    if (shouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, ResourceManager::getTexture("tex_speed")));
    if (shouldSpawn(75))
        this->PowerUps.push_back(PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position, ResourceManager::getTexture("tex_sticky")));
    if (shouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position, ResourceManager::getTexture("tex_pass")));
    if (shouldSpawn(75))
        this->PowerUps.push_back(PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.position, ResourceManager::getTexture("tex_size")));
    if (shouldSpawn(15)) // Negative powerups should spawn more often
        this->PowerUps.push_back(PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.position, ResourceManager::getTexture("tex_confuse")));
    if (shouldSpawn(15))
        this->PowerUps.push_back(PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.position, ResourceManager::getTexture("tex_chaos")));
}