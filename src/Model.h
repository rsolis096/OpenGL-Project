#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Object.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

class Model : public Object
{
public:
    // Model data 
    vector<ModelTexture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>    meshes;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(string const& path, bool gamma = false);
    ~Model() override;
    // draws the model, and thus all its meshes
    void Draw(Shader& shader) override;
    void ShadowPassDraw(Shader& shader) override;

    static const aiScene* CheckPath(std::string const& path);
    ObjectType GetType() const override;

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path);
    
    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<ModelTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
    int updateTexture(std::vector<std::string> texturePaths) override { return 1; }
    static unsigned int modelCount;

    static Assimp::Importer importer;

};