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

  void SceneObject::Destroy()
  {
    // Recursively destroy all children
    for (SceneObject *child : mChildren)
    {
      child->Destroy();
      delete child;
    }
    mChildren.clear();

    // Perform any other necessary cleanup here

    // Optionally, if this object is owned by a parent, remove it from the parent's child list
    if (mParent)
    {
      auto it = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
      if (it != mParent->mChildren.end())
      {
        mParent->mChildren.erase(it);
      }
    }
  }

  std::vector<unsigned long> SceneObject::ChildrenIds()
  {
    return mChildrenIds;
  }

  SceneObject::SceneObject(unsigned long aObjectId,
                           unsigned long aParentId,
                           std::string aName,
                           Transform aTransform,
                           Model *aModel) : mObjectId(aObjectId), mParentId(aParentId), mName(aName), mTransform(aTransform), mModel(aModel)
  {
  }

  SceneObject::~SceneObject()
  {
    ObjectIDCounter = ObjectIDCounter - 1;
  };

  unsigned long SceneObject::ObjectIDCounter = 1;

}
