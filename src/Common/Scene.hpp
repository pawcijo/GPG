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

        void serialize(std::ofstream &outFile) const;

        void LoadPtrsAndRefs();

        void AddSceneObject(SceneObject *object);

        std::vector<SceneObject *> &SceneObjects() { return mSceneObjects; };

        friend class SceneObject;
    };

}