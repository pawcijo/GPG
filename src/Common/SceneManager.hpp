#pragma once

// Singleton for sceneObject managment

class Scene;
class SceneManager
{

    Scene *mScene;

public:
    long long GenerateUId();
};