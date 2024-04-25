#pragma once

#include "Obb.h"
#include <vector>
#include <glm/glm.hpp>
#include <unordered_set>

#include <cstdio>

#include <SimpleShape/Box.h>

class App;

class PhysicsManager
{
private:
    glm::vec3 gravity;

    App *app;

public:
    std::vector<Obb *> obbs;
    std::unordered_set<std::pair<Obb *, Obb *>, PairObbHash> collisionSet;
    PhysicsManager(float gravityX, float gravityY, float gravityZ);

    void SetApp(App *aApp) { app = aApp; }

    void AddOBB(Obb *obb);
    void Update(float deltaTime);

    void ClearCollisonMap() { collisionSet.clear(); }

    bool Intersects(Obb *one, Obb *other);
    void ResolveCollision(Obb *one, Obb *other);
    void ResolveAllCollision();

    void ApplyPositionCorrection(Obb *obb);

    void SynchroniseWithRender();

    friend class App;
};
