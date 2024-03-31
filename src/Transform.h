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

    glm::mat4 transform;
    glm::vec3 Position;
    glm::vec3 Scale;
    glm::vec3 Rotation;
    glm::mat4 Rotation_matrix;
    glm::quat Quaterion;

public:
    Transform();

    void scaleTransform(GLfloat x, GLfloat y, GLfloat z);
    void setTransform(glm::mat4 transform);
    void setRotation(float x, float y, float z);
    void setPosition(float x, float y, float z);
    void setPosition(glm::vec3 position);
    void setScale(float x, float y, float z);

    static Transform origin();

    Transform combine(Transform &other);
    void translate(glm::vec3 Value);
    void rotate(float value, glm::vec3 axis);
    glm::mat4 &getTransform();
    glm::mat4 *getTransformPtr();

    const glm::vec3 getPosition() const;
    glm::vec3 &getPositionRef();
    const glm::vec3 getRotation() const;
    const glm::vec3 getScale() const;
};