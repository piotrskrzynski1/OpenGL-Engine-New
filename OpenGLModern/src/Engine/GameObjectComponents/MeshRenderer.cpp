#include "Engine/GameObjectComponents/MeshRenderer.hpp"
#include <OPENGL/glad/glad.h>
#include <iostream>
#include <OPENGL/glm/glm.hpp>
#include <Engine/GameObject.hpp>
MeshRenderer::MeshRenderer(const std::string& path) {
    LoadModel(path);
}

MeshRenderer::~MeshRenderer() {
    for (auto& mesh : meshes) {
        glDeleteVertexArrays(1, &mesh.VAO);
        glDeleteBuffers(1, &mesh.VBO);
        glDeleteBuffers(1, &mesh.EBO);
    }
}

void MeshRenderer::Draw(Shader& shader) {
    // 1. Get Transform from the Owner GameObject
    glm::mat4 model = glm::mat4(1.0f);
    if (owner) {
        model = glm::translate(model, owner->position);
        // Basic rotation logic (could be improved with Quaternions)
        model = glm::rotate(model, glm::radians(owner->rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(owner->rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(owner->rotation.z), glm::vec3(0, 0, 1));
        model = glm::scale(model, owner->scale);
    }

    shader.setMat4("model", model);
    
    // recalculate normal matrix
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    shader.setMat3("normalMatrix", normalMatrix);

    // 2. Draw all submeshes
    for (unsigned int i = 0; i < meshes.size(); i++) {
        glBindVertexArray(meshes[i].VAO);
        glDrawElements(GL_TRIANGLES, meshes[i].indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}

void MeshRenderer::LoadModel(const std::string& path) {
    Assimp::Importer importer;
    // Triangulate: Ensure all faces are triangles (GL_TRIANGLES)
    // FlipUVs: OpenGL expects origin at bottom-left, images often top-left
    const aiScene* scene = importer.ReadFile(path, 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    // Save directory for loading textures relative to the model later
    directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene->mRootNode, scene);
}

void MeshRenderer::ProcessNode(aiNode* node, const aiScene* scene) {
    // Process all meshes in current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }
    // Process children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void MeshRenderer::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<float> vertices; 
    std::vector<unsigned int> indices;

    // 1. Process Vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Positions
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);

        // Normals
        if (mesh->HasNormals()) {
            vertices.push_back(mesh->mNormals[i].x);
            vertices.push_back(mesh->mNormals[i].y);
            vertices.push_back(mesh->mNormals[i].z);
        } else {
            vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
        }

        // Texture Coords (Assimp allows up to 8 channels, we use 0)
        if (mesh->mTextureCoords[0]) {
            vertices.push_back(mesh->mTextureCoords[0][i].x);
            vertices.push_back(mesh->mTextureCoords[0][i].y);
        } else {
            vertices.push_back(0.0f); vertices.push_back(0.0f);
        }
    }

    // 2. Process Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // 3. Create Buffers (VAO/VBO/EBO)
    SubMesh subMesh;
    subMesh.indexCount = indices.size();

    glGenVertexArrays(1, &subMesh.VAO);
    glGenBuffers(1, &subMesh.VBO);
    glGenBuffers(1, &subMesh.EBO);

    glBindVertexArray(subMesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, subMesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, subMesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Stride = 8 floats (3 pos + 3 normal + 2 uv)
    int stride = 8 * sizeof(float);

    // Position (Loc 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Normal (Loc 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // UV (Loc 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Store the mesh
    meshes.push_back(subMesh);
}