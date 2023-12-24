#include "Mesh.h"

#include <memory>

void Mesh::set_vertices(const std::vector<vector4>& vertices) {
    this->vertices = vertices;
}

void Mesh::set_vertex_normals(const std::vector<vector4>& vertex_normals) {
    this->vertex_normals = vertex_normals;
}

void Mesh::set_triangles(const std::vector<Triangle>&& triangles) {
    this->triangles = std::move(triangles);
}

const std::vector<vector4>& Mesh::get_vertices() const {
    return vertices;
}

const std::vector<Triangle>& Mesh::get_triangles() const {
    return triangles;
}

const std::vector<vector4> Mesh::get_transformed_vertices(const matrix44& transform) const {
    std::vector<vector4> transformed(vertices.size());
    std::transform(vertices.begin(), vertices.end(), transformed.begin(), [&transform](const vector4& vertex) {
        return transform * vertex;
    });
    return transformed;
}

// NOTE assumes no scaling
const std::vector<vector4> Mesh::get_transformed_vertex_normals(const matrix44& transform) const {
    std::vector<vector4> transformed(vertex_normals.size());
    std::transform(vertex_normals.begin(), vertex_normals.end(), transformed.begin(), [&transform](const vector4& vertex_normal) {
        return transform * vertex_normal;
    });
    return transformed;
}