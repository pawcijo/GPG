#pragma once

#include <vulkan/vulkan.h>
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

        SceneObject(unsigned long aObjectId,
                    unsigned long aParentId,
                    std::string aName,
                    Transform aTransform,
                    Model *aModel);

        ~SceneObject();

        void LoadPtrs();

        SceneObject *FindObjectPtr(SceneObject *root, unsigned long objId);

        void AddChild(SceneObject *aChild);
        void SetParent(SceneObject *aParent);
        void SetModel(Model *model);
        void SetName(const std::string &aName);

        Model *ModelPtr();
        Transform TransformValue();
        const std::string &Name() const { return mName; }

        void RecordDraw(VkCommandBuffer aCommandBuffer,
                        uint32_t aCurrentFrame, VkPipelineLayout aPipelineLayout);

        std::vector<SceneObject *> Children();
        std::vector<unsigned long> ChildrenIds();

        void Destroy();

        void DrawSceneObjectGraph();

        long long SceneObjectSizeInBytes();

        unsigned long ObjectId();
        unsigned long ParentId();

        friend class Scene;
    };

}