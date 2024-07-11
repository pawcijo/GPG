#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>

Transform::Transform()
{

    mPosition = glm::vec3(0.0f);
    mScale = glm::vec3(1.0f);
    mRotation = glm::vec3(0.0f, 0.0f, 0.0f);
    mQuaternion = glm::quat(mRotation);

    glm::mat4 positionMatrix = glm::mat4(1.0f);
    positionMatrix = glm::scale(positionMatrix, mScale);
    positionMatrix = glm::translate(positionMatrix,
                                    glm::vec3(mPosition.x, mPosition.y, mPosition.z));

    mTransform = glm::mat4(1.0f);
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

    mScale = glm::vec3(aScale.x, aScale.y, aScale.z);
    positionMatrix = glm::scale(positionMatrix, glm::vec3(aScale.x, aScale.y, aScale.z));
    positionMatrix =
        glm::translate(positionMatrix, glm::vec3(mPosition.x, mPosition.y, mPosition.z));

    mTransform = positionMatrix;
    setRotation(mRotation.x, mRotation.y, mRotation.z);
}

void Transform::setScale(float x, float y, float z)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);

    mScale = glm::vec3(x, y, z);
    positionMatrix = glm::scale(positionMatrix, glm::vec3(x, y, z));
    positionMatrix =
        glm::translate(positionMatrix, glm::vec3(mPosition.x, mPosition.y, mPosition.z));

    mTransform = positionMatrix;
    setRotation(mRotation.x, mRotation.y, mRotation.z);
}

void Transform::setPosition(glm::vec3 aPosition)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);
    mPosition = glm::vec3(aPosition.x, aPosition.y, aPosition.z);

    positionMatrix = glm::scale(positionMatrix, mScale);
    positionMatrix = glm::translate(positionMatrix,
                                    glm::vec3(aPosition.x, aPosition.y, aPosition.z));

    mTransform = positionMatrix;
}

void Transform::setPosition(float x, float y, float z)
{

    glm::mat4 positionMatrix = glm::mat4(1.0f);

    mPosition = glm::vec3(x, y, z);

    positionMatrix = glm::scale(positionMatrix, mScale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(x, y, z));

    mTransform = positionMatrix;
    setRotation(mRotation.x, mRotation.y, mRotation.z);
}

Transform Transform::combine(Transform &other)
{
    Transform t;
    t.mTransform = other.mTransform * mTransform;
    return t;
}
glm::mat4 *Transform::TransformPtr() { return &mTransform; }

glm::mat4 &Transform::TransformMat4() { return mTransform; }

void Transform::scaleTransform(GLfloat x, GLfloat y, GLfloat z)
{
    mScale *= glm::vec3(x, y, z);
    mTransform = glm::scale(mTransform, glm::vec3(x, y, z));
}

void Transform::translate(glm::vec3 Value)
{
    mPosition += Value;
    mTransform = glm::translate(mTransform, Value);
}

void Transform::rotate(float value, glm::vec3 axis)
{
    glm::vec3 rotationDelta = value * axis;

    mRotation += rotationDelta;

    mRotation.x = fmod(mRotation.x, 360.0f);
    mRotation.y = fmod(mRotation.y, 360.0f);
    mRotation.z = fmod(mRotation.z, 360.0f);

    mTransform = glm::rotate(mTransform, glm::radians(value), axis);
    mQuaternion = glm::toQuat(mTransform);
    mRotationMatrix = glm::mat4_cast(mQuaternion);
}

void Transform::setRotation(float x, float y, float z)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);
    mRotation = glm::vec3(x, y, z);

    positionMatrix = glm::scale(positionMatrix, mScale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(mPosition.x, mPosition.y, mPosition.z));

    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f),  glm::radians(z), glm::vec3(1.0f, 0.0f, 0.0f)) *
                               glm::rotate(glm::mat4(1.0f),  glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                               glm::rotate(glm::mat4(1.0f),  glm::radians(x), glm::vec3(0.0f, 0.0f, 1.0f));

    mTransform = positionMatrix * rotationMatrix;
    
    mQuaternion = glm::toQuat(mTransform);
    mRotationMatrix = glm::mat4_cast(mQuaternion);
}

void Transform::setRotation(glm::mat3 rotation)
{
    glm::mat4 positionMatrix = glm::mat4(1.0f);
    float pie = glm::pi<float>();

    glm::mat4 rotationMatrix = glm::mat4(rotation);

    positionMatrix = glm::scale(positionMatrix, mScale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(mPosition.x, mPosition.y, mPosition.z));

    positionMatrix = positionMatrix * rotationMatrix;

    mTransform = positionMatrix;
    mQuaternion = glm::toQuat(mTransform);
    rotationMatrix = glm::mat4_cast(mQuaternion);
}

void Transform::setRotation(glm::vec3 rotation)
{

    glm::mat4 positionMatrix = glm::mat4(1.0f);
    rotation = glm::vec3(rotation.x, rotation.y, rotation.z);

    float pie = glm::pi<double>();

    positionMatrix = glm::scale(positionMatrix, mScale);
    positionMatrix = glm::translate(positionMatrix, glm::vec3(mPosition.x, mPosition.y, mPosition.z));

    positionMatrix = positionMatrix * (glm::eulerAngleXYZ((rotation.x * 180.0f) / pie,
                                                          (rotation.y * 180.0f) / pie,
                                                          (rotation.z * 180.0f) / pie));

    mTransform = positionMatrix;
    mQuaternion = glm::toQuat(mTransform);
    mRotationMatrix = glm::mat4_cast(mQuaternion);
}

void Transform::setTransform(glm::mat4 aTransform) { mTransform = aTransform; }

glm::vec3 &Transform::PositionRef() { return mPosition; }

const glm::mat4 Transform::RotationMatrix() const { return mRotationMatrix; }

const glm::vec3 Transform::Position() const { return mPosition; }
const glm::vec3 Transform::Rotation() const { return mRotation; }
const glm::vec3 Transform::Scale() const { return mScale; }