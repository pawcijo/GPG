#include "SceneObject.hpp"

#include "Common/Transform.h"
#include "Common/Scene.hpp"

#include "Vulkan/Model.hpp"

#include <vector>
#include <fstream>

namespace GPGVulkan
{

  SceneObject::SceneObject() : mParent(nullptr), mParentId(0),
                               mTransform(new Transform())
  {
    mObjectId = ObjectIDCounter;
    ObjectIDCounter++;
  }

  void SceneObject::AddChild(SceneObject *aChild)
  {
    if (nullptr != aChild)
    {
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

  SceneObject::SceneObject(Transform *aTransform,
                           SceneObject *parent)
      : mParent(parent),
        mTransform(aTransform)
  {
    mObjectId = ObjectIDCounter;
    ObjectIDCounter++;
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

  void SceneObject::Serialize(std::ofstream &outFile) const
  {
  }

  void SceneObject::Deserialize(std::ifstream &inFile)
  {
  }

  unsigned long SceneObject::ObjectIDCounter = 0;

}
