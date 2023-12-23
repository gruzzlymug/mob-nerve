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