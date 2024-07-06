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
        SceneObject(SceneObject *parent = nullptr,
                    Transform *aTransform = nullptr);

        Transform *mTransfrom;
        Model *mModel;
    };

}