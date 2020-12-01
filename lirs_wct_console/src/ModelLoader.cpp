#include "ModelLoader.h"
#include "utils.h"

void Model::loadParameters(const std::string& texture, double scale, double rotateAngle)
{
    texture_ = texture;
    scale_ = scale;
    rotAngle_ = rotateAngle;
}

void Model::load(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, 0);
    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("error:= %s\n", importer.GetErrorString());
        return;
    }
    processNode(scene->mRootNode, scene);
}

void Model::loadFromScene(const aiScene* _scene)
{
    processNode(_scene->mRootNode, _scene);
}

void Model::save(const std::string &path)
{
    aiScene scene;
    scene.mRootNode = new aiNode();
    scene.mRootNode->mName = aiString("Root");
    Math::calculateTransformMatrix(scene.mRootNode->mTransformation, rotAngle_, scale_);
    buildTextures(scene);
    scene.mMaterials = new aiMaterial *[1];
    scene.mNumMaterials = 1;
    auto material = scene.mMaterials[0] = new aiMaterial();
    if(texture_ != "")
    {
        auto texPath = aiString(texture_);
        material->AddProperty(&texPath, AI_MATKEY_TEXTURE_DIFFUSE(0));
    }
    scene.mMeshes = new aiMesh *[meshes_.size()];
    scene.mNumMeshes = meshes_.size();
    //printf("meshes count:= %u/n", scene.mNumMeshes);
    auto a = meshes_[0];
    /*for(auto &i : a._vertices)
    {
        printf("UV x= %f, y= %f\n", i.uvCoordinate.x, i.uvCoordinate.y);
        printf("Position x= %f, y= %f, z= %f\n", i.position.x, i.position.y, i.position.z);
        printf("Normal x= %f, y= %f, z= %f\n", i.normal.x, i.normal.y, i.normal.z);
    }*/
    scene.mRootNode->mMeshes = new unsigned[meshes_.size()];
    scene.mRootNode->mNumMeshes = meshes_.size();
    for (unsigned i = 0; i < scene.mNumMeshes; ++i) {
        auto mesh = scene.mMeshes[i] = new aiMesh();
        mesh->mName = aiString("Mesh_" + std::to_string(i));
        scene.mMeshes[i]->mMaterialIndex = 0;
        scene.mRootNode->mMeshes[i] = i;
        buildAiMesh(meshes_[i], mesh);
    }
    Assimp::Exporter exporter;
    auto format = exporter.GetExportFormatDescription(0);
    //aiPostProcessSteps post;
    exporter.Export(&scene, "collada", path, aiProcess_FindInvalidData | aiProcess_OptimizeMeshes | aiProcess_FindDegenerates | aiProcess_OptimizeGraph);
}

void Model::buildAiMesh(const Mesh &src, aiMesh *mesh)
{
    auto vs = src._vertices;
    mesh->mNumVertices = vs.size();
    mesh->mPrimitiveTypes = 4;
    mesh->mVertices = new aiVector3D[vs.size()];
    mesh->mNormals = new aiVector3D[vs.size()];
    mesh->mNumUVComponents[0] = vs.size();
    mesh->mTextureCoords[0] = new aiVector3D[vs.size()];
    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        auto pos = vs[i].position;
        auto n = vs[i].normal;
        auto uv = vs[i].uvCoordinate;
        mesh->mVertices[i] = aiVector3D(pos(0), pos(1), pos(2));
        mesh->mNormals[i] = aiVector3D(n(0), n(1), n(2));
        mesh->mTextureCoords[0][i] = aiVector3D(uv(0), uv(1), 0);
    }
    auto fs = src._faces;
    mesh->mNumFaces = fs.size(); 
    mesh->mFaces = new aiFace[fs.size()];
    for (unsigned i = 0; i < mesh->mNumFaces; ++i) {
        auto &face = mesh->mFaces[i];
        face.mIndices = new unsigned[fs[i].size()];
        face.mNumIndices = fs[i].size();

        for (unsigned j = 0; j < face.mNumIndices; ++j) {
            face.mIndices[j] = fs[i][j];
        }
    }
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh *srcMesh = scene->mMeshes[node->mMeshes[i]];
        auto mesh = processMesh(srcMesh, scene);
        mesh.generateUVs();
        meshes_.push_back(mesh);
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++)
        processNode(node->mChildren[i], scene);
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        Eigen::Vector3f vector3f;

        vector3f(0) = mesh->mVertices[i].x;
        vector3f(1) = mesh->mVertices[i].y;
        vector3f(2) = mesh->mVertices[i].z;
        vertex.position = vector3f;

        vector3f(0) = mesh->mNormals[i].x;
        vector3f(1) = mesh->mNormals[i].y;
        vector3f(2) = mesh->mNormals[i].z;
        vertex.normal = vector3f;

        if (mesh->mTextureCoords[0]){
            Eigen::Vector2f vector2f;
            vector2f(0) = mesh->mTextureCoords[0][i].x;
            vector2f(1) = mesh->mTextureCoords[0][i].y;
            vertex.uvCoordinate = vector2f;

        } else
            vertex.uvCoordinate = Eigen::Vector2f(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for ( unsigned i = 0; i < mesh->mNumFaces; i++ ) {
        aiFace srcFace = mesh->mFaces[i];
        Face face;

        for ( unsigned j = 0; j < srcFace.mNumIndices; j++ ) {
            face.push_back(srcFace.mIndices[j]);
        }

        faces.push_back(face);
    }

    if (mesh->mMaterialIndex >= 0){
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        // Diffuse map
        std::vector<Texture> diffuseMaps = loadMaterialTextures( material, aiTextureType_DIFFUSE, "texture_diffuse" );
        textures.insert( textures.end( ), diffuseMaps.begin( ), diffuseMaps.end( ) );

        // Specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures( material, aiTextureType_SPECULAR, "texture_specular" );
        textures.insert( textures.end( ), specularMaps.begin( ), specularMaps.end( ) );
    }

    return Mesh(vertices, faces, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        texture.type = typeName;
        texture.path = str;
        textures.push_back(texture);
    }
    return textures;
}

void Model::printfModelInfo()
{
    printf("meshes size:= %lu\n", meshes_.size());
    printf("mat size:= %lu\n", meshes_[0]._textures.size());
}

void Model::buildTextures(aiScene &scene)
{
    unsigned texsCount = 0;

    for (auto m = meshes_.begin(); m != meshes_.end(); ++m) {
        for (auto t = m->_textures.begin(); t != m->_textures.end(); ++t, ++texsCount);
    }

    scene.mNumTextures = texsCount;
    scene.mTextures = new aiTexture *[texsCount];
    unsigned index = 0;

    for (auto m = meshes_.begin(); m != meshes_.end(); ++m) {
        for (auto t = m->_textures.begin(); t != m->_textures.end(); ++t) {
            auto texture = scene.mTextures[index++] = new aiTexture();
            // Intialize textures here
        }
    }
}
