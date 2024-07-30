/* Copyright (c) <2003-2022> <Newton Game Dynamics>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely
 */

#pragma once

#include "PhysicsUtils.hpp"
#include "PhysicsEntityManager.hpp"

#include <ndBodyTriggerVolume.h>

namespace GPGVulkan
{

	class ndArchimedesBuoyancyVolume : public ndBodyTriggerVolume
	{
	public:
		D_CLASS_REFLECTION(ndArchimedesBuoyancyVolume);
		ndArchimedesBuoyancyVolume();
		ndArchimedesBuoyancyVolume(const ndLoadSaveBase::ndLoadDescriptor &desc);

		void CalculatePlane(ndBodyKinematic *const body);
		void OnTriggerEnter(ndBodyKinematic *const body, ndFloat32 timestep);
		void OnTrigger(ndBodyKinematic *const kinBody, ndFloat32 timestep);
		void OnTriggerExit(ndBodyKinematic *const body, ndFloat32 timestep);
		virtual void Save(const ndLoadSaveBase::ndSaveDescriptor &desc) const;

		ndPlane m_plane;
		ndFloat32 m_density;
		bool m_hasPlane;
	};
}
