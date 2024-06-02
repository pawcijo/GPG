#pragma once

#include <Shader.h>
#include <Transform.h>
#include <Mesh/Mesh.h>

#include <MyPhysics/PhysicsBody.h>

class App;
class GameObject
{
protected:
    static unsigned ID;

private:
    Mesh *mesh;
    Transform *transform;
    PhysicsBody *body;

    bool isStatic;
    unsigned int pickingColorID;

    // unique Id
    unsigned int objectId;

    GameObject(Mesh *aMesh, PhysicsBody *aBody, Transform *aTransform);

    friend class GameObjectBuilder;

public:
    Mesh *GetMesh() { return mesh; };
    PhysicsBody *GetBody() { return body; }
    void SetStatic(bool isStatic);
    bool IsStatic();
    void Draw(Shader &shaderProgram, bool colorPick, App &app);
    unsigned int ObjectId();

    glm::vec3 Position();

    GameObject() = delete;
    ~GameObject();
};
