#ifndef particleGenerator_hpp
#define particleGenerator_hpp

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "texture.hpp"
#include "gameObject.hpp"

struct Particle {
    glm::vec2 position, velocity;
    glm::vec4 color;
    GLfloat life;
    
    Particle() : position(0.0f), velocity(0.0f), color(1.0f), life(0.0f) { }
};

class ParticleGenerator {
public:
    ParticleGenerator(Shader shader, Texture2D texture, GLuint amount);
    void update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
    void draw();
private:
    std::vector<Particle> particles;
    GLuint amount;
    Shader shader;
    Texture2D texture;
    GLuint VAO;
    void init();
    GLuint firstUnusedParticle();
    void respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

#endif