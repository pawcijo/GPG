#include "SceneUtils.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ranges>

#include "Vulkan/Model.hpp"

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
        XMLError eResult = doc.SaveFile("output.xml");
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