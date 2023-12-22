#include "AssetMgr.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

std::vector<float> AssetMgr::parseVertex(const std::string& line) {
    std::istringstream iss(line);
    std::string label;
    float x, y, z;

    // Read the label (v) and the three float values
    if (!(iss >> label >> x >> y >> z)) {
        throw std::runtime_error("Invalid vertex format");
    }

    return std::vector<float>{x, y, z};
}

void AssetMgr::parseLine(const std::string& line) {
    std::istringstream iss(line);
    std::string identifier;
    iss >> identifier;

    if (identifier == "v") {
        std::cout << line << std::endl;
        parseVertex(line);
    } else if (identifier == "vt") {
        // parseTexture(line);
    } else if (identifier == "vn") {
        // parseNormal(line);
    } else {
        // std::cout << "Unsupported or unrecognized line type: " << line << std::endl;
    }
}

void AssetMgr::loadObj(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        // std::cerr << "Failed to open file: " << filename << std::endl;
        // return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        parseLine(line);
    }

    file.close();
}