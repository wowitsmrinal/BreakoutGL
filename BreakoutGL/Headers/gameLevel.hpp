/*
 
 Levels are stored as plaintext files with space + newline separated matrix of numbers
 
 = 0: no brick, an empty space within the level.
 = 1: a solid brick, a brick that cannot be destroyed.
 > 1: a destroyable brick; each number only differs in color.
 
 Example:
 1 1 1 1 1 1
 2 2 0 0 2 2
 3 3 4 4 3 3
 
*/


#ifndef gameLevel_hpp
#define gameLevel_hpp

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.hpp"
#include "spriteRenderer.hpp"
#include "resourceManager.hpp"

class GameLevel {
public:
    std::vector<GameObject> bricks;
    GameLevel() { }
    void load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
    void draw(SpriteRenderer &renderer);
    GLboolean isCompleted();
private:
    void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif