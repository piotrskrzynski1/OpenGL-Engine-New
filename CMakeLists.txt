cmake_minimum_required(VERSION 3.24)
project(OpenGLModern)

set(CMAKE_CXX_STANDARD 20)

# ==========================================
# 1. SETUP & DEPENDENCIES
# ==========================================
include(FetchContent)
find_package(OpenGL REQUIRED)

# --- GLFW ---
FetchContent_Declare(
    glfw 
    GIT_REPOSITORY https://github.com/glfw/glfw.git 
    GIT_TAG master
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# --- Assimp (ADDED) ---
FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v5.4.3  # Using a specific tag is safer than master
)

# Optimization: Disable Assimp tools, tests, and exporters to speed up build
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE) # We usually only need to Import, not Export
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)

# --- GoogleTest ---
set(CMAKE_TLS_VERIFY OFF) 

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)
set(CMAKE_TLS_VERIFY ON)

# ==========================================
# 2. DEFINITIONS (FIXED PATHS HERE)
# ==========================================

# Source is inside "OpenGLModern"
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenGLModern")

# Includes are at the ROOT level (Sibling to OpenGLModern)
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/OpenGLModern/Includes")

# Tests are in "Testing", not "Tests"
set(TEST_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/Testing")

# ==========================================
# 3. ENGINE CORE LIBRARY
# ==========================================

file(GLOB_RECURSE ENGINE_SOURCES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.c")

# FIX: Look for headers in the correct INCLUDE_DIR
file(GLOB_RECURSE ENGINE_HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")

# Exclude main from the library
list(FILTER ENGINE_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

add_library(EngineCore STATIC ${ENGINE_SOURCES} ${ENGINE_HEADERS})

# Link Assimp to the EngineCore
target_link_libraries(EngineCore PRIVATE glfw OpenGL::GL assimp)

# FIX: Point to the correct Include directory
target_include_directories(EngineCore PUBLIC "${INCLUDE_DIR}")
target_include_directories(EngineCore PUBLIC "${SOURCE_DIR}") 

# ==========================================
# 4. GAME EXECUTABLE
# ==========================================

add_executable(GameApp "${SOURCE_DIR}/src/main.cpp")
target_link_libraries(GameApp PRIVATE EngineCore glfw assimp)

# Asset Copying
add_custom_command(TARGET GameApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:GameApp>/Shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCE_DIR}/Shaders" "$<TARGET_FILE_DIR:GameApp>/Shaders"
    
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:GameApp>/Textures"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SOURCE_DIR}/Textures" "$<TARGET_FILE_DIR:GameApp>/Textures"
)

# ==========================================
# 5. TEST EXECUTABLE
# ==========================================

enable_testing()

if(EXISTS "${TEST_DIR}")
    # Finds mainTests.cpp inside /Testing
    file(GLOB_RECURSE TEST_SOURCES "${TEST_DIR}/*.cpp")

    add_executable(UnitTests ${TEST_SOURCES})

    # Link GTest Main so you don't need a main() function
    target_link_libraries(UnitTests PRIVATE 
        EngineCore 
        GTest::gtest_main
        glfw
        assimp
        # Assimp is linked transitively via EngineCore, but explicit linking is okay too
    )

    # Allow tests to access headers from the root Includes folder
    target_include_directories(UnitTests PRIVATE "${INCLUDE_DIR}")

    include(GoogleTest)
    gtest_discover_tests(UnitTests)
endif()