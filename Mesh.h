#pragma once

#include "matlib.h"
#include <vector>

struct Triangle {
    int v1, v2, v3;
};

class Mesh {
private:
    std::vector<vector4> vertices;
    std::vector<Triangle> triangles;

public:
    Mesh() = default;
    ~Mesh() = default;

    void setVertices(const std::vector<vector4>& vertices);
    void setTriangles(const std::vector<Triangle>&& triangles);

    const std::vector<vector4>& getVertices() const;
    const std::vector<Triangle>& getTriangles() const;
};
