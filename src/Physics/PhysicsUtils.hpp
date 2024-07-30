#pragma once

class PhysicsEntityManager;
class ndBodyKinematic;

namespace GPGVulkan
{
    ndBodyKinematic *BuildFlatPlane(PhysicsEntityManager * scene, bool optimized);
}