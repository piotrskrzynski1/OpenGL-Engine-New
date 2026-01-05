#pragma once
#include "Component.hpp"
#include <Shader/Shader.hpp>
#include <Texture/Texture.hpp>

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <memory>

// A simple struct to hold GPU data for a single sub-mesh
struct SubMesh {
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    // We can store a material index here later if handling multiple textures
};

class MeshRenderer : public Component {
public:
    MeshRenderer(const std::string& path);
    ~MeshRenderer();

    void Draw(Shader& shader) override;

private:
    std::vector<SubMesh> meshes;
    std::string directory;

    // Helper functions for Assimp
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
};