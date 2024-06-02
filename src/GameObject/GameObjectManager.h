#pragma once

#include "GameObject.h"

#include <vector>

class GameObjectManager
{
public:
    std::vector<GameObject *> objectList;

    void AddObject(GameObject *obj);

    /**
     * Removes Gameobject for selected Id.
     * @param SelectedId
     **/
    void RemoveOjbect(unsigned int Id);
    /**
     * Returns Gameobject for selected Id.
     * @param SelectedId
     * @returns GameObject;
     * ! Returns null ptr if object not found.
     **/
    GameObject *GetSelectedObjectPtr(unsigned int selectedId);
};
