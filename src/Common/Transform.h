#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <string>
#include <vector>

#include <GLFW/glfw3.h>

class Transform
{

    glm::mat4 mTransform;

    glm::vec3 mPosition;
    glm::vec3 mScale;
    glm::vec3 mRotation;

    glm::mat4 mRotationMatrix;
    glm::quat mQuaternion;

public:
    Transform();

    void scaleTransform(GLfloat x, GLfloat y, GLfloat z);
    void setTransform(glm::mat4 transform);

    void setRotation(glm::mat3 rotation);
    void setRotation(glm::vec3 rotation);
    void setRotation(float x, float y, float z);

    void setPosition(glm::vec3 position);
    void setPosition(float x, float y, float z);

    void setScale(glm::vec3 aScale);
    void setScale(float x, float y, float z);

    static Transform origin();

    Transform combine(Transform &other);
    void translate(glm::vec3 Value);
    void rotate(float value, glm::vec3 axis);
    glm::mat4 &TransformMat4();
    glm::mat4 *TransformPtr();

    void Serialize(std::ofstream &outFile) const;
    void Deserialize(std::ifstream &inFile) ;

    const glm::vec3 Position() const;
    glm::vec3 &PositionRef();
    const glm::mat4 RotationMatrix() const;
    const glm::vec3 Rotation() const;
    const glm::vec3 Scale() const;
};