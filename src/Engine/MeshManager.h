#pragma once
#include "../Art/MeshData.h"
#include <map>

class MeshManager
{
private:
    static std::map<std::string, MeshData*> meshDataMap;
public:
    MeshManager();
    ~MeshManager();
    static MeshData* getMeshData(std::string meshname);
};

