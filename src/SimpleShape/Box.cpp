#include "Box.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>
#include <filesystem>

Box::Box(std::filesystem::path texturePath, DrawMode aDrawmode)
{
    mDrawmode = aDrawmode;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BoxSpace::vertices), BoxSpace::vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

void Box::SetDrawMode(DrawMode drawMode)
{
    mDrawmode = drawMode;
}

void Box::Draw(Shader *shader)
{

    glUseProgram(shader->shaderProgramID);

    if (mDrawmode == DrawMode::EWireFrame)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // get matrix's uniform location and set matrix
    unsigned int transformLoc = glGetUniformLocation(shader->shaderProgramID, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform.getTransform()));

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, funnyTexture);

    // Draw mesh
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // CLEAR
    glActiveTexture(GL_TEXTURE0);
}

Transform &Box::getTransform()
{
    return transform;
}

Box::~Box()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    printf("Box removed.\n");
}
