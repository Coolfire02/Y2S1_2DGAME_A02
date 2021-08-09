/**
 CPhysics2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Physics2D.h"

#include <iostream>
using namespace std;

glm::vec2 CPhysics2D::v2Gravity = glm::vec2(0.0f, -0.8f);
CPhysics2D::GRAVITY_DIRECTION CPhysics2D::sCurrentGravityDirection = GRAVITY_DIRECTION::GRAVITY_DOWN;
float CPhysics2D::GRAVITY_MAGNITUDE = 1.1f;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::CPhysics2D(void)
	: v2InitialVelocity(glm::vec2(0.0f))
	, v2FinalVelocity(glm::vec2(0.0f))
	, v2Acceleration(glm::vec2(0.0f))
	, v2Displacement(glm::vec2(0.0f))
	, v2PrevDisplacement(glm::vec2(0.0f))
	, fTime(0.0f)
	, sCurrentStatus(STATUS::IDLE)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPhysics2D::~CPhysics2D(void)
{
}

/**
@brief Init Initialise this instance
*/ 
bool CPhysics2D::Init(void)
{
	// Reset these variables
	v2InitialVelocity = glm::vec2(0.0f);
	v2FinalVelocity = glm::vec2(0.0f);
	//v2Acceleration = glm::vec2(0.0f);		// Acceleration does not need to be reset here.
	v2Displacement = glm::vec2(0.0f);
	v2PrevDisplacement = glm::vec2(0.0f);
	fTime = 0.0f;
	return true;
}

void CPhysics2D::setGravityMagnitude(float grav)
{
	GRAVITY_MAGNITUDE = grav;
}

// Set methods
// Set Initial velocity
void CPhysics2D::SetInitialVelocity(const glm::vec2 v2InitialVelocity)
{
	this->v2InitialVelocity = v2InitialVelocity;	// Initial velocity
}

// Set Final velocity
void CPhysics2D::SetFinalVelocity(const glm::vec2 v2FinalVelocity)
{
	this->v2FinalVelocity = v2FinalVelocity;		// Final velocity
}

// Set Acceleration
void CPhysics2D::SetAcceleration(const glm::vec2 v2Acceleration)
{
	this->v2Acceleration = v2Acceleration;		// Acceleration
}

// Set Displacement
void CPhysics2D::SetDisplacement(const glm::vec2 v2Displacement)
{
	this->v2Displacement = v2Displacement;		// Displacement
}

// Set Time
void CPhysics2D::SetTime(const float fTime)
{
	this->fTime = fTime;					// Time
}

// Set Status
void CPhysics2D::SetStatus(const STATUS sStatus)
{
	// If there is a change in status, then reset to default values
	if (sCurrentStatus != sStatus)
	{
		// Reset to default values
		Init();

		// Store the new status
		sCurrentStatus = sStatus;
	}
}

glm::vec2 CPhysics2D::GetGravityDirVector(void)
{
	glm::vec2 vec = glm::vec2(0, -1.f);
	switch (sCurrentGravityDirection)
	{
	case GRAVITY_UP:
		vec.y = 1.f;
		break;
	case GRAVITY_LEFT:
		vec.y = 0.f;
		vec.x = -1.f;
		break;
	case GRAVITY_RIGHT:
		vec.y = 0.f;
		vec.x = 1.f;
		break;
	}
	return vec;
}

glm::vec2 CPhysics2D::GetGravityVector(void)
{
	glm::vec2 vec = GetGravityDirVector();
	vec *= GRAVITY_MAGNITUDE;
	return vec;
}

CPhysics2D::GRAVITY_DIRECTION CPhysics2D::GetGravityDirection()
{
	return sCurrentGravityDirection;
}

void CPhysics2D::SetGravityDirection(CPhysics2D::GRAVITY_DIRECTION dir)
{
	sCurrentGravityDirection = dir;
}

bool CPhysics2D::ReachedPeakOfJump(void) const
{
	switch (sCurrentGravityDirection)
	{
	case GRAVITY_DOWN:
		if (v2InitialVelocity.y <= 0.0f)
			return true;
		break;

	case GRAVITY_UP:
		if (v2InitialVelocity.y >= 0.0f)
			return true;
		break;

	case GRAVITY_LEFT:
		if (v2InitialVelocity.x <= 0.0f)
			return true;
		break;

	case GRAVITY_RIGHT:
		if (v2InitialVelocity.x >= 0.0f)
			return true;
		break;

	}
	return false;
}

glm::vec2 CPhysics2D::GetRelativeDirVector(DIRECTION relativeDir)
{
	glm::vec2 gravityDir = GetGravityDirVector();

	glm::vec3 fwdVector = glm::vec3(gravityDir.x, gravityDir.y, 0);
	fwdVector.x *= -1; fwdVector.y *= -1;
	glm::vec3 rightVector = glm::cross(fwdVector, glm::vec3(0, 0, 1));

	//Set to UP Direction by default
	glm::vec2 moveDirectionInWorld = glm::vec2(fwdVector.x, fwdVector.y);

	switch (relativeDir)
	{
	case LEFT:
		moveDirectionInWorld.x = -rightVector.x;
		moveDirectionInWorld.y = -rightVector.y;
		break;
	case RIGHT:
		moveDirectionInWorld.x = rightVector.x;
		moveDirectionInWorld.y = rightVector.y;
		break;
	case DOWN:
		moveDirectionInWorld.y = -fwdVector.y;
	}
	return moveDirectionInWorld;
}

// Get methods
// Get Initial velocity
glm::vec2 CPhysics2D::GetInitialVelocity(void) const
{
	return v2InitialVelocity;	// Initial velocity
}

// Get Final velocity
glm::vec2 CPhysics2D::GetFinalVelocity(void) const
{
	return v2FinalVelocity;		// Final velocity
}

// Get Acceleration
glm::vec2 CPhysics2D::GetAcceleration(void) const
{
	return v2Acceleration;		// Acceleration
}

// Get Displacement
glm::vec2 CPhysics2D::GetDisplacement(void) const
{
	return v2Displacement;		// Displacement
}

// Get Delta Displacement
glm::vec2 CPhysics2D::GetDeltaDisplacement(void) const
{
	return v2Displacement - v2PrevDisplacement;		// Delta Displacement
}

// Get Time
float CPhysics2D::GetTime(void) const
{
	return fTime;					// Time
}

// Get Status
CPhysics2D::STATUS CPhysics2D::GetStatus(void) const
{
	return sCurrentStatus;
}

// Update
void CPhysics2D::Update(void)
{
	// If the player is in IDLE mode, 
	// then we don't calculate further
	if (sCurrentStatus == IDLE)
		return;

	// Calculate the final velocity
	v2FinalVelocity = v2InitialVelocity + GetGravityVector() * fTime;
	// Calculate the displacement
	v2Displacement = v2FinalVelocity * fTime - 0.5f * GetGravityVector() * fTime * fTime;
	// Update v2InitialVelocity
	v2InitialVelocity = v2FinalVelocity;
}

// Add elapsed time
void CPhysics2D::AddElapsedTime(const float fElapseTime)
{
	fTime += fElapseTime;
}

// Calculate the distance between two vec2 varables
float CPhysics2D::CalculateDistance(glm::vec2 source, glm::vec2 destination)
{
	return glm::length(destination - source);
}

// PrintSelf
void CPhysics2D::PrintSelf(void)
{
	cout << "CPhysics2D::PrintSelf()" << endl;
	cout << "v2InitialVelocity\t=\t" << v2InitialVelocity.x << ", " << v2InitialVelocity.y << endl;
	cout << "v2FinalVelocity\t=\t" << v2FinalVelocity.x << ", " << v2FinalVelocity.y << endl;
	cout << "v2Acceleration\t=\t" << v2Acceleration.x << ", " << v2Acceleration.y << endl;
	cout << "v2Displacement\t=\t" << v2Displacement.x << ", " << v2Displacement.y << endl;
	cout << "fTime\t=\t" << fTime << endl;
}
