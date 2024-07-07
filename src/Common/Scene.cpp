#include "Scene.hpp"

#include "SceneObject.hpp"
#include "SceneUtils.hpp"

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
        SaveSceneBinary(aPath, this);
    }

    void Scene::serialize(std::ofstream &outFile) const
    {
        // Write number of SceneObjects
        size_t numObjects = mSceneObjects.size();
        outFile.write(reinterpret_cast<const char *>(&numObjects), sizeof(numObjects));

        // Write each SceneObject
        for (const auto *obj : mSceneObjects)
        {
            obj->serialize(outFile);
        }
    }

    void Scene::ClearScene()
    {
        mSceneObjects.clear();
    }

    void Scene::AddSceneObject(SceneObject *aObject)
    {
        if (nullptr != aObject)
        {
            mSceneObjects.push_back(aObject);
        }
    }

    SceneObject *Scene::FindObjectPtr(unsigned long objectId)
    {
        for (auto *sceneObj : mSceneObjects)
        {
            auto *ptr = sceneObj->FindObjectPtr(sceneObj, objectId);
            if (nullptr != ptr)
            {
                return ptr;
            }
        }
        return nullptr;
    }

    void Scene::LoadPtrsAndRefs()
    {
        for (auto *sceneObj : mSceneObjects)
        {
            sceneObj->LoadPtrs();
        }
    }

}