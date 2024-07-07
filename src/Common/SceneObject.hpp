#pragma once

#include <vector>

#include "Common/Transform.h"
#include "Vulkan/Model.hpp"

namespace GPGVulkan
{

    class SceneObject
    {

        SceneObject *mParent;
        std::vector<SceneObject *> mChildren;

        // serialize
        unsigned long mObjectId;
        unsigned long mParentId;
        std::vector<unsigned long> mChildrenIds;

        Transform *mTransform;
        Model *mModel;

        static unsigned long ObjectIDCounter;

    public:
        SceneObject();
        SceneObject(Transform *aTransform,
                    SceneObject *parent);

        void serialize(std::ofstream &outFile) const;

        void LoadPtrs();

        SceneObject * FindObjectPtr(SceneObject * root,unsigned long objId);

        void AddChild(SceneObject *aChild);
        void SetParent(SceneObject *aParent);

        unsigned long ObjectId() { return mObjectId; }
    };

}