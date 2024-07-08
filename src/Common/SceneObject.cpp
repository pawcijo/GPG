#include "SceneObject.hpp"

#include "Common/Transform.h"
#include "Common/Scene.hpp"

#include "Vulkan/Model.hpp"

#include <vector>
#include <fstream>

namespace GPGVulkan
{

  SceneObject::SceneObject() : mParent(nullptr), mModel(nullptr), mParentId(0), mName("SceneObject"),
                               mTransform(Transform())
  {
    mObjectId = ObjectIDCounter;
    ObjectIDCounter++;
  }

  SceneObject::SceneObject(Transform aTransform,
                           SceneObject *parent)
      : mParent(parent),
        mTransform(aTransform),
        mName("SceneObject"),
        mModel(nullptr)

  {
    mObjectId = ObjectIDCounter;
    ObjectIDCounter++;
  }

  Transform SceneObject::TransformValue()
  {
    return mTransform;
  }
  unsigned long SceneObject::ObjectId()
  {
    return mObjectId;
  }

  unsigned long SceneObject::ParentId()
  {
    return mParentId;
  }

  void SceneObject::SetName(const std::string &aName)
  {
    mName = aName;
  }

  std::vector<SceneObject *> SceneObject::Children()
  {
    return mChildren;
  }

  void SceneObject::AddChild(SceneObject *aChild)
  {
    if (nullptr != aChild)
    {

      aChild->mParentId = mObjectId;
      aChild->mParent = this;
      mChildren.push_back(aChild);
      mChildrenIds.push_back(aChild->mObjectId);
    }
  }
  void SceneObject::SetParent(SceneObject *aParent)
  {
    if (nullptr != aParent)
    {
      mParent = aParent;
    }
  }

  void SceneObject::SetModel(GPGVulkan::Model *aModel)
  {
    mModel = aModel;
  }

  void SceneObject::LoadPtrs()
  {
    if (0 != mParentId)
    {
      mParent = mScene->FindObjectPtr(mParentId);
    }

    for (auto childId : mChildrenIds)
    {
      mChildren.push_back(mScene->FindObjectPtr(childId));
      mChildren[childId]->LoadPtrs();
    }
  }

  SceneObject *SceneObject::FindObjectPtr(SceneObject *root, unsigned long objId)
  {
    // Check if current node is nullptr or matches the objId
    if (root == nullptr || root->ObjectId() == objId)
    {
      return root;
    }

    // Recursively search in children
    for (auto &child : root->mChildren)
    {
      SceneObject *result = FindObjectPtr(child, objId);
      if (result != nullptr)
      {
        return result;
      }
    }

    // ObjId not found in this subtree
    return nullptr;
  }

  Model *SceneObject::ModelPtr()
  {
    return mModel;
  };

  void SceneObject::Serialize(std::ofstream &outFile) const
  {
  }

  void SceneObject::Deserialize(std::ifstream &inFile)
  {
  }

  unsigned long SceneObject::ObjectIDCounter = 1;

}
