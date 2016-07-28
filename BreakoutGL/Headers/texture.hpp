#ifndef texture_hpp
#define texture_hpp
#pragma once

#include <glad/glad.h>

class Texture2D {
public:
    GLuint ID;
    GLuint width, height;
    GLuint internalFormat;
    GLuint imageFormat;
    
    // Texture configuration
    GLuint wrap_s;
    GLuint wrap_t;
    GLuint filter_min;
    GLuint filter_max;
    
    Texture2D();
    void generate(GLuint width, GLuint height, unsigned char* data);
    void bind() const;
};

#endif
