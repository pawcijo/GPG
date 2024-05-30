#pragma once

#include <Mesh/Mesh.h>
#include <MyPhysics/PhysicsBody.h>
#include "GameObject.h"

class GameObjectBuilder
{
private:
    Mesh *mesh = nullptr;
    PhysicsBody *body = nullptr;
    Transform *transform = nullptr;

public:
    GameObjectBuilder &setMesh(Mesh *aMesh)
    {
        this->mesh = aMesh;
        return *this;
    }

    GameObjectBuilder &setBody(PhysicsBody *aBody)
    {
        this->body = aBody;
        return *this;
    }

    GameObjectBuilder &setTransform(Transform *aTransform)
    {
        this->transform = aTransform;
        return *this;
    }

    GameObject *build()
    {
        GameObject *obj = new GameObject(mesh, body, transform);

        mesh = nullptr;
        body = nullptr;
        transform = nullptr;
        return obj;
    }
};