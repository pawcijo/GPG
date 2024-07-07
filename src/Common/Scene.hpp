#pragma once

#include <filesystem>
#include <vector>

#include "SceneObject.hpp"

namespace GPGVulkan
{

    class Scene
    {

        std::vector<SceneObject *> sceneObjects;

    public:
        Scene();

        void SaveScene(std::filesystem::path aScenePath);
        void ClearScene();

        void serialize(std::ofstream &outFile) const;

        void AddSceneObject(SceneObject *object);
    };

}