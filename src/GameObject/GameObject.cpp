#include "GameObject.h"

unsigned int GameObject::ID = 0;

GameObject::GameObject(Mesh *aMesh,
                       PhysicsBody *aBody,
                       Transform *aTransform) : mesh(aMesh),
                                                body(aBody),
                                                transform(aTransform)
{
    ID++;
    objectId = ID;
}

GameObject::~GameObject()
{
    printf("GameObject removed.\n");
}

void GameObject::Draw(Shader &shaderProgram, bool drawColorPick, App &app)
{
    if (nullptr != mesh)
    {
        if (nullptr != transform)
        {
            if (!drawColorPick)
            {
                mesh->Draw(shaderProgram, *transform, objectId, app);
            }
            else
            {
                mesh->Draw_Color(*transform, objectId);
            }
        }
        else
        {
            if (!drawColorPick)
            {
                mesh->Draw(shaderProgram, body->GetTransformMat4(), objectId, app);
            }
            else
            {
                mesh->Draw_Color(body->GetTransformMat4(), objectId);
            }
        }
    }
}

unsigned int GameObject::ObjectId()
{
    return objectId;
}

void GameObject::SetStatic(bool aIsStatic)
{
    isStatic = aIsStatic;
}
bool GameObject::IsStatic()
{
    return isStatic;
}