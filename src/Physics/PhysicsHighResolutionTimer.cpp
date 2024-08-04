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

#include "PhysicsHighResolutionTimer.h"

#include <ndCore.h>

static uint64_t m_prevTime = 0;

void dResetTimer()
{
	m_prevTime = ndGetTimeInMicroseconds();
}

ndFloat32 dGetElapsedSeconds()
{
	const ndFloat64 TICKS2SEC = 1.0e-6f;
	ndUnsigned64 microseconds = ndGetTimeInMicroseconds();

	ndFloat32 timeStep = ndFloat32((ndFloat64)(microseconds - m_prevTime) * TICKS2SEC);
	m_prevTime = microseconds;

	return timeStep;
}
