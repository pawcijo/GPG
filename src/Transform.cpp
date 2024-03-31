#include "Transform.h"
#include <glm/gtx/euler_angles.hpp>

Transform::Transform()
{
    transform = glm::mat4(1.0f);
    Position = glm::vec3(0.0f);
    Scale = glm::vec3(1.0f);
    Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Quaterion = glm::quat(Rotation);
}

Transform Transform::origin() { return Transform(); }

void Transform::setScale(float x, float y, float z)
{
    glm::mat4 position = glm::mat4(1.0f);

    Scale = glm::vec3(x, y, z);
    position = glm::scale(position, glm::vec3(x, y, z));
    position =
        glm::translate(position, glm::vec3(Position.x, Position.y, Position.z));

    transform = position;
    setRotation(Rotation.x, Rotation.y, Rotation.z);
}

void Transform::setPosition(glm::vec3 aPosition)
{
    glm::mat4 position = glm::mat4(1.0f);
    Position = glm::vec3(aPosition.x, aPosition.y, aPosition.z);

    position = glm::scale(position, Scale);
    position = glm::translate(position,
                              glm::vec3(aPosition.x, aPosition.y, aPosition.z));

    transform = position;
}

void Transform::setPosition(float x, float y, float z)
{

    glm::mat4 position = glm::mat4(1.0f);

    glm::vec3 rotation = Rotation;

    Position = glm::vec3(x, y, z);

    position = glm::scale(position, Scale);
    position = glm::translate(position, glm::vec3(x, y, z));

    transform = position;
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
    Scale *= glm::vec3(x, y, z);
    transform = glm::scale(transform, glm::vec3(x, y, z));
}

void Transform::translate(glm::vec3 Value)
{
    Position += Value;
    transform = glm::translate(transform, Value);
}

void Transform::rotate(float value, glm::vec3 axis)
{
    Rotation += value;

    Rotation.x = fmod(Rotation.x, 360.0f);
    Rotation.y = fmod(Rotation.y, 360.0f);
    Rotation.z = fmod(Rotation.z, 360.0f);

    transform = glm::rotate(transform, glm::radians(value), axis);
    Quaterion = glm::toQuat(transform);
}

void Transform::setRotation(float x, float y, float z)
{

    glm::mat4 position = glm::mat4(1.0f);
    Rotation = glm::vec3(x, y, z);

    position = glm::scale(position, Scale);
    position = glm::translate(position, glm::vec3(Position.x, Position.y, Position.z));
    position = position * glm::eulerAngleXYZ((x / 180.0f) * 3.14f, (y / 180.0f) * 3.14f, (z / 180.0f) * 3.14f);

    transform = position;
}

void Transform::setTransform(glm::mat4 aTransform) { transform = aTransform; }

glm::vec3 &Transform::getPositionRef() { return Position; }

const glm::vec3 Transform::getPosition() const { return Position; }
const glm::vec3 Transform::getRotation() const { return Rotation; }
const glm::vec3 Transform::getScale() const { return Scale; }