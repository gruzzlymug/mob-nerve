#include "AssetMgr.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <string>

Mesh AssetMgr::loadObj(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // std::cerr << "Failed to open file: " << filename << std::endl;
        // TODO handle this error
    }

    Mesh mesh;
    std::vector<vector4> verts;
    std::vector<vector4> normals;
    std::vector<Triangle> triangles;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string identifier;
        iss >> identifier;

        if (identifier == "v") {
            // Parse vertex definitions
            vector4 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertex.w = 1.0f;
            verts.push_back(vertex);
        } else if (identifier == "vn") {
            // Parse vertex normal definitions
            vector4 vertex_normal;
            iss >> vertex_normal.x >> vertex_normal.y >> vertex_normal.z;
            vertex_normal.w = 0.0f;
            normals.push_back(vertex_normal);
        } else if (identifier == "f") {
            // Parse face definitions
            std::vector<int> vi;
            std::vector<int> vni;

            while (iss) {
                std::string token;
                iss >> token;
                if (token.empty())
                    break;  // End of line

                // Split the token using '/' as a delimiter
                std::istringstream tokenStream(token);
                std::string vertexIndexStr, textureIndexStr, normalIndexStr;
                getline(tokenStream, vertexIndexStr, '/');
                getline(tokenStream, textureIndexStr, '/');
                getline(tokenStream, normalIndexStr, '/');

                // Convert strings to integers (subtracting 1 to make them zero-based)
                int vertexIndex = std::stoi(vertexIndexStr) - 1;
                vi.push_back(vertexIndex);
                int normalIndex = std::stoi(normalIndexStr) - 1;
                vni.push_back(normalIndex);
            }
            Triangle triangle = {vi[0], vi[1], vi[2], vni[0], vni[1], vni[2]};
            triangles.push_back(triangle);
        }
    }

    mesh.set_vertices(verts);
    mesh.set_vertex_normals(normals);
    // move semantics just for fun
    mesh.set_triangles(std::move(triangles));

    file.close();

    return mesh;
}