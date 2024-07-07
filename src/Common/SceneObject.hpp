#pragma once

#include <vector>

#include "Common/Transform.h"
#include "Vulkan/Model.hpp"

namespace GPGVulkan
{

    class Scene;
    class SceneObject
    {

        Scene *mScene;
        SceneObject *mParent;
        std::vector<SceneObject *> mChildren;

        // serialize
        unsigned long mObjectId;
        unsigned long mParentId;
        std::vector<unsigned long> mChildrenIds;

        Transform *mTransform;
        Model *mModel; // change to mesh ?

        static unsigned long ObjectIDCounter;

    public:
        SceneObject();
        SceneObject(Transform *aTransform,
                    SceneObject *parent);

        void Serialize(std::ofstream &outFile) const;
        void Deserialize(std::ifstream &inFile);

        void LoadPtrs();

        SceneObject *FindObjectPtr(SceneObject *root, unsigned long objId);

        void AddChild(SceneObject *aChild);
        void SetParent(SceneObject *aParent);
        void SetModel(Model *model);

        Model *Model() { return mModel; };
        Transform *SceneObjectTransform() { return mTransform; }

        unsigned long ObjectId() { return mObjectId; }

        friend class Scene;
    };

}