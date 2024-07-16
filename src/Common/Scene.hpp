#pragma once

#include <filesystem>
#include <vector>

#include "SceneObject.hpp"

namespace GPGVulkan
{

    class Scene
    {

        std::vector<SceneObject *> mSceneObjects;

        SceneObject *FindObjectPtr(unsigned long objectId);

    public:
        Scene();

        void SaveScene(std::filesystem::path aScenePath);
        void ClearScene();

        void Serialize(std::ofstream &outFile) const;
        void Deserialize(std::ifstream &inFile);

        void LoadPtrsAndRefs();

        void AddSceneObject(SceneObject *object);

        long long SceneSizeInBytes();

        std::vector<SceneObject *> &SceneObjects() { return mSceneObjects; };

        friend class SceneObject;
    };

}