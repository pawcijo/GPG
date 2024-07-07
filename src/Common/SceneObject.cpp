#include "SceneObject.hpp"

#include "Common/Transform.h"


#include <vector>
#include <fstream>

namespace GPGVulkan
{

  

  SceneObject::SceneObject() : mParent(nullptr),
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

  void SceneObject::serialize(std::ofstream &outFile) const
  {
    // Serialize mObjectId
    outFile.write(reinterpret_cast<const char *>(&mObjectId), sizeof(mObjectId));

    // Serialize mParentId
    outFile.write(reinterpret_cast<const char *>(&mParentId), sizeof(mParentId));

    // Serialize number of children
    size_t numChildren = mChildrenIds.size();
    outFile.write(reinterpret_cast<const char *>(&numChildren), sizeof(numChildren));

    // Serialize each child's ID
    for (const auto &childId : mChildrenIds)
    {
      outFile.write(reinterpret_cast<const char *>(&childId), sizeof(childId));
    }

    // Serialize mTransform
    bool hasTransform = (mTransform != nullptr);
    outFile.write(reinterpret_cast<const char *>(&hasTransform), sizeof(hasTransform));
    if (hasTransform)
    {
      mTransform->serialize(outFile);
    }

     bool hasModel = (mModel != nullptr);
     outFile.write(reinterpret_cast<const char*>(&hasModel), sizeof(hasModel));
     if (hasModel) {
         mModel->serialize(outFile);
     }
  }

unsigned long SceneObject::ObjectIDCounter = 0;

}
