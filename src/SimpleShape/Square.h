#pragma once

#include <Shader.h>

#include <Utils/DrawMode.h>

namespace SquareSpace{

  static float vertices[] = {
         0.5f,  0.5f, 0.0f,    0.9f,  0.1f, 0.1f, // top right
         0.5f, -0.5f, 0.0f,   0.1f,  0.9f, 0.1f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.1f,  0.1f, 0.9f, // bottom left
        -0.5f,  0.5f, 0.0f,    0.5f,  0.5f, 0.0f, // top left 
    };

 static unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

}
class Square
{
    //Vertex Array buffer
    unsigned VAO; //simply verticies
    // Vertex Buffer object
    unsigned VBO; //simpy object of verticies
    // Element buffer object
    unsigned EBO; // simply object of indicies

     DrawMode mDrawmode;

public:
    Square(DrawMode drawmode = DrawMode::EDefault);
    ~Square();
    void Draw(Shader *shaderProgram);
    void SetDrawMode(DrawMode drawMode);
};