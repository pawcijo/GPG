#pragma once

#include <filesystem>

#include "Scene.hpp"

#include <tinyxml2.h>

namespace GPGVulkan
{


    void SaveSceneBinary(std::filesystem::path aPath, Scene *aScene);
    Scene *LoadSceneBinary(std::filesystem::path aPath);

    void SaveSceneXml(std::filesystem::path aPath, Scene *aScene);

    using namespace tinyxml2;
    XMLElement *CreateTransform(XMLDocument& document, Transform aTransform);
    XMLElement *CreateModelInfo(XMLDocument &doc, Model *aModel);
    XMLElement *CreateSceneObject(XMLDocument &doc, SceneObject *sceneObj);

}