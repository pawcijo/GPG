#include "SceneUtils.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ranges>

#include "Vulkan/Model.hpp"
#include "Vulkan/VulkanApp.hpp"
#include "Vulkan/VulkanPipeLine.hpp"
#include "Common/SceneObject.hpp"

#include <tinyxml2.h>

namespace GPGVulkan
{
    using namespace tinyxml2;

    void SaveSceneBinary(std::filesystem::path filepath, Scene *aScene)
    {
        throw std::runtime_error("Not implemented");
    }
    Scene *LoadSceneBinary(std::filesystem::path aPath)
    {
        throw std::runtime_error("Not implemented");
    }

    SceneObject *ParseSceneObjectRecursive(tinyxml2::XMLElement *sceneObjElement, SceneObject *parentObj, VulkanApp &aApp, VulkanPipeLine &aPipeline)
    {
        // Parse attributes of <SceneObject> element
        const char *nameAttr = sceneObjElement->Attribute("Name");
        const char *objectIdAttr = sceneObjElement->Attribute("ObjectId");
        const char *parentIdAttr = sceneObjElement->Attribute("ParentId");
        if (!nameAttr || !objectIdAttr || !parentIdAttr)
        {
            std::cerr << "Missing attributes in <SceneObject> element" << std::endl;
            return nullptr;
        }

        std::vector<unsigned int> childrenIds;
        tinyxml2::XMLElement *childrenIdsElement = sceneObjElement->FirstChildElement("ChildrenIds");
        if (childrenIdsElement)
        {
            for (tinyxml2::XMLElement *childIdElement = childrenIdsElement->FirstChildElement("ChildId"); childIdElement; childIdElement = childIdElement->NextSiblingElement("ChildId"))
            {
                int childId = childIdElement->IntText();
                childrenIds.push_back(childId);
            }
        }

        std::string name = nameAttr;
        int objectId = std::stoi(objectIdAttr);
        int parentId = std::stoi(parentIdAttr);

        // Parse <Transform> element
        Transform transform;
        tinyxml2::XMLElement *transformElement = sceneObjElement->FirstChildElement("Transform");
        if (transformElement)
        {
            // Parse <Position> element
            tinyxml2::XMLElement *positionElement = transformElement->FirstChildElement("Position");
            if (positionElement)
            {
                float posX = positionElement->FloatAttribute("x");
                float posY = positionElement->FloatAttribute("y");
                float posZ = positionElement->FloatAttribute("z");
                transform.setPosition(glm::vec3(posX, posY, posZ)); // Example using glm for vectors
            }

            // Parse <Scale> element
            tinyxml2::XMLElement *scaleElement = transformElement->FirstChildElement("Scale");
            if (scaleElement)
            {
                float scaleX = scaleElement->FloatAttribute("x");
                float scaleY = scaleElement->FloatAttribute("y");
                float scaleZ = scaleElement->FloatAttribute("z");
                transform.setScale(glm::vec3(scaleX, scaleY, scaleZ)); // Example using glm for vectors
            }

            // Parse <Rotation> element
            tinyxml2::XMLElement *rotationElement = transformElement->FirstChildElement("Rotation");
            if (rotationElement)
            {
                float rotX = rotationElement->FloatAttribute("x");
                float rotY = rotationElement->FloatAttribute("y");
                float rotZ = rotationElement->FloatAttribute("z");

                transform.setRotation(rotX, rotY, rotZ);
            }
        }

        // Parse <ModelInfo> element (if exists)
        Model *model = nullptr;
        tinyxml2::XMLElement *modelInfoElement = sceneObjElement->FirstChildElement("ModelInfo");
        if (modelInfoElement)
        {
            const char *modelPathAttr = modelInfoElement->Attribute("ModelPath");
            const char *texturePathAttr = modelInfoElement->Attribute("TexturePath");
            if (modelPathAttr || texturePathAttr)
            {

                // Parse <Transform> element inside <ModelInfo>
                Transform modelTransform;
                tinyxml2::XMLElement *modelTransformElement = modelInfoElement->FirstChildElement("Transform");
                if (modelTransformElement)
                {
                    // Parse <Position> element
                    tinyxml2::XMLElement *modelPositionElement = modelTransformElement->FirstChildElement("Position");
                    if (modelPositionElement)
                    {
                        float posX = modelPositionElement->FloatAttribute("x");
                        float posY = modelPositionElement->FloatAttribute("y");
                        float posZ = modelPositionElement->FloatAttribute("z");
                        modelTransform.setPosition(glm::vec3(posX, posY, posZ)); // Example using glm for vectors
                    }

                    // Parse <Scale> element
                    tinyxml2::XMLElement *modelScaleElement = modelTransformElement->FirstChildElement("Scale");
                    if (modelScaleElement)
                    {
                        float scaleX = modelScaleElement->FloatAttribute("x");
                        float scaleY = modelScaleElement->FloatAttribute("y");
                        float scaleZ = modelScaleElement->FloatAttribute("z");
                        modelTransform.setScale(glm::vec3(scaleX, scaleY, scaleZ)); // Example using glm for vectors
                    }

                    // Parse <Rotation> element
                    tinyxml2::XMLElement *modelRotationElement = modelTransformElement->FirstChildElement("Rotation");
                    if (modelRotationElement)
                    {
                        float rotX = modelRotationElement->FloatAttribute("x");
                        float rotY = modelRotationElement->FloatAttribute("y");
                        float rotZ = modelRotationElement->FloatAttribute("z");

                        modelTransform.setRotation(rotX, rotY, rotZ);
                    }

                    model = aApp.GetModel(std::filesystem::path(modelPathAttr));
                    if (nullptr == model)
                    {
                        model = new Model(modelPathAttr, texturePathAttr, modelTransform, aPipeline.GetVulkanContext());
                    }
                }
            }
        }

        // Create SceneObject and set its attributes
        SceneObject *sceneObj = new SceneObject(objectId, parentId, name, transform, model);

        if (nullptr != model)
        {
            aApp.AddModel(model);
        }

        // Add this SceneObject to its parent (if parentId is not 0)
        if (parentId != 0 && parentObj)
        {
            parentObj->AddChild(sceneObj);
        }

        // Recursively parse children <SceneObject> elements
        for (tinyxml2::XMLElement *childElement = sceneObjElement->FirstChildElement("SceneObject"); childElement; childElement = childElement->NextSiblingElement("SceneObject"))
        {
            ParseSceneObjectRecursive(childElement, sceneObj, aApp, aPipeline);
        }

        return sceneObj;
    }

    Scene *LoadSceneXml(std::filesystem::path aPath, VulkanApp &app, VulkanPipeLine &aPipeline)
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(aPath.c_str()) != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Failed to load XML file: " << aPath << std::endl;
            return nullptr;
        }

        tinyxml2::XMLElement *sceneElement = doc.FirstChildElement("Scene");
        if (!sceneElement)
        {
            std::cerr << "No <Scene> element found in XML file: " << aPath << std::endl;
            return nullptr;
        }

        Scene *scene = new Scene();

        // Parse each SceneObject in the Scene
        for (tinyxml2::XMLElement *sceneObjectElement = sceneElement->FirstChildElement("SceneObject"); sceneObjectElement; sceneObjectElement = sceneObjectElement->NextSiblingElement("SceneObject"))
        {
            SceneObject *sceneObject = ParseSceneObjectRecursive(sceneObjectElement, nullptr, app, aPipeline);
            scene->AddSceneObject(sceneObject);
        }

        printf("Scene Loaded: %s .\n", aPath.c_str());
        return scene;
    }

    void SaveSceneXml(std::filesystem::path aPath, Scene *aScene)
    {

        XMLDocument doc;

        // Create a root element
        XMLNode *root = doc.NewElement("Scene");
        doc.InsertFirstChild(root);

        for (auto *sceneObj : aScene->SceneObjects())
        {
            root->InsertEndChild(CreateSceneObject(doc, sceneObj));
        }

        // Save the document to a file
        XMLError eResult = doc.SaveFile(aPath.c_str());
        if (eResult != XML_SUCCESS)
        {
            std::cout << "Error saving file: " << eResult << std::endl;
            throw std::runtime_error("XML error /|\\ ");
        }
    }

    // Recrusive function
    XMLElement *CreateSceneObject(XMLDocument &doc, SceneObject *sceneObj)
    {

        if (nullptr != sceneObj)
        {
            XMLElement *sceneObjectElement = doc.NewElement("SceneObject");
            sceneObjectElement->SetAttribute("Name", sceneObj->Name().c_str());

            sceneObjectElement->SetAttribute("ObjectId", (int64_t)sceneObj->ObjectId());
            sceneObjectElement->SetAttribute("ParentId", (int64_t)sceneObj->ParentId());

            XMLElement *transformElement = CreateTransform(doc, sceneObj->TransformValue());
            sceneObjectElement->InsertEndChild(transformElement);

            tinyxml2::XMLElement *childrenIdsElement = doc.NewElement("ChildrenIds");
            for (const auto &childId : sceneObj->ChildrenIds())
            {
                tinyxml2::XMLElement *childIdElement = doc.NewElement("ChildId");
                childIdElement->SetText((int64_t)childId);
                childrenIdsElement->InsertEndChild(childIdElement);
            }
            sceneObjectElement->InsertEndChild(childrenIdsElement);

            if (nullptr != sceneObj->ModelPtr())
            {
                XMLElement *modelInfo = CreateModelInfo(doc, sceneObj->ModelPtr());
                sceneObjectElement->InsertEndChild(modelInfo);
            }

            for (auto childe : sceneObj->Children())
            {
                sceneObjectElement->InsertEndChild(CreateSceneObject(doc, childe));
            }

            return sceneObjectElement;
        }
        return nullptr;
    }

    XMLElement *CreateTransform(XMLDocument &doc, Transform aTransform)
    {
        XMLElement *transformElement = doc.NewElement("Transform");

        // Create Position element
        XMLElement *positionElement = doc.NewElement("Position");
        positionElement->SetAttribute("x", aTransform.Position().x);
        positionElement->SetAttribute("y", aTransform.Position().y);
        positionElement->SetAttribute("z", aTransform.Position().z);
        transformElement->InsertEndChild(positionElement);

        // Create Scale element
        XMLElement *scaleElement = doc.NewElement("Scale");
        scaleElement->SetAttribute("x", aTransform.Scale().x);
        scaleElement->SetAttribute("y", aTransform.Scale().y);
        scaleElement->SetAttribute("z", aTransform.Scale().z);
        transformElement->InsertEndChild(scaleElement);

        // Create Rotation element
        XMLElement *rotationElement = doc.NewElement("Rotation");
        rotationElement->SetAttribute("x", aTransform.Rotation().x);
        rotationElement->SetAttribute("y", aTransform.Rotation().y);
        rotationElement->SetAttribute("z", aTransform.Rotation().z);
        transformElement->InsertEndChild(rotationElement);

        return transformElement;
    }

    XMLElement *CreateModelInfo(XMLDocument &doc, Model *aModel)
    {
        XMLElement *modelInfo = doc.NewElement("ModelInfo");

        modelInfo->SetAttribute("ModelPath", aModel->ModelPath().c_str());
        modelInfo->SetAttribute("TexturePath", aModel->TexturePath().c_str());

        XMLElement *modelTransform = CreateTransform(doc, aModel->GetTransform());

        modelInfo->InsertEndChild(modelTransform);

        return modelInfo;
    }
}