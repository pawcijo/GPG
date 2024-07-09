#pragma once

#include <vector>

#include "Common/Transform.h"

namespace GPGVulkan
{

    class Scene;
    class Model;
    class SceneObject
    {

        Scene *mScene;
        SceneObject *mParent;
        std::vector<SceneObject *> mChildren;

        // serialize
        unsigned long mObjectId;
        unsigned long mParentId;
        std::vector<unsigned long> mChildrenIds;
        std::string mName;

        Transform mTransform;
        Model *mModel; // change to mesh ?

        static unsigned long ObjectIDCounter;

    public:
        SceneObject();
        SceneObject(Transform aTransform,
                    SceneObject *parent);

        void Serialize(std::ofstream &outFile) const;
        void Deserialize(std::ifstream &inFile);

        void LoadPtrs();

        SceneObject *FindObjectPtr(SceneObject *root, unsigned long objId);

        void AddChild(SceneObject *aChild);
        void SetParent(SceneObject *aParent);
        void SetModel(Model *model);
        void SetName(const std::string& aName);

        Model *ModelPtr();
        Transform TransformValue();
        const std::string &Name() const { return mName; }

        std::vector<SceneObject *> Children();

        unsigned long ObjectId();
        unsigned long ParentId();

        friend class Scene;
    };

}