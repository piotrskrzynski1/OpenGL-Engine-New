#include <gtest/gtest.h>
#include <OPENGL/glad/glad.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <fstream>
#include "Shader/Shader.hpp" // Twoja klasa

namespace fs = std::filesystem;

// 1. Fixture: Zarządza środowiskiem OpenGL i plikami tymczasowymi
class ShaderTestEnv : public ::testing::Test {
protected:
    // Uruchamiane RAZ dla wszystkich testów (ustawienie OpenGL)
    static void SetUpTestSuite() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW" << std::endl;
            return;
        }
        // Ukryte okno, tylko po to, by mieć kontekst OpenGL
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        GLFWwindow* window = glfwCreateWindow(640, 480, "TestWindow", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return;
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD" << std::endl;
        }
    }

    static void TearDownTestSuite() {
        glfwTerminate();
    }

    // Uruchamiane PRZED każdym pojedynczym testem
    void SetUp() override {
        // Tworzymy strukturę katalogów, której wymaga Twoja klasa
        fs::create_directories("shaders/TestShaders/Modules");
        fs::create_directories("temp_shaders");

        // Tworzymy dummy plik Lighting.glsl, bo klasa go sztywno wymaga
        createFile("shaders/TestShaders/Modules/Lighting.glsl", 
            "void setupLights() { /* dummy lighting */ }\n");
        
        // Tworzymy dummy plik Version.glsl (jeśli jest potrzebny)
        createFile("shaders/TestShaders/Modules/Version.glsl", 
            "#version 330 core\n");
    }

    // Uruchamiane PO każdym teście
    void TearDown() override {
        // Sprzątamy pliki
        fs::remove_all("shaders");
        fs::remove_all("temp_shaders");
    }

    // Helper do tworzenia plików
    void createFile(const std::string& path, const std::string& content) {
        std::ofstream out(path);
        out << content;
        out.close();
    }
};

// 2. Testy właściwe

// Sprawdza, czy shader kompiluje się poprawnie przy dobrym kodzie
TEST_F(ShaderTestEnv, CompilesSuccessfullyWithValidCode) {
    // Arrange
    std::string vertPath = "temp_shaders/test.vert";
    std::string fragPath = "temp_shaders/test.frag";

    createFile(vertPath, 
        "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main() { gl_Position = vec4(aPos, 1.0); setupLights(); }" // setupLights z lighting module
    );

    createFile(fragPath, 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() { FragColor = vec4(1.0); setupLights(); }"
    );

    // Act
    Shader shader(vertPath, fragPath);

    // Assert
    // Sprawdzamy czy ID programu jest większe od 0 (czyli OpenGL utworzył obiekt)
    EXPECT_GT(shader.ID, 0);
    
    // Sprawdźmy czy program jest poprawny (zlinkowany)
    GLint success;
    glGetProgramiv(shader.ID, GL_LINK_STATUS, &success);
    EXPECT_TRUE(success) << "Shader program linking failed";
}

// Sprawdza zachowanie, gdy plik nie istnieje
TEST_F(ShaderTestEnv, HandlesMissingFiles) {
    // Twoja klasa wypisuje błąd na cerr, ale nie rzuca wyjątku w obecnej formie.
    // Testujemy czy ID zostanie utworzone (powinno, ale linkowanie pewnie padnie).
    
    // Act
    Shader shader("missing.vert", "missing.frag");

    // Assert
    // W obecnej implementacji readFile zwraca pusty string, a glCompileShader próbuje to kompilować.
    // Oczekujemy, że linkowanie się NIE uda.
    GLint success;
    glGetProgramiv(shader.ID, GL_LINK_STATUS, &success);
    EXPECT_FALSE(success) << "Program should not link with missing files";
}

// Sprawdza ustawianie Uniformów
TEST_F(ShaderTestEnv, SetsUniformsWithoutCrashing) {
    // Arrange: Tworzymy poprawny shader
    std::string vertPath = "temp_shaders/uniform.vert";
    std::string fragPath = "temp_shaders/uniform.frag";
    createFile(vertPath, "#version 330 core\n layout (location = 0) in vec3 aPos; void main(){gl_Position=vec4(aPos,1.0);}");
    createFile(fragPath, "#version 330 core\n out vec4 Color; uniform vec3 uColor; void main(){Color=vec4(uColor,1.0);}");
    
    Shader shader(vertPath, fragPath);
    shader.use();

    // Act & Assert
    // Jeśli uniform nie istnieje, OpenGL zazwyczaj ignoruje wywołanie (nie crashuje),
    // ale warto sprawdzić czy funkcja się wykonuje.
    EXPECT_NO_THROW(shader.setVec3("uColor", 1.0f, 0.5f, 0.2f));
    EXPECT_NO_THROW(shader.setBool("nonExistent", true)); // To też nie powinno wywalić programu
}