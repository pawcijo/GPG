#include "PhysicsUtils.hpp"

#include "PhysicsEntity.hpp"
#include "PhysicsEntityManager.hpp"
#include "PhysicsEntityNotify.hpp"
#include "PhysicsWorld.hpp"

#include <ndBodyDynamic.h>
#include <ndBodyKinematic.h>
#include <ndBody.h>

#include <ndSharedPtr.h>

namespace GPGVulkan
{

	ndBodyKinematic *BuildFlatPlane(PhysicsEntityManager * scene, bool optimized)
	{
		auto world = scene->GetWorld();
		ndVector floor[] =
			{
				{200.0f, 0.0f, 200.0f, 1.0f},
				{200.0f, 0.0f, -200.0f, 1.0f},
				{-200.0f, 0.0f, -200.0f, 1.0f},
				{-200.0f, 0.0f, 200.0f, 1.0f},
			};
		ndInt32 index[][3] = {{0, 1, 2}, {0, 2, 3}};

		ndPolygonSoupBuilder meshBuilder;
		meshBuilder.Begin();
		// meshBuilder.LoadPLY("sword.ply");
		// meshBuilder.LoadPLY("static_mesh.ply");
		meshBuilder.AddFaceIndirect(&floor[0].m_x, sizeof(ndVector), 31, &index[0][0], 3);
		meshBuilder.AddFaceIndirect(&floor[0].m_x, sizeof(ndVector), 31, &index[1][0], 3);
		meshBuilder.End(optimized);

		ndShapeInstance plane(new ndShapeStatic_bvh(meshBuilder));
		ndMatrix uvMatrix(ndGetIdentityMatrix());
		uvMatrix[0][0] *= 0.025f;
		uvMatrix[1][1] *= 0.025f;
		uvMatrix[2][2] *= 0.025f;

		// ndSharedPtr<ndDemoMeshInterface>geometry (new ndDemoMesh("box", scene->GetShaderCache(), &plane, "marbleCheckBoard.tga", "marbleCheckBoard.tga", "marbleCheckBoard.tga", 1.0f, uvMatrix));

		ndMatrix matrix(ndGetIdentityMatrix());
		PhysicsEntity *const entity = new PhysicsEntity(matrix, nullptr);

		// entity->SetMesh(geometry);

		auto body = new ndBodyDynamic();
		body->SetNotifyCallback(new PhysicsEntityNotify(scene, entity));
		body->SetMatrix(matrix);
		body->SetCollisionShape(plane);

		ndSharedPtr<ndBody> bodyPtr(body);
		world->AddBody(bodyPtr);

		scene->AddEntity(entity);
		return body;
	}
}