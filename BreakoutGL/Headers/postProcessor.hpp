#ifndef postProcessor_hpp
#define postProcessor_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.hpp"
#include "spriteRenderer.hpp"
#include "shader.hpp"

// It is required to call BeginRender() before rendering the game
// and EndRender() after rendering the game for the class to work.
class PostProcessor {
public:
    Shader      postProcessingShader;
    Texture2D   texture;
    GLuint      width, height;
    GLboolean   confuse, chaos, shake;
    
    PostProcessor(Shader shader, GLuint width, GLuint height);
    void beginRender();
    void endRender();
    void render(GLfloat time);
private:
    GLuint MSFBO, FBO; // MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
    GLuint RBO; // RBO is used for multisampled color buffer
    GLuint VAO;
    void initRenderData();
};

#endif