/**
 CPhysics2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class CPhysics2D
{
	float GRAVITY_MAGNITUDE = 1.1;
public:
	enum STATUS
	{
		IDLE = 0,
		JUMP,
		FALL,
		NUM_STATUS
	};
	enum GRAVITY_DIRECTION
	{
		GRAVITY_DOWN,
		GRAVITY_UP,
		GRAVITY_RIGHT,
		GRAVITY_LEFT,
		GRAVITY_COUNT
	};
	enum DIRECTION
	{
		UP = 0,
		DOWN = 1,
		LEFT = 2,
		RIGHT = 3,
		NUM_DIRECTIONS
	};

	// Constructor
	CPhysics2D(void);

	// Destructor
	virtual ~CPhysics2D(void);

	// Init
	bool Init(void);

	// Set methods
	void SetInitialVelocity(const glm::vec2 v2InitialVelocity);	// Set Initial velocity
	void SetFinalVelocity(const glm::vec2 v2FinalVelocity);		// Set Final velocity
	void SetAcceleration(const glm::vec2 v2Acceleration);		// Set Acceleration
	void SetDisplacement(const glm::vec2 v2Displacement);		// Set Displacement
	void SetTime(const float fTime);							// Set Time
	void SetStatus(const STATUS sStatus);						// Set Status
	void SetGravityDirection(const GRAVITY_DIRECTION sGravDir);	// Set Gravity Direction

	// Get methods
	glm::vec2 GetInitialVelocity(void) const;			// Get Initial velocity
	glm::vec2 GetFinalVelocity(void) const;				// Get Final velocity
	glm::vec2 GetAcceleration(void) const;				// Get Acceleration
	glm::vec2 GetDisplacement(void) const;				// Get Displacement
	float GetTime(void) const;							// Get Time
	STATUS GetStatus(void) const;						// Get Status
	glm::vec2 GetDeltaDisplacement(void) const;			// Get Delta Displacement
	GRAVITY_DIRECTION GetGravityDirection(void) const;	// Get Gravity Direction
	void setGravityMagnitude(float mag);
	bool ReachedPeakOfJump(void) const;					// Checks if Peak of Jump has been found relative to Gravity Direction

	glm::vec2 GetGravityDirVector(void) const;			// Get Gravity Direction's Vector
	glm::vec2 GetGravityVector(void) const;				// Get Gravity Vector after Multiplying Dir Vec * Gravity Magnitude

	glm::vec2 GetRelativeDirVector(DIRECTION);			// Get Relative Direction Vector based on Gravity Direction


	// Update
	void Update(void);

	// Add elapsed time
	void AddElapsedTime(const float fElapseTime);

	// Calculate the distance between two vec2 varables
	float CalculateDistance(glm::vec2 source, glm::vec2 destination);

	// PrintSelf
	void PrintSelf(void);

protected:
	// Variables for SUVAT calculations
	glm::vec2 v2InitialVelocity;	// Initial velocity
	glm::vec2 v2FinalVelocity;		// Final velocity
	glm::vec2 v2Acceleration;		// Acceleration
	glm::vec2 v2Displacement;		// Displacement
	glm::vec2 v2PrevDisplacement;	// Previous Displacement
	float fTime;					// Time

	const glm::vec2 v2Gravity = glm::vec2( 0.0f, -1.1f);		// Gravity constant
	GRAVITY_DIRECTION sCurrentGravityDirection;

	STATUS sCurrentStatus;
};

