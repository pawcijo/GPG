#include "PhysicsTimer.hpp"

#include "SDL.h"

PhysicsTimer::PhysicsTimer(int aUpdatesPerSec)
{
    mMaxUpdates = aUpdatesPerSec;
    mUpdateCount = 0;

    SetUpdatesPerSec(aUpdatesPerSec);
}

PhysicsTimer::~PhysicsTimer()
{
}

void PhysicsTimer::Reset()
{
    mLocalTime = (double)SDL_GetTicks();
}

bool PhysicsTimer::WantUpdate()
{
    ++mUpdateCount;
    if (mUpdateCount > mMaxUpdates)
        return false;

    if (mLocalTime < (double)SDL_GetTicks())
    {
        Update();
        return true;
    }
    return false;
}

void PhysicsTimer::EndUpdateLoop()
{
    if (mUpdateCount > mMaxUpdates)
    {
        Reset();
    }

    mUpdateCount = 0;
}

void PhysicsTimer::SetUpdatesPerSec(int aUpdatesPerSec)
{
    mLocalTimeAdd = 1000.0 / ((double)aUpdatesPerSec);
    Reset();
}

void PhysicsTimer::SetMaxUpdates(int alMax)
{
    mMaxUpdates = alMax;
}

int PhysicsTimer::GetUpdatesPerSec()
{
    return (int)(1000.0 / ((double)mLocalTimeAdd));
}

float PhysicsTimer::GetStepSize()
{
    return ((float)mLocalTimeAdd) / 1000.0f;
}

// Private
void PhysicsTimer::Update()
{
    mLocalTime += mLocalTimeAdd;
}
