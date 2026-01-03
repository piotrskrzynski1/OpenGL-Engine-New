#pragma once

#include <OPENGL/glad/glad.h>
#include <OPENGL/glm/glm.hpp>
#include <OPENGL/glm/gtc/type_ptr.hpp>

#include <string>

class Shader {
public:
    // The program ID
    unsigned int ID;

    // Constructor reads and builds the shader from file paths.
    // geometryPath is optional (empty string if not used).
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");

    // Destructor deletes the GL program
    ~Shader();

    // Use/activate the shader program
    void use() const;

    // Convenience uniform setters (overloads for common types)
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& v) const;
    void setVec2(const std::string& name, float x, float y) const;

    void setVec3(const std::string& name, const glm::vec3& v) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec4(const std::string& name, const glm::vec4& v) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;

    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    static constexpr const char* lightingModule = "shaders/TestShaders/Modules/Lighting.glsl";
    static constexpr const char* versionModule = "shaders/TestShaders/Modules/Version.glsl";
    std::string readFile(const std::string& path) const;
    void checkCompileErrors(GLuint object, const std::string& type) const;
};
