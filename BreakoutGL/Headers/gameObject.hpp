#ifndef gameObject_hpp
#define gameObject_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.hpp"
#include "spriteRenderer.hpp"

class GameObject {
public:
    // Object state
    glm::vec2   position, size, velocity;
    glm::vec3   color;
    GLfloat     rotation;
    GLboolean   isSolid;
    GLboolean   destroyed;
    
    // Render state
    Texture2D   sprite;
    
    // Constructor(s)
    GameObject();
    GameObject(
        glm::vec2 pos,
        glm::vec2 size,
        Texture2D sprite,
        glm::vec3 color = glm::vec3(1.0f),
        glm::vec2 velocity = glm::vec2(0.0f, 0.0f)
    );
    
    // Draw sprite
    virtual void draw(SpriteRenderer &renderer);
};

class BallObject : public GameObject {
public:
    // Ball state
    GLfloat   radius;
    GLboolean stuck;
    GLboolean sticky, passThrough;
    
    // Constructor(s)
    BallObject();
    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);
    
    glm::vec2 move(GLfloat dt, GLuint window_width); // Returns new position
    void      reset(glm::vec2 position, glm::vec2 velocity);
};

class PowerUp : public GameObject {
public:
    // PowerUp State
    std::string type;
    GLfloat     duration;
    GLboolean   activated;
    
    // Constructor
    PowerUp(std::string type,
            glm::vec3   color,
            GLfloat     duration,
            glm::vec2   position,
            Texture2D   texture)
    : GameObject(
            position,
            glm::vec2(60,20),
            texture,
            color,
            glm::vec2(0.0f, 150.0f)
        ),
            type(type), duration(duration), activated()
    { }
};

#endif