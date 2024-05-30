#include "GameObjectManager.h"

void GameObjectManager::AddObject(GameObject * obj)
{
    if(nullptr != obj)
    objectList.push_back(obj);
}
void GameObjectManager::RemoveOjbect(unsigned id)
{
    for(int i = 0 ; i <objectList.size(); i++)
    {
        if(id == objectList[i]->ObjectId() )
        {
            objectList.erase(objectList.begin() + i);
            return;
        }
        
    }

}