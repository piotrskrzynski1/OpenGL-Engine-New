#include <Shader/Shader.hpp>

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>


Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
    // 1. Retrieve the source code from file paths
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);
    std::string geometryCode;
    if (!geometryPath.empty()) {
        geometryCode = readFile(geometryPath);
    }

    // Read the common Lighting Module
    std::string lightCode = readFile(lightingModule);

    // =========================================================================
    // HELPER LAMBDA: Safely injects code after the #version tag
    // =========================================================================
    auto injectLib = [](std::string& shaderCode, const std::string& libCode) {
        // Find where "#version" starts
        size_t versionPos = shaderCode.find("#version");

        if (versionPos == std::string::npos) {
            // Case A: No version tag found. Just prepend the lib code.
            shaderCode = libCode + "\n" + shaderCode;
        }
        else {
            // Case B: Version tag found. We must insert AFTER it.

            // Find the end of the line where #version is defined
            size_t endOfVersionLine = shaderCode.find('\n', versionPos);

            if (endOfVersionLine == std::string::npos) {
                // The file ends immediately after #version (rare but possible)
                shaderCode += "\n" + libCode + "\n";
            }
            else {
                // Insert the library code starting at the next line
                // We add extra \n to be safe against missing newlines
                shaderCode.insert(endOfVersionLine + 1, "\n" + libCode + "\n");
            }
        }
    };
    // =========================================================================

    // 2. Inject the lighting code into Vertex and Fragment shaders
    // Note: We do NOT simple concatenate (light + vertex) because #version must stay at the top!
    injectLib(vertexCode, lightCode);
    injectLib(fragmentCode, lightCode);

    // Convert to C-strings for OpenGL
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 3. Compile Shaders
    unsigned int vertex, fragment, geometry;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Geometry Shader (if present)
    if (!geometryPath.empty()) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (!geometryPath.empty()) {
        glAttachShader(ID, geometry);
    }
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Cleanup
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (!geometryPath.empty()) {
        glDeleteShader(geometry);
    }
}

Shader::~Shader()
{
    if (ID) {
        glDeleteProgram(ID);
    }
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& v) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(v));
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

std::string Shader::readFile(const std::string& path) const
{
    if (!std::filesystem::exists(path)) {
        std::cerr << "Shader file does not exist: " << path << std::endl;
        return std::string();
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return std::string();
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void Shader::checkCompileErrors(GLuint object, const std::string& type) const
{
    GLint success;
    GLchar infoLog[1024];
    if (type == "PROGRAM") {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }
    }
    else { // shader
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }
}
