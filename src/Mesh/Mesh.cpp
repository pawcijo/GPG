#include "Mesh.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <App.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::filesystem::path texturePath, Shader *colorPickingShader)
{

    mesh_name = texturePath.filename();
    vertices_size = vertices.size();

    SetPickingShader(colorPickingShader);

    drawMode = DrawMode::EDefault;
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::float_number * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Vertex::float_number * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // color pick
    {
        glGenVertexArrays(1, &color_pick_VAO);
        glGenBuffers(1, &color_pick_VBO);

        glBindVertexArray(color_pick_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, color_pick_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }

    LoadTexture(texturePath);

    pickingColorID = glGetUniformLocation(pickingShader->shaderProgramID, "PickingColor");
}
Mesh::Mesh(std::vector<Vertex> vertices, unsigned int *indicies, std::filesystem::path texturePath, Shader *colorPickingShader)
{
    vertices_size = vertices.size();

    SetPickingShader(colorPickingShader);

    drawMode = DrawMode::EDefault;
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, Vertex::float_number * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, Vertex::float_number * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    // color pick
    {
        glGenVertexArrays(1, &color_pick_VAO);
        glGenBuffers(1, &color_pick_VBO);

        glBindVertexArray(color_pick_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, color_pick_VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
    }

    LoadTexture(texturePath);

    pickingColorID = glGetUniformLocation(pickingShader->shaderProgramID, "PickingColor");
}

void Mesh::Draw(Shader &shaderRef, Transform &transform, unsigned int objectId, App &app)
{

    glUseProgram(shaderRef.shaderProgramID);

    if (objectId == app.selectedObject)
    {
        shaderRef.setBool("isSelected", true);
    }
    else
    {
        shaderRef.setBool("isSelected", false);
    }

    if (drawMode == DrawMode::EWireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // get matrix's uniform location and set matrix
    unsigned int transformLoc = glGetUniformLocation(shaderRef.shaderProgramID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform.getTransform()));

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, funnyTexture);

    if (0 != indices.size())
    {
        glDrawElements(GL_TRIANGLES, indices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, vertices_size);
    }

    glBindVertexArray(0);
}

void Mesh::SetDrawMode(DrawMode aDrawMode)
{
    drawMode = aDrawMode;
}

void Mesh::Draw(Shader &shaderRef, glm::mat4 transform, unsigned int objectId, App &app)
{

    glUseProgram(shaderRef.shaderProgramID);

    if (objectId == app.selectedObject)
    {
        shaderRef.setBool("isSelected", true);
    }
    else
    {
        shaderRef.setBool("isSelected", false);
    }

    if (drawMode == DrawMode::EWireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // get matrix's uniform location and set matrix
    unsigned int transformLoc = glGetUniformLocation(shaderRef.shaderProgramID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, funnyTexture);

    if (0 != indices.size())
    {
        glDrawElements(GL_TRIANGLES, indices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, vertices_size);
    }

    glBindVertexArray(0);
}

void Mesh::Draw_Color(Transform &transform, unsigned int objectId)
{

    if (nullptr == pickingShader)
    {
        printf("Cos ty odjebal? picking shader not set.\n");
        return;
    }
    glUseProgram(pickingShader->shaderProgramID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // get matrix's uniform location and set matrix
    unsigned int transformLoc = glGetUniformLocation(pickingShader->shaderProgramID, "transform");

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform.getTransform()));

    // Convert "i", the integer mesh ID, into an RGB color
    int r = (objectId & 0x000000FF) >> 0;
    int g = (objectId & 0x0000FF00) >> 8;
    int b = (objectId & 0x00FF0000) >> 16;

    // OpenGL expects colors to be in [0,1], so divide by 255.
    glUniform4f(pickingColorID, r / 255.0f, b / 255.0f, b / 255.0f, 1.0f);

    // Draw mesh
    glBindVertexArray(color_pick_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Mesh::Draw_Color(glm::mat4 transform, unsigned int objectId)
{

    if (nullptr == pickingShader)
    {
        printf("Cos ty odjebal? picking shader not set.\n");
        return;
    }
    glUseProgram(pickingShader->shaderProgramID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // get matrix's uniform location and set matrix
    unsigned int transformLoc = glGetUniformLocation(pickingShader->shaderProgramID, "transform");

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    // Convert "i", the integer mesh ID, into an RGB color
    int r = (objectId & 0x000000FF) >> 0;
    int g = (objectId & 0x0000FF00) >> 8;
    int b = (objectId & 0x00FF0000) >> 16;

    // OpenGL expects colors to be in [0,1], so divide by 255.
    glUniform4f(pickingColorID, r / 255.0f, b / 255.0f, b / 255.0f, 1.0f);

    // Draw mesh
    glBindVertexArray(color_pick_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Mesh::SetShader(Shader *aShader)
{
    normalShader = aShader;
}
void Mesh::SetPickingShader(Shader *aShader)
{
    pickingShader = aShader;
}

void Mesh::LoadTexture(std::filesystem::path texturePath)
{

    // load and create a texture
    // -------------------------
    glGenTextures(1, &funnyTexture);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrComponents, 0);

    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, funnyTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texturePath.c_str() << std::endl;
        stbi_image_free(data);
    }
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    printf("Mesh removed");
}