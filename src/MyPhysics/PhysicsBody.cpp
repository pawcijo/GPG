#include "PhysicsBody.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "PhysicsScene.h"
#include "PhysicsContact.h"
#include "PhysicsBroadPhase.h"
#include "PhysicsBox.h"

glm::quat GetQuaterion(const glm::vec3 &axis, float radians)
{
	glm::quat quateron;

	float halfAngle = float(0.5) * radians;
	float s = std::sin(halfAngle);
	quateron.x = s * axis.x;
	quateron.y = s * axis.y;
	quateron.z = s * axis.z;
	quateron.w = std::cos(halfAngle);

	return quateron;
}

//--------------------------------------------------------------------------------------------------
// PhysicsBody
//--------------------------------------------------------------------------------------------------
PhysicsBody::PhysicsBody(const PhysicsBodyDef &def, PhysicsScene *scene)
{
	m_linearVelocity = def.linearVelocity;
	m_angularVelocity = def.angularVelocity;
	m_force = glm::vec3{0};
	m_torque = glm::vec3{0};
	m_q = GetQuaterion(def.axis, def.angle);
	m_tx.rotation = glm::mat3(m_q);
	m_tx.position = def.position;
	m_sleepTime = float(0.0);
	m_gravityScale = def.gravityScale;
	m_layers = def.layers;
	m_userData = def.userData;
	m_scene = scene;
	m_flags = 0;
	m_linearDamping = def.linearDamping;
	m_angularDamping = def.angularDamping;

	if (def.bodyType == eDynamicBody)
		m_flags |= PhysicsBody::eDynamic;

	else
	{
		if (def.bodyType == eStaticBody)
		{
			m_flags |= PhysicsBody::eStatic;
			m_linearVelocity = glm::vec3{0};
			m_angularVelocity = glm::vec3{0};
			m_force = glm::vec3{0};
			m_torque = glm::vec3{0};
		}

		else if (def.bodyType == eKinematicBody)
			m_flags |= PhysicsBody::eKinematic;
	}

	if (def.allowSleep)
		m_flags |= eAllowSleep;

	if (def.awake)
		m_flags |= eAwake;

	if (def.active)
		m_flags |= eActive;

	if (def.lockAxisX)
		m_flags |= eLockAxisX;

	if (def.lockAxisY)
		m_flags |= eLockAxisY;

	if (def.lockAxisZ)
		m_flags |= eLockAxisZ;

	m_boxes = NULL;
	m_contactList = NULL;
}

//--------------------------------------------------------------------------------------------------
const PhysicsBox *PhysicsBody::AddBox(const PhysicsBoxDef &def)
{
	PhysicsAABB aabb;
	PhysicsBox *box = (PhysicsBox *)m_scene->m_heap.Allocate(sizeof(PhysicsBox));
	box->local = def.m_tx;
	box->e = def.m_e;
	box->next = m_boxes;
	m_boxes = box;
	box->ComputeAABB(m_tx, &aabb);

	box->body = this;
	box->friction = def.m_friction;
	box->restitution = def.m_restitution;
	box->density = def.m_density;
	box->sensor = def.m_sensor;

	CalculateMassData();

	m_scene->m_contactManager.m_broadphase.InsertBox(box, aabb);
	m_scene->m_newBox = true;

	return box;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::RemoveBox(const PhysicsBox *box)
{
	assert(box);
	assert(box->body == this);

	PhysicsBox *node = m_boxes;

	bool found = false;
	if (node == box)
	{
		m_boxes = node->next;
		found = true;
	}

	else
	{
		while (node)
		{
			if (node->next == box)
			{
				node->next = box->next;
				found = true;
				break;
			}

			node = node->next;
		}
	}

	// This shape was not connected to this body.
	assert(found);

	// Remove all contacts associated with this shape
	PhysicsContactEdge *edge = m_contactList;
	while (edge)
	{
		PhysicsContactConstraint *contact = edge->constraint;
		edge = edge->next;

		PhysicsBox *A = contact->A;
		PhysicsBox *B = contact->B;

		if (box == A || box == B)
			m_scene->m_contactManager.RemoveContact(contact);
	}

	m_scene->m_contactManager.m_broadphase.RemoveBox(box);

	CalculateMassData();

	m_scene->m_heap.Free((void *)box);
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::RemoveAllBoxes()
{
	while (m_boxes)
	{
		PhysicsBox *next = m_boxes->next;

		m_scene->m_contactManager.m_broadphase.RemoveBox(m_boxes);
		m_scene->m_heap.Free((void *)m_boxes);

		m_boxes = next;
	}

	m_scene->m_contactManager.RemoveContactsFromBody(this);
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::ApplyLinearForce(const glm::vec3 &force)
{
	m_force += force * m_mass;

	SetToAwake();
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::ApplyForceAtWorldPoint(const glm::vec3 &force, const glm::vec3 &point)
{
	m_force += force * m_mass;
	m_torque += glm::cross(point - m_worldCenter, force);

	SetToAwake();
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::ApplyLinearImpulse(const glm::vec3 &impulse)
{
	m_linearVelocity += impulse * m_invMass;

	SetToAwake();
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::ApplyLinearImpulseAtWorldPoint(const glm::vec3 &impulse, const glm::vec3 &point)
{
	m_linearVelocity += impulse * m_invMass;
	m_angularVelocity += m_invInertiaWorld * glm::cross(point - m_worldCenter, impulse);

	SetToAwake();
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::ApplyTorque(const glm::vec3 &torque)
{
	m_torque += torque;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetToAwake()
{
	if (!(m_flags & eAwake))
	{
		m_flags |= eAwake;
		m_sleepTime = float(0.0);
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetToSleep()
{
	m_flags &= ~eAwake;
	m_sleepTime = float(0.0);
	m_linearVelocity = glm::vec3{0};
	m_angularVelocity = glm::vec3{0};
	m_force = glm::vec3{0};
	m_torque = glm::vec3{0};
}

//--------------------------------------------------------------------------------------------------
bool PhysicsBody::IsAwake() const
{
	return m_flags & eAwake ? true : false;
}

//--------------------------------------------------------------------------------------------------
float PhysicsBody::GetMass() const
{
	return m_mass;
}

//--------------------------------------------------------------------------------------------------
float PhysicsBody::GetInvMass() const
{
	return m_invMass;
}

//--------------------------------------------------------------------------------------------------
float PhysicsBody::GetGravityScale() const
{
	return m_gravityScale;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetGravityScale(float scale)
{
	m_gravityScale = scale;
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetLocalPoint(const glm::vec3 &p) const
{
	return TransformMulTranspose(m_tx, p);
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetLocalVector(const glm::vec3 &v) const
{
	return TransformMulTranspose(m_tx.rotation, v);
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetWorldPoint(const glm::vec3 &p) const
{
	return TransformMulTranspose(m_tx, p);
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetWorldVector(const glm::vec3 &v) const
{
	return TransformMulTranspose(m_tx.rotation, v);
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetLinearVelocity() const
{
	return m_linearVelocity;
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetVelocityAtWorldPoint(const glm::vec3 &p) const
{
	glm::vec3 directionToPoint = p - m_worldCenter;
	glm::vec3 relativeAngularVel = glm::cross(m_angularVelocity, directionToPoint);

	return m_linearVelocity + relativeAngularVel;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetLinearVelocity(const glm::vec3 &v)
{
	// Velocity of static bodies cannot be adjusted
	if (m_flags & eStatic)
		assert(false);

	if (glm::dot(v, v) > float(0.0))
	{
		SetToAwake();
	}

	m_linearVelocity = v;
}

//--------------------------------------------------------------------------------------------------
const glm::vec3 PhysicsBody::GetAngularVelocity() const
{
	return m_angularVelocity;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetAngularVelocity(const glm::vec3 v)
{
	// Velocity of static bodies cannot be adjusted
	if (m_flags & eStatic)
		assert(false);

	if (glm::dot(v, v) > float(0.0))
	{
		SetToAwake();
	}

	m_angularVelocity = v;
}

//--------------------------------------------------------------------------------------------------
bool PhysicsBody::CanCollide(const PhysicsBody *other) const
{
	if (this == other)
		return false;

	// Every collision must have at least one dynamic body involved
	if (!(m_flags & eDynamic) && !(other->m_flags & eDynamic))
		return false;

	if (!(m_layers & other->m_layers))
		return false;

	return true;
}

//--------------------------------------------------------------------------------------------------
const PhysicsTransform PhysicsBody::GetTransform() const
{
	return m_tx;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetTransform(const glm::vec3 &position)
{
	m_worldCenter = position;

	SynchronizeProxies();
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetTransform(const glm::vec3 &position, const glm::vec3 &axis, float angle)
{
	m_worldCenter = position;

	m_q = glm::angleAxis(angle, axis);
	m_tx.rotation = glm::mat3_cast(m_q);

	SynchronizeProxies();
}

//--------------------------------------------------------------------------------------------------
int PhysicsBody::GetFlags() const
{
	return m_flags;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetLayers(int layers)
{
	m_layers = layers;
}

//--------------------------------------------------------------------------------------------------
int PhysicsBody::GetLayers() const
{
	return m_layers;
}

//--------------------------------------------------------------------------------------------------
const glm::quat PhysicsBody::GetQuaternion() const
{
	return m_q;
}

//--------------------------------------------------------------------------------------------------
void *PhysicsBody::GetUserData() const
{
	return m_userData;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetLinearDamping(float damping)
{
	m_linearDamping = damping;
}

//--------------------------------------------------------------------------------------------------
float PhysicsBody::GetLinearDamping(float damping) const
{
	return m_linearDamping;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SetAngularDamping(float damping)
{
	m_angularDamping = damping;
}

//--------------------------------------------------------------------------------------------------
float PhysicsBody::GetAngularDamping(float damping) const
{
	return m_angularDamping;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::Render(PhysicsRender *render) const
{
	bool awake = IsAwake();
	PhysicsBox *box = m_boxes;

	while (box)
	{
		box->Render(m_tx, awake, render);
		box = box->next;
	}
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::Dump(FILE *file, int index) const
{
	fprintf(file, "{\n");
	fprintf(file, "\tPhysicsBodyDef bd;\n");

	switch (m_flags & (eStatic | eDynamic | eKinematic))
	{
	case eStatic:
		fprintf(file, "\tbd.bodyType = PhysicsBodyType( %d );\n", eStaticBody);
		break;

	case eDynamic:
		fprintf(file, "\tbd.bodyType = PhysicsBodyType( %d );\n", eDynamicBody);
		break;

	case eKinematic:
		fprintf(file, "\tbd.bodyType = PhysicsBodyType( %d );\n", eKinematicBody);
		break;
	}

	fprintf(file, "\tbd.position.Set( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", m_tx.position.x, m_tx.position.y, m_tx.position.z);

	glm::vec3 axis = glm::vec3(0);
	float angle = 0;

	// WTF
	// m_q = glm::angleAxis(angle,axis);

	fprintf(file, "\tbd.axis.Set( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", axis.x, axis.y, axis.z);
	fprintf(file, "\tbd.angle = float( %.15lf );\n", angle);
	fprintf(file, "\tbd.linearVelocity.Set( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", m_linearVelocity.x, m_linearVelocity.y, m_linearVelocity.z);
	fprintf(file, "\tbd.angularVelocity.Set( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z);
	fprintf(file, "\tbd.gravityScale = float( %.15lf );\n", m_gravityScale);
	fprintf(file, "\tbd.layers = %d;\n", m_layers);
	fprintf(file, "\tbd.allowSleep = bool( %d );\n", m_flags & eAllowSleep);
	fprintf(file, "\tbd.awake = bool( %d );\n", m_flags & eAwake);
	fprintf(file, "\tbd.awake = bool( %d );\n", m_flags & eAwake);
	fprintf(file, "\tbd.lockAxisX = bool( %d );\n", m_flags & eLockAxisX);
	fprintf(file, "\tbd.lockAxisY = bool( %d );\n", m_flags & eLockAxisY);
	fprintf(file, "\tbd.lockAxisZ = bool( %d );\n", m_flags & eLockAxisZ);
	fprintf(file, "\tbodies[ %d ] = scene.CreateBody( bd );\n\n", index);

	PhysicsBox *box = m_boxes;

	while (box)
	{
		fprintf(file, "\t{\n");
		fprintf(file, "\t\tPhysicsBoxDef sd;\n");
		fprintf(file, "\t\tsd.SetFriction( float( %.15lf ) );\n", box->friction);
		fprintf(file, "\t\tsd.SetRestitution( float( %.15lf ) );\n", box->restitution);
		fprintf(file, "\t\tsd.SetDensity( float( %.15lf ) );\n", box->density);
		int sensor = (int)box->sensor;
		fprintf(file, "\t\tsd.SetSensor( bool( %d ) );\n", sensor);
		fprintf(file, "\t\tPhysicsTransform boxTx;\n");
		PhysicsTransform boxTx = box->local;
		glm::vec3 xAxis = glm::vec3(boxTx.rotation[0][0], boxTx.rotation[1][0], boxTx.rotation[2][0]);
		glm::vec3 yAxis = glm::vec3(boxTx.rotation[0][1], boxTx.rotation[1][1], boxTx.rotation[2][1]);
		glm::vec3 zAxis = glm::vec3(boxTx.rotation[0][2], boxTx.rotation[1][2], boxTx.rotation[2][2]);
		fprintf(file, "\t\tPhysicsVec3 xAxis( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", xAxis.x, xAxis.y, xAxis.z);
		fprintf(file, "\t\tPhysicsVec3 yAxis( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", yAxis.x, yAxis.y, yAxis.z);
		fprintf(file, "\t\tPhysicsVec3 zAxis( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", zAxis.x, zAxis.y, zAxis.z);
		fprintf(file, "\t\tboxTx.rotation.SetRows( xAxis, yAxis, zAxis );\n");
		fprintf(file, "\t\tboxTx.position.Set( float( %.15lf ), float( %.15lf ), float( %.15lf ) );\n", boxTx.position.x, boxTx.position.y, boxTx.position.z);
		fprintf(file, "\t\tsd.Set( boxTx, glm::vec3( float( %.15lf ), float( %.15lf ), float( %.15lf ) ) );\n", box->e.x * 2.0f, box->e.y * 2.0f, box->e.z * 2.0f);
		fprintf(file, "\t\tbodies[ %d ]->AddBox( sd );\n", index);
		fprintf(file, "\t}\n");
		box = box->next;
	}

	fprintf(file, "}\n\n");
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::CalculateMassData()
{
	glm::mat3 inertia = PhysicsDiagonal(float(0.0));
	m_invInertiaModel = PhysicsDiagonal(float(0.0));
	m_invInertiaWorld = PhysicsDiagonal(float(0.0));
	m_invMass = float(0.0);
	m_mass = float(0.0);
	float mass = float(0.0);

	if (m_flags & eStatic || m_flags & eKinematic)
	{
		m_localCenter = glm::vec3{0};
		m_worldCenter = m_tx.position;
		int dupa = 1;
		return;
	}

	glm::vec3 lc;
	lc = glm::vec3{0};

	for (PhysicsBox *box = m_boxes; box; box = box->next)
	{
		if (box->density == float(0.0))
			continue;

		PhysicsMassData md;
		box->ComputeMass(&md);
		mass += md.mass;
		inertia += md.inertia;
		lc += md.center * md.mass;
	}

	if (mass > float(0.0))
	{
		m_mass = mass;
		m_invMass = float(1.0) / mass;
		lc *= m_invMass;
		glm::mat3 identity;
		identity = glm::mat3{1};
		inertia -= (identity * glm::dot(lc, lc) - PhysicsOuterProduct(lc, lc)) * mass;
		m_invInertiaModel = glm::inverse(inertia);

		if (m_flags & eLockAxisX)
			MatrixRawIdentity(m_invInertiaModel, 0);

		if (m_flags & eLockAxisY)
			MatrixRawIdentity(m_invInertiaModel, 1);

		if (m_flags & eLockAxisZ)
			MatrixRawIdentity(m_invInertiaModel, 2);
	}
	else
	{
		// Force all dynamic bodies to have some mass
		m_invMass = float(1.0);
		m_invInertiaModel = PhysicsDiagonal(float(0.0));
		m_invInertiaWorld = PhysicsDiagonal(float(0.0));
	}

	m_localCenter = lc;
	m_worldCenter = TransformMul(m_tx, lc);
	int dupa = 1;
}

//--------------------------------------------------------------------------------------------------
void PhysicsBody::SynchronizeProxies()
{
	PhysicsBroadPhase *broadphase = &m_scene->m_contactManager.m_broadphase;

	m_tx.position = m_worldCenter - TransformMul(m_tx.rotation, m_localCenter);

	PhysicsAABB aabb;
	PhysicsTransform tx = m_tx;

	PhysicsBox *box = m_boxes;
	while (box)
	{
		box->ComputeAABB(tx, &aabb);
		broadphase->Update(box->broadPhaseIndex, aabb);
		box = box->next;
	}
}
