#pragma once
#include <OPENGL/glad/glad.h>
#include <string>

class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;

    // Constructor: loads and creates the texture
    Texture(const char* imagePath);

    // Bind the texture to a specific slot (default is 0)
    void bind(unsigned int slot = 0) const;

    // Unbind (optional cleanup)
    void unbind() const;
};