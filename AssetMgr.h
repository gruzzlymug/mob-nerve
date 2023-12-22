#pragma once

#include <string>
#include <vector>

class AssetMgr {
private:
    std::vector<float> parseVertex(const std::string& line);
    void parseLine(const std::string& line);

public:
    AssetMgr() = default;
    ~AssetMgr() = default;

    void loadObj(const std::string& filename);
};