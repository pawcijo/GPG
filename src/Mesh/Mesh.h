#pragma once

#include <vector>
#include <filesystem>

#include <glm/glm.hpp>

#include <Utils/DrawMode.h>
#include <Shader.h>
#include <Transform.h>

#include "Common.h"

class App;
class Mesh
{
private:

    std::string mesh_name;

    unsigned int vertices_size;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int VBO;
    unsigned int EBO;

    unsigned color_pick_VAO;
    unsigned color_pick_VBO;

    unsigned int pickingColorID;

    DrawMode drawMode;

    Shader *normalShader;
    Shader *pickingShader;

    // TODO
    // replace with texture vector
    unsigned int funnyTexture;

public:

    std::string Name(){ return mesh_name;}

    void SetShader(Shader *shader);
    void SetPickingShader(Shader *shader);
    void LoadTexture(std::filesystem::path texturePath);
    void SetDrawMode(DrawMode drawMode);

    unsigned int VAO;
    Mesh(std::vector<Vertex> vertices, std::filesystem::path texturePath, Shader *colorPickingShader);
    Mesh(std::vector<Vertex> vertices, unsigned int *indicies, std::filesystem::path texturePath, Shader *colorPickingShader);
    void Draw(Shader &shaderRef, Transform &transfrom, unsigned int objectId, App &app);
    void Draw(Shader &shaderRef, glm::mat4 transform, unsigned int objectId, App &app);
    void Draw_Color(Transform &transform, unsigned int objectId);
    void Draw_Color(glm::mat4 transform, unsigned int objectId);

    ~Mesh();
};
