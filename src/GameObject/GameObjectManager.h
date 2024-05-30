#pragma once

#include "GameObject.h"

#include <vector>

class GameObjectManager
{
public:
    std::vector<GameObject*> objectList;

    void AddObject(GameObject * obj);
    void RemoveOjbect(unsigned int Id);
};
