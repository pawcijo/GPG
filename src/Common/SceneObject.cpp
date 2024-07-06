#include "SceneObject.hpp"

#include "Common/Transform.h"

namespace GPGVulkan
{

    SceneObject::SceneObject(SceneObject *parent,
                             Transform *aTransform)
        : mParent(parent),
          mTransfrom(aTransform)
    {
    }

}
