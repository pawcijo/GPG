#include "Scene.hpp"

#include "SceneObject.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <ranges>

namespace GPGVulkan
{
    Scene::Scene()
    {
    }

    void Scene::SaveScene(std::filesystem::path aPath)
    {
        // Save to
    }

    void Scene::serialize(std::ofstream &outFile) const
    {
        // Write number of SceneObjects
        size_t numObjects = sceneObjects.size();
        outFile.write(reinterpret_cast<const char *>(&numObjects), sizeof(numObjects));

        // Write each SceneObject
        for (const auto *obj : sceneObjects)
        {
            obj->serialize(outFile);
        }
    }

    void Scene::ClearScene()
    {
        sceneObjects.clear();
    }

    void Scene::AddSceneObject(SceneObject *aObject)
    {
        if (nullptr != aObject)
        {
            sceneObjects.push_back(aObject);
        }
    }

}