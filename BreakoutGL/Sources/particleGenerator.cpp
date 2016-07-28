#include "particleGenerator.hpp"

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount)
: shader(shader), texture(texture), amount(amount) {
    this->init();
}

void ParticleGenerator::update(GLfloat dt, GameObject &object, GLuint newParticles, glm::vec2 offset)
{
    // Add new particles
    for (GLuint i = 0; i < newParticles; ++i) {
        int unusedParticle = this->firstUnusedParticle();
        this->respawnParticle(this->particles[unusedParticle], object, offset);
    }
    
    // Update all particles
    for (GLuint i = 0; i < this->amount; ++i) {
        Particle &p = this->particles[i];
        p.life -= dt; // reduce life
        if (p.life > 0.0f)
        {	// particle is alive, thus update
            p.position -= p.velocity * dt;
            p.color.a -= dt * 2.5;
        }
    }
}

void ParticleGenerator::draw()
{
    // Use additive blending to give it a 'glow' effect
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->shader.activate();
    
    for (Particle particle : this->particles) {
        if (particle.life > 0.0f) {
            this->shader.setVector2f("offset", particle.position);
            this->shader.setVector4f("color", particle.color);
            this->texture.bind();
            glBindVertexArray(this->VAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::init()
{
    // Set up mesh and attribute properties
    GLuint VBO;
    GLfloat particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(this->VAO);
    
    // Fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    
    // Set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);
    
    // Create this->amount default particle instances
    particles.resize(this->amount);
}

// Stores the index of the last particle used (for quick access to next dead particle)
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::firstUnusedParticle()
{
    // First search from last used particle, this will usually return almost instantly
    for (GLuint i = lastUsedParticle; i < this->amount; ++i) {
        if (this->particles[i].life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    
    // Otherwise, do a linear search
    for (GLuint i = 0; i < lastUsedParticle; ++i) {
        if (this->particles[i].life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    
    // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator::respawnParticle(Particle &particle, GameObject &object, glm::vec2 offset) {
    GLfloat random = ((rand() % 100) - 50) / 10.0f;
    GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
    particle.position = object.position + random + offset;
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.life = 1.0f;
    particle.velocity = object.velocity * 0.1f;
}