#include "SceneObject.hpp"

#include "Common/Transform.h"

namespace GPGVulkan
{

  SceneObject::SceneObject() : mParent(nullptr),
                               mTransfrom(new Transform())
  {
  }

  SceneObject::SceneObject(Transform *aTransform,
                           SceneObject *parent)
      : mParent(parent),
        mTransfrom(aTransform)
  {
  }

}
