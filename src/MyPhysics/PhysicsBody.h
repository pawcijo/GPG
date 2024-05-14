//--------------------------------------------------------------------------------------------------
/**
@file	DropBoxes.h

@author	Randy Gaul
@date	11/25/2014

	Copyright (c) 2014 Randy Gaul http://www.randygaul.net

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
		1. The origin of this software must not be misrepresented; you must not
			 claim that you wrote the original software. If you use this software
			 in a product, an acknowledgment in the product documentation would be
			 appreciated but is not required.
		2. Altered source versions must be plainly marked as such, and must not
			 be misrepresented as being the original software.
		3. This notice may not be removed or altered from any source distribution.
*/
//--------------------------------------------------------------------------------------------------


#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "PhysicsTransform.h"
#include <stdio.h>

class PhysicsScene;
class PhysicsRender;
class PhysicsBoxDef;
class PhysicsBox;
class PhysicsBodyDef;
struct PhysicsContactEdge;

enum PhysicsBodyType
{
    eStaticBody,
    eDynamicBody,
    eKinematicBody
};

class PhysicsBody
{
public:
    // Adds a box to this body. Boxes are all defined in local space
    // of their owning body. Boxes cannot be defined relative to one
    // another. The body will recalculate its mass values. No contacts
    // will be created until the next q3Scene::Step( ) call.
    const PhysicsBox *AddBox(const PhysicsBoxDef &def);

    // Removes this box from the body and broadphase. Forces the body
    // to recompute its mass if the body is dynamic. Frees the memory
    // pointed to by the box pointer.
    void RemoveBox(const PhysicsBox *box);

    // Removes all boxes from this body and the broadphase.
    void RemoveAllBoxes();

    void ApplyLinearForce(const glm::vec3 &force);
    void ApplyForceAtWorldPoint(const glm::vec3 &force, const glm::vec3 &point);
    void ApplyLinearImpulse(const glm::vec3 &impulse);
    void ApplyLinearImpulseAtWorldPoint(const glm::vec3 &impulse, const glm::vec3 &point);
    void ApplyTorque(const glm::vec3 &torque);
    void SetToAwake();
    void SetToSleep();
    bool IsAwake() const;
    float GetGravityScale() const;
    void SetGravityScale(float scale);
    const glm::vec3 GetLocalPoint(const glm::vec3 &p) const;
    const glm::vec3 GetLocalVector(const glm::vec3 &v) const;
    const glm::vec3 GetWorldPoint(const glm::vec3 &p) const;
    const glm::vec3 GetWorldVector(const glm::vec3 &v) const;
    const glm::vec3 GetLinearVelocity() const;
    const glm::vec3 GetVelocityAtWorldPoint(const glm::vec3 &p) const;
    void SetLinearVelocity(const glm::vec3 &v);
    const glm::vec3 GetAngularVelocity() const;
    void SetAngularVelocity(const glm::vec3 v);
    bool CanCollide(const PhysicsBody *other) const;
    const PhysicsTransform GetTransform() const;
    int GetFlags() const;
    void SetLayers(int layers);
    int GetLayers() const;
    const glm::quat GetQuaternion() const;
    void *GetUserData() const;

    void SetLinearDamping(float damping);
    float GetLinearDamping(float damping) const;
    void SetAngularDamping(float damping);
    float GetAngularDamping(float damping) const;

    // Manipulating the transformation of a body manually will result in
    // non-physical behavior. Contacts are updated upon the next call to
    // q3Scene::Step( ). Parameters are in world space. All body types
    // can be updated.
    void SetTransform(const glm::vec3 &position);
    void SetTransform(const glm::vec3 &position, const glm::vec3 &axis, float angle);

    // Used for debug rendering lines, triangles and basic lighting

    void Render(PhysicsRender *render) const;

    // Dump this rigid body and its shapes into a log file. The log can be
    // used as C++ code to re-create an initial scene setup.
    /* not neeeded right now*/
    void Dump(FILE *file, int index) const;

    float GetMass() const;
    float GetInvMass() const;

    PhysicsBody *Next() { return m_next; };
    PhysicsBody *Previous() { return m_prev; };

private:
    // m_flags
    enum
    {
        eAwake = 0x001,
        eActive = 0x002,
        eAllowSleep = 0x004,
        eIsland = 0x010,
        eStatic = 0x020,
        eDynamic = 0x040,
        eKinematic = 0x080,
        eLockAxisX = 0x100,
        eLockAxisY = 0x200,
        eLockAxisZ = 0x400,
    };

    glm::mat3 m_invInertiaModel;
    glm::mat3 m_invInertiaWorld;
    float m_mass;
    float m_invMass;
    glm::vec3 m_linearVelocity;
    glm::vec3 m_angularVelocity;
    glm::vec3 m_force;
    glm::vec3 m_torque;
    PhysicsTransform m_tx;
    glm::quat m_q;
    glm::vec3 m_localCenter;
    glm::vec3 m_worldCenter;
    float m_sleepTime;
    float m_gravityScale;
    int m_layers;
    int m_flags;

    PhysicsBox *m_boxes;
    void *m_userData;
    PhysicsScene *m_scene;
    PhysicsBody *m_next;
    PhysicsBody *m_prev;
    int m_islandIndex;

    float m_linearDamping;
    float m_angularDamping;

    PhysicsContactEdge *m_contactList;

    friend class PhysicsScene;
    friend struct PhysicsManifold;
    friend class PhysicsContactManager;
    friend struct PhysicsIsland;
    friend struct PhysicsContactSolver;

    PhysicsBody(const PhysicsBodyDef &def, PhysicsScene *scene);

    void CalculateMassData();
    void SynchronizeProxies();
};

class PhysicsBodyDef
{
public:
    PhysicsBodyDef()
    {
        // Set all initial positions/velocties to zero
        axis = glm::vec3{0};
        angle = float(0.0);
        position = glm::vec3{0};
        linearVelocity = glm::vec3{0};
        angularVelocity = glm::vec3{0};

        // Usually a gravity scale of 1 is the best
        gravityScale = float(1.0);

        // Common default values
        bodyType = eStaticBody;
        layers = 0x000000001;
        userData = NULL;
        allowSleep = true;
        awake = true;
        active = true;
        lockAxisX = false;
        lockAxisY = false;
        lockAxisZ = false;

        linearDamping = float(0.0);
        angularDamping = float(0.1);
    }

    glm::vec3 axis;            // Initial world transformation.
    float angle;               // Initial world transformation. Radians.
    glm::vec3 position;        // Initial world transformation.
    glm::vec3 linearVelocity;  // Initial linear velocity in world space.
    glm::vec3 angularVelocity; // Initial angular velocity in world space.
    float gravityScale;        // Convenient scale values for gravity x, y and z directions.
    int layers;                // Bitmask of collision layers. Bodies matching at least one layer can collide.
    void *userData;            // Use to store application specific data.

    float linearDamping;
    float angularDamping;

    // Static, dynamic or kinematic. Dynamic bodies with zero mass are defaulted
    // to a mass of 1. Static bodies never move or integrate, and are very CPU
    // efficient. Static bodies have infinite mass. Kinematic bodies have
    // infinite mass, but *do* integrate and move around. Kinematic bodies do not
    // resolve any collisions.
    PhysicsBodyType bodyType;

    bool allowSleep; // Sleeping lets a body assume a non-moving state. Greatly reduces CPU usage.
    bool awake;      // Initial sleep state. True means awake.
    bool active;     // A body can start out inactive and just sits in memory.
    bool lockAxisX;  // Locked rotation on the x axis.
    bool lockAxisY;  // Locked rotation on the y axis.
    bool lockAxisZ;  // Locked rotation on the z axis.
};