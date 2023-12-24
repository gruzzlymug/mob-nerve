#include "Mesh.h"

#include <memory>

void Mesh::setVertices(const std::vector<vector4>& vertices) {
    this->vertices = vertices;
}

void Mesh::setTriangles(const std::vector<Triangle>&& triangles) {
    this->triangles = std::move(triangles);
}

const std::vector<vector4>& Mesh::getVertices() const {
    return vertices;
}

const std::vector<Triangle>& Mesh::getTriangles() const {
    return triangles;
}

const std::vector<vector4> Mesh::getTransformedVertices(const matrix44& transform) const {
    std::vector<vector4> transformed(vertices.size());
    std::transform(vertices.begin(), vertices.end(), transformed.begin(), [&transform](const vector4& vertex) {
        return transform * vertex;
    });
    return transformed;
}
