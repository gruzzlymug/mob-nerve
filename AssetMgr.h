#pragma once

#include "Mesh.h"
#include <string>

class AssetMgr {
private:

public:
    AssetMgr() = default;
    ~AssetMgr() = default;

    Mesh loadObj(const std::string& filename);
};