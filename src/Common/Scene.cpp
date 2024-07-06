#include "Scene.hpp"

#include "SceneObject.hpp"

namespace GPGVulkan
{
    Scene::Scene()
    {
    }

    void Scene::SaveScene(std::filesystem::path aPath)
    {
        // Save to
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