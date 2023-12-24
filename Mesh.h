#pragma once

#include "matlib.h"
#include <vector>

struct Triangle {
    int v1, v2, v3;
    int vn1, vn2, vn3;
};

class Mesh {
private:
    std::vector<vector4> vertices;
    std::vector<vector4> vertex_normals;
    std::vector<Triangle> triangles;

public:
    Mesh() = default;
    ~Mesh() = default;

    void set_vertices(const std::vector<vector4>& vertices);
    void set_vertex_normals(const std::vector<vector4>& vertex_normals);
    void set_triangles(const std::vector<Triangle>&& triangles);

    const std::vector<vector4>& get_vertices() const;
    const std::vector<Triangle>& get_triangles() const;

    const std::vector<vector4> get_transformed_vertices(const matrix44& transform) const;
    const std::vector<vector4> get_transformed_vertex_normals(const matrix44& transform) const;
};
