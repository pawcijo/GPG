#pragma once

#include <filesystem>

#include "Scene.hpp"
#include "Vulkan/VulkanPipeLine.hpp"

#include <tinyxml2.h>

namespace GPGVulkan
{

    void SaveSceneBinary(std::filesystem::path aPath, Scene *aScene);
    Scene *LoadSceneBinary(std::filesystem::path aPath);

    void SaveSceneXml(std::filesystem::path aPath, Scene *aScene);

    Scene *LoadSceneXml(std::filesystem::path aPath, VulkanApp &app, VulkanPipeLine &aPipeline);

    using namespace tinyxml2;
    XMLElement *CreateTransform(XMLDocument &document, Transform aTransform);
    XMLElement *CreateModelInfo(XMLDocument &doc, Model *aModel);
    XMLElement *CreateSceneObject(XMLDocument &doc, SceneObject *sceneObj);

    SceneObject *ParseSceneObjectRecursive(tinyxml2::XMLElement *sceneObjElement, SceneObject *parentObj,
                                           VulkanApp &aApp, VulkanPipeLine &aPipeline);

}