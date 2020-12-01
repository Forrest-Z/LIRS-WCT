#include <cmath>
#include "Mesh.h"

Bounds::Bounds(const Eigen::Vector3f &start, const Eigen::Vector3f &end) :
    _start{start}, _end{end}
{

}

Bounds::Bounds(const Mesh &mesh)
{
    init(mesh);
}

Bounds::~Bounds()
{

}

Eigen::Vector3f Bounds::start()
{
    return _start;
}

void Bounds::start(const Eigen::Vector3f &start)
{
    _start = start;
}

Eigen::Vector3f Bounds::end()
{
    return _end;
}

void Bounds::end(const Eigen::Vector3f &end)
{
    _end = end;
}

Eigen::Vector3f Bounds::distance()
{
    return _end - _start;
}

Look Bounds::lookAt(const Eigen::Vector3f &vec)
{
    float xy = atan2(vec(1), vec(0)) * 180 / M_PI;
    float yx = atan2(vec(0), vec(1)) * 180 / M_PI;
    float xz = atan2(vec(2), vec(0)) * 180 / M_PI;
    float yz = atan2(vec(2), vec(1)) * 180 / M_PI;
    float zx = atan2(vec(0), vec(2)) * 180 / M_PI;
    float zy = atan2(vec(1), vec(2)) * 180 / M_PI;
    auto c = [](float min, float v, float max) {
            float av = abs(v);
            return av >= min && av <= max;
    };
    if (c(0, xy, 45) && c(0, xz, 45)) { return Look::POS_X; }
    if (c(135, xy, 225) && c(135, xz, 225)) { return Look::NEG_X; }
    if (c(0, yx, 45) && c(0, yz, 45)) { return Look::POS_Y; }
    if (c(135, yx, 225) && c(135, yz, 225)) { return Look::NEG_Y; }
    if (c(0, zx, 45) && c(0, zy, 45)) { return Look::POS_Z; }

    return Look::NEG_Z;
}

void Bounds::init(const Mesh &mesh)
{
    auto vs = mesh._vertices;
    // Check if the mesh contains vertices
    if (vs.size() == 0) { return ; }
    // Initialize boundaries
    _start = _end = vs[0].position;
    // Update boundaries
    for (auto it = vs.begin(); it != vs.end(); ++it) {
        auto v = it->position;
        if (_start(0) > v(0)) { _start(0) = v(0); }
        if (_start(1) > v(1)) { _start(1) = v(1); }
        if (_start(2) > v(2)) { _start(2) = v(2); }
        if (_end(0) < v(0)) { _end(0) = v(0); }
        if (_end(1) < v(1)) { _end(1) = v(1); }
        if (_end(2) < v(2)) { _end(2) = v(2); }
    }
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<Face>& indices, std::vector<Texture>& textures) :
    _vertices{vertices}, _faces{indices}, _textures{textures}, _bounds{*this}
{
}

Mesh::~Mesh()
{

}

void Mesh::generateUVs(const float sx, const float sy, const float sz)
{
    // Allows to normalize a value to the [0; 1] interval
    auto normalize = [](float min, float v, float max) {
        return (v - min) / (max - min);
    };
    // Handle vertices
    for (auto it = _vertices.begin(); it != _vertices.end(); ++it) {
        Look look = _bounds.lookAt(it->normal);
        auto pos = it->position;
        Eigen::Vector2f uv;

        switch (look) {
        case Look::POS_X:
        case Look::NEG_X:
            uv(0) = sy * normalize(_bounds.start()(1), pos(1), _bounds.end()(1));
            uv(1) = sz * normalize(_bounds.start()(2), pos(2), _bounds.end()(2));
            break;
        case Look::POS_Y:
        case Look::NEG_Y:
            uv(0) = sx * normalize(_bounds.start()(0), pos(0), _bounds.end()(0));
            uv(1) = sz * normalize(_bounds.start()(2), pos(2), _bounds.end()(2));
            break;
        default:
            uv(0) = sx * normalize(_bounds.start()(0), pos(0), _bounds.end()(0));
            uv(1) = sy * normalize(_bounds.start()(1), pos(1), _bounds.end()(1));
            break;
        }

        it->uvCoordinate = uv;
    }
}
