#pragma once

#include <filesystem>
#include <vector>

#include "SceneObject.hpp"
#include "Physics/PhysicsEntityManager.hpp"

class PhysicsWorld;

namespace GPGVulkan
{

    class Scene
    {
    private:
        std::vector<SceneObject *> mSceneObjects;

        PhysicsEntityManager mEntityManager;
        PhysicsWorld *mPhysicsWorld;
        SceneObject *FindObjectPtr(unsigned long objectId);

    public:
        Scene();

       // PhysicsWorld *PhysicsWorldPtr();

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