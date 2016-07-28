#include "gameObject.hpp"

GameObject::GameObject()
: position(0, 0), size(1, 1), velocity(0.0f), color(1.0f), rotation(0.0f), sprite(), isSolid(false), destroyed(false) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
: position(pos), size(size), velocity(velocity), color(color), rotation(0.0f), sprite(sprite), isSolid(false), destroyed(false) { }

void GameObject::draw(SpriteRenderer &renderer) {
    renderer.drawSprite(this->sprite, this->position, this->size, this->rotation, this->color);
}

// Ball

BallObject::BallObject()
: GameObject(), radius(12.5f), stuck(true), sticky(GL_FALSE), passThrough(GL_FALSE) { }

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite)
:  GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.0f), velocity), radius(radius), stuck(true), sticky(GL_FALSE), passThrough(GL_FALSE) { }

glm::vec2 BallObject::move(GLfloat dt, GLuint window_width) {

    if (!this->stuck)
    {
        this->position += this->velocity * dt;
        
        // Check if outside window bounds and if so, reverse velocity and restore at correct position
        if (this->position.x <= 0.0f)
        {
            this->velocity.x *= -1.0f;
            this->position.x = 0.0f;
        }
        else if (this->position.x + this->size.x >= window_width)
        {
            this->velocity.x *= -1.0f;
            this->position.x = window_width - this->size.x;
        }
        if (this->position.y <= 0.0f)
        {
            this->velocity.y *= -1.0f;
            this->position.y = 0.0f;
        }
    }
    return this->position;
}

void BallObject::reset(glm::vec2 position, glm::vec2 velocity)
{
    this->position = position;
    this->velocity = velocity;
    this->stuck = GL_TRUE;
    this->sticky = GL_FALSE;
    this->passThrough = GL_FALSE;
}