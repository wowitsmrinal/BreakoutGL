#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "glitter.hpp"

Game Breakout(mWidth, mHeight);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main(int argc, char * argv[]) {
    
    GLFWwindow* window = setupWindowing("Breakout", mWidth, mHeight);
    glfwSetKeyCallback(window, key_callback);
    
    Breakout.init();
    Breakout.state = GAME_ACTIVE;
    
    // DeltaTime variables
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window)) {
        
        // Calculate delta time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        
        Breakout.processInput(deltaTime);
        Breakout.update(deltaTime);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.render();
        glfwSwapBuffers(window);
    }
    
    ResourceManager::clear();
    glfwTerminate();
    return EXIT_SUCCESS;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            Breakout.keys[key] = GL_TRUE;
        else if (action == GLFW_RELEASE)
            Breakout.keys[key] = GL_FALSE;
    }
}