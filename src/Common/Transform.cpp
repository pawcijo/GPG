#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

Transform::Transform()
{

    position = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    quaternion = glm::quat(rotation);

    glm::mat4 positionMatrix = glm::mat4(1.0f);
    positionMatrix = glm::scale(positionMatrix, scale);
    positionMatrix = glm::translate(positionMatrix,
                                    glm::vec3(position.x, position.y, position.z));

    transform = glm::mat4(1.0f);
}

Transform Transform::origin() { return Transform(); }

void Transform::Serialize(std::ofstream &outFile) const
{
}

void Transform::Deserialize(std::ifstream &inFile)
{

}

void Transform::setScale(glm::vec3 aScale)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);

    scale = glm::vec3(aScale.x, aScale.y, aScale.z);
    positionMatrix = glm::scale(positionMatrix, glm::vec3(aScale.x, aScale.y, aScale.z));
    positionMatrix =
        glm::translate(positionMatrix, glm::vec3(position.x, position.y, position.z));

    transform = positionMatrix;
    setRotation(rotation.x, rotation.y, rotation.z);
}

void Transform::setScale(float x, float y, float z)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);

    scale = glm::vec3(x, y, z);
    positionMatrix = glm::scale(positionMatrix, glm::vec3(x, y, z));
    positionMatrix =
        glm::translate(positionMatrix, glm::vec3(position.x, position.y, position.z));

    transform = positionMatrix;
    setRotation(rotation.x, rotation.y, rotation.z);
}

void Transform::setPosition(glm::vec3 aPosition)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);
    position = glm::vec3(aPosition.x, aPosition.y, aPosition.z);

    positionMatrix = glm::scale(positionMatrix, scale);
    positionMatrix = glm::translate(positionMatrix,
                                    glm::vec3(aPosition.x, aPosition.y, aPosition.z));

    transform = positionMatrix;
}

void Transform::setPosition(float x, float y, float z)
{

    glm::mat4 positionMatrix = glm::mat4(1.0f);

    position = glm::vec3(x, y, z);

    positionMatrix = glm::scale(positionMatrix, scale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(x, y, z));

    transform = positionMatrix;
    setRotation(rotation.x, rotation.y, rotation.z);
}

Transform Transform::combine(Transform &other)
{
    Transform t;
    t.transform = other.transform * transform;
    return t;
}
glm::mat4 *Transform::getTransformPtr() { return &transform; }

glm::mat4 &Transform::getTransform() { return transform; }

void Transform::scaleTransform(GLfloat x, GLfloat y, GLfloat z)
{
    scale *= glm::vec3(x, y, z);
    transform = glm::scale(transform, glm::vec3(x, y, z));
}

void Transform::translate(glm::vec3 Value)
{
    position += Value;
    transform = glm::translate(transform, Value);
}

void Transform::rotate(float value, glm::vec3 axis)
{
    glm::vec3 rotationDelta = value * axis;

    rotation += rotationDelta;

    rotation.x = fmod(rotation.x, 360.0f);
    rotation.y = fmod(rotation.y, 360.0f);
    rotation.z = fmod(rotation.z, 360.0f);

    transform = glm::rotate(transform, glm::radians(value), axis);
    quaternion = glm::toQuat(transform);
    rotationMatrix = glm::mat4_cast(quaternion);
}

void Transform::setRotation(float x, float y, float z)
{

    glm::mat4 positionMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(x, y, z);

    float pie = glm::pi<float>();

    positionMatrix = glm::scale(positionMatrix, scale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(position.x, position.y, position.z));
    positionMatrix = positionMatrix * glm::eulerAngleXYZ((x / 180.0f) * pie, (y / 180.0f) * pie, (z / 180.0f) * pie);

    transform = positionMatrix;
    quaternion = glm::toQuat(transform);
    rotationMatrix = glm::mat4_cast(quaternion);
}

void Transform::setRotation(glm::mat3 rotation)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);
    float pie = glm::pi<float>();

    glm::mat4 rotationMatrix = glm::mat4(rotation);

    positionMatrix = glm::scale(positionMatrix, scale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(position.x, position.y, position.z));

    positionMatrix = positionMatrix * rotationMatrix;

    transform = positionMatrix;
    quaternion = glm::toQuat(transform);
    rotationMatrix = glm::mat4_cast(quaternion);
}

void Transform::setRotation(glm::vec3 rotation)
{

    glm::mat4 positionMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(rotation.x, rotation.y, rotation.z);

    float pie = glm::pi<float>();

    positionMatrix = glm::scale(positionMatrix, scale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(position.x, position.y, position.z));

    positionMatrix = positionMatrix * (glm::eulerAngleXYZ((rotation.x * 180.0f) / pie, (rotation.y * 180.0f) / pie, (rotation.z * 180.0f) / pie));

    transform = positionMatrix;
    quaternion = glm::toQuat(transform);
    rotationMatrix = glm::mat4_cast(quaternion);
}

void Transform::setTransform(glm::mat4 aTransform) { transform = aTransform; }

glm::vec3 &Transform::getPositionRef() { return position; }

const glm::mat4 Transform::getRotationMatrix() const { return rotationMatrix; }

const glm::vec3 Transform::getPosition() const { return position; }
const glm::vec3 Transform::getRotation() const { return rotation; }
const glm::vec3 Transform::getScale() const { return scale; }