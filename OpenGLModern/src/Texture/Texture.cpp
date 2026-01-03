#include <Texture/Texture.hpp>
#include <iostream>

// Define this ONLY in one cpp file (like here) to include the implementation
#define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

Texture::Texture(const char* imagePath) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    std::cout << ID << std::endl;

    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image, create texture and generate mipmaps
    // OpenGL expects 0.0 y-axis on bottom, images usually have 0.0 at top. Flip it.
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    GLenum format = GL_RGBA;
    if (data) {
       
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            format = GL_RGB;
        }
        else if (nrChannels == 4)
            format = GL_RGBA;
        else
            std::cerr << "BAD TEXTURE FORMAT" << std::endl;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture: " << imagePath << std::endl;
    }

    stbi_image_free(data);
    this->unbind();
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}