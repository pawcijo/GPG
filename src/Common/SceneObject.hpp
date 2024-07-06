#pragma once

#include <vector>

class Transform;
class Model;

namespace GPGVulkan
{

    class SceneObject
    {
        SceneObject *mParent;
        std::vector<SceneObject *> mChildren;

    public:
        SceneObject();
        SceneObject(Transform *aTransform,
                    SceneObject *parent);

        Transform *mTransfrom;
        Model *mModel;
    };

}