#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <string>
#include <vector>
#include "Common.h"
#include "Mesh.h"

class Model
{    
public:
    void loadParameters(const std::string& texture, double scale, double rotateAngle);
    void load(const std::string& path);
    void loadFromScene(const aiScene* scene);
    void save(const std::string& path);
    void buildAiMesh(const Mesh& src, aiMesh* mesh);
    void printfModelInfo();
private:
    std::vector<Mesh> meshes_;
    std::string texture_;
    double scale_;
    double rotAngle_;
    const aiScene* scene;
    void buildTextures(aiScene& scene);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    
};


#endif //MODELLOADER_H
