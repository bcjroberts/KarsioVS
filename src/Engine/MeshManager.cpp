#include "MeshManager.h"

MeshManager::MeshManager() = default;
MeshManager::~MeshManager() = default;

std::map<std::string, MeshData*> MeshManager::meshDataMap;

// Retrieves the mesh data either by returning an existing reference or creating a new one.
MeshData* MeshManager::getMeshData(std::string meshname) {
    MeshData* result = meshDataMap[meshname];
    if (!result)
    {
        result = new MeshData(meshname);
        result->loadMeshData("data/assets/meshes/" + meshname);
        meshDataMap.insert_or_assign(meshname, result);
    }
    return result;
}
