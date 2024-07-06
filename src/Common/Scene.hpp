#pragma once

#include <filesystem>
#include <vector>

class SceneObject;

namespace GPGVulkan
{

    class Scene
    {

        std::vector<SceneObject *> sceneObjects;

    public:
        Scene();

        void SaveScene(std::filesystem::path aScenePath);
        void ClearScene();

        void AddSceneObject(SceneObject *object);
    };

}