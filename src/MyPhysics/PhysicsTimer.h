#pragma once

class PhysicsTimer
{

public:
    PhysicsTimer(int UpdatesPerSec);

    ~PhysicsTimer();

    /**
     * Reset the time. Do this when the logical update of the game has been ide, ie while loading.
     */
    void Reset();

    /**
     * Check with the time if it is time for another logical update
     * \return true if logic should be updated, else false.
     */
    bool WantUpdate();

    /**
     * Resets various variables that makes the graphics is never frozen.
     */
    void EndUpdateLoop();

    /**
     * Set the number of times per second to update
     * \param UpdatesPerSec
     */
    void SetUpdatesPerSec(int UpdatesPerSec);

    /**
     * Sets the maximum updates in a row.
     * \param alUpdatesPerSec
     */
    void SetMaxUpdates(int alMax);

    /**
     * Get the number of updates per second.
     */
    int GetUpdatesPerSec();
    /**
     *Get the size of each step in seconds.
     */
    float GetStepSize();

    	private:
		void Update();

		double mLocalTime;
		double mLocalTimeAdd;

		int mMaxUpdates;
		int mUpdateCount;
};
