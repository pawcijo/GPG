#include "GameObjectManager.h"

void GameObjectManager::AddObject(GameObject *obj)
{
    if (nullptr != obj)
        objectList.push_back(obj);
}
void GameObjectManager::RemoveOjbect(unsigned int id)
{
    for (int i = 0; i < objectList.size(); i++)
    {
        if (id == objectList[i]->ObjectId())
        {
            objectList.erase(objectList.begin() + i);
            return;
        }
    }
}

GameObject *GameObjectManager::GetSelectedObjectPtr(unsigned int selectedId)
{
    for (int i = 0; i < objectList.size(); i++)
    {
        if (selectedId == objectList[i]->ObjectId())
        {
            return objectList[i];
        }
    }
    return nullptr;
}
