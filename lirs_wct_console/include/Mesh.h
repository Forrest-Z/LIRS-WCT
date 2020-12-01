#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include "Common.h"
#include <Eigen/Geometry>
//#include "math_3d.h"

using Face = std::vector<unsigned>;

struct Vertex
{
    Eigen::Vector3f position;
    Eigen::Vector3f normal;
    Eigen::Vector2f uvCoordinate;
};

struct Texture
{
    unsigned id;
    std::string type;
    aiString path;
};

class Mesh;

enum Look : unsigned char {
    POS_X,
    NEG_X,
    POS_Y,
    NEG_Y,
    POS_Z,
    NEG_Z
};

class Bounds {
private:
    Eigen::Vector3f _start;
    Eigen::Vector3f _end;
public:
    Bounds(const Eigen::Vector3f &start, const Eigen::Vector3f &end);
    Bounds(const Mesh &mesh);
    ~Bounds();
    Eigen::Vector3f start();
    void start(const Eigen::Vector3f &start);
    Eigen::Vector3f end();
    void end(const Eigen::Vector3f &end);
    Eigen::Vector3f distance();
    Look lookAt(const Eigen::Vector3f &vec);
private:
    void init(const Mesh &mesh);
};

class Mesh
{
public:
    std::vector<Vertex> _vertices;
    std::vector<Face> _faces;
    std::vector<Texture> _textures;
    Bounds _bounds;
    Mesh(std::vector<Vertex>& vertices, std::vector<Face>& indices, std::vector<Texture>& textures);
    ~Mesh();
    void generateUVs(const float sx = 1.0f, const float sy = 1.0f, const float sz = 1.0f);
};


#endif //MESH_H
