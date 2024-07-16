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
        SaveSceneXml(aPath, this);
    }

    void Scene::Serialize(std::ofstream &outFile) const
    {
        throw std::logic_error("Not implemented");
    }
    void Scene::Deserialize(std::ifstream &inFile)
    {
        throw std::logic_error("Not implemented");
    }

    void Scene::ClearScene()
    {

        for (auto sceneObj : mSceneObjects)
        {
            sceneObj->Destroy();
        }

        mSceneObjects.clear();
    }

    void Scene::AddSceneObject(SceneObject *aObject)
    {
        if (nullptr != aObject)
        {
            aObject->mParentId = 0;
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

    long long Scene::SceneSizeInBytes()
    {
        long long wholeSize = 0;

        for (auto sceneobject : mSceneObjects)
        {
            wholeSize += sceneobject->SceneObjectSizeInBytes();
        }

        return wholeSize;
    }

}