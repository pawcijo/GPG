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

#include "PhysicsTransform.h"
#include "PhysicsRaycastData.h"
#include "Common.h"

#include "PhysicsRender.h"

class PhysicsBox
{

public:
	PhysicsTransform local;
	glm::vec3 e; // extent, as in the extent of each OBB axis

	PhysicsBox *next;
	class PhysicsBody *body;
	float friction;
	float restitution;
	float density;
	int broadPhaseIndex;
	mutable void *userData;
	mutable bool sensor;

	void SetUserdata(void *data) const;
	void *GetUserdata() const;
	void SetSensor(bool isSensor);

	bool TestPoint(const PhysicsTransform &tx, const glm::vec3 &p) const;
	bool Raycast(const PhysicsTransform &tx, PhysicsRaycastData *raycast) const;
	void ComputeAABB(const PhysicsTransform &tx, PhysicsAABB *aabb) const;
	void ComputeMass(PhysicsMassData *md) const;

	// This part will invoke another object to do this
	void Render(const PhysicsTransform &tx, bool awake, PhysicsRender *render) const;
};

class PhysicsBoxDef
{
public:
	PhysicsBoxDef()
	{
		// Common default values
		m_friction = float(0.4);
		m_restitution = float(0.2);
		m_density = float(1.0);
		m_sensor = false;
	}

	void Set(const PhysicsTransform &tx, const glm::vec3 &extents);

	void SetFriction(float friction);
	void SetRestitution(float restitution);
	void SetDensity(float density);
	void SetSensor(bool sensor);
	glm::vec3 GetExtension() { return m_e; }

private:
	PhysicsTransform m_tx;
	glm::vec3 m_e;

	float m_friction;
	float m_restitution;
	float m_density;
	bool m_sensor;

	friend class PhysicsBody;
};

#include "PhysicsBox.inl"