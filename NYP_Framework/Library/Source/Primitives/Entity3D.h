/**
 CEntity3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#pragma once

 // Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

#include <includes\glm.hpp>

// Include CCollider
#include "Collider.h"

// Include Settings
#include "..\GameControl\Settings.h"

// Include LevelOfDetails
#include "LevelOfDetails.h"

class CEntity3D : public CLevelOfDetails
{
public:
	enum TYPE
	{
		PLAYER,
		NPC,
		PROJECTILE,
		STRUCTURE,	// Buildings etc
		OTHERS,		// Default value: Other entities such as ground and skybox
		NUM_TYPES
	};

	// The handle to the CCollider class instance
	CCollider* cCollider;

	// Constructor(s)
    CEntity3D(void);

	// Destructor
	virtual ~CEntity3D(void);

	// Init
	virtual bool Init(void);

	// Set a shader to this class instance
	virtual void SetShader(const std::string& _name);

	// Set model
	virtual void SetModel(glm::mat4 model) = 0;
	// Set view
	virtual void SetView(glm::mat4 view) = 0;
	// Set projection
	virtual void SetProjection(glm::mat4 projection) = 0;


	// Set Methods
	virtual void SetTextureID(const GLuint iTextureID);
	virtual void SetType(const TYPE eType);
	virtual void SetPosition(const glm::vec3 vec3Position);
	virtual void SetPreviousPosition(const glm::vec3 vec3PreviousPosition);
	virtual void SetFront(const glm::vec3 vec3Front);
	virtual void SetScale(const glm::vec3 vec3Scale);
	virtual void SetRotation(const float fRotationAngle, const glm::vec3 vec3RotationAxis);
	virtual void SetColour(const glm::vec4 vec4Colour);
	virtual void SetMovementSpeed(const float fMovementSpeed);
	virtual void SetColliderScale(const glm::vec3 vec3Scale);

	// Get Methods
	virtual const GLuint GetTextureID(void) const;
	virtual const CEntity3D::TYPE GetType(void) const;
	virtual const glm::vec3 GetPosition(void) const;
	virtual const glm::vec3 GetPreviousPosition(void) const;
	virtual const glm::vec3 GetFront(void) const;
	virtual const glm::vec3 GetScale(void) const;
	virtual const glm::vec3 GetColliderScale() const;
	virtual const float GetRotationAngle(void) const;
	virtual const glm::vec3 GetRotationAxis(void) const;
	virtual const glm::vec4 GetColour(void) const;
	virtual const float GetMovementSpeed(void) const;

	// These methods are for marking this CEntity3D for deletion
	virtual void SetToDelete(const bool bToDelete);
	virtual const bool IsToDelete(void) const;

	// Activate the CCollider for this class instance
	virtual void ActivateCollider(const std::string& _name);

	// Get Sphere Radius
	virtual float GetSphereRadius(void);
	// Check for collision with another CCollider
	virtual bool CheckForCollision(const CEntity3D* cEntity3D);
	// Store position for rollback
	virtual void StorePositionForRollback(void);
	// Rollback the position to the previous position
	virtual void RollbackPosition(void);

	// Update this class instance
	virtual void Update(const double dElapsedTime) = 0;

	// PreRender
	virtual void PreRender(void) = 0;
	// Render
	virtual void Render(void) = 0;
	// PostRender
	virtual void PostRender(void) = 0;

protected:
	// The handle to the CSettings instance
	CSettings* cSettings;

	// Name of Shader Program instance
	std::string sShaderName;

	// OpenGL objects
	GLuint VAO, VBO, IBO;
	GLuint index_buffer_size;

	// The texture ID in OpenGL
	GLuint iTextureID;

	// Render Settings
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

	// Entity Type
	TYPE eType;

	// Position
	glm::vec3 vec3Position;
	// Previous Position
	glm::vec3 vec3PreviousPosition;
	// Direction
	glm::vec3 vec3Front;
	// Scale
	glm::vec3 vec3Scale;
	glm::vec3 vec3ColliderScale;
	// Rotation angle and axis
	float fRotationAngle;
	glm::vec3 vec3RotationAxis;
	// Colour
	glm::vec4 vec4Colour;
	// Movement speed
	float fMovementSpeed;

	// Boolean flag to indicate if this CEntity3D is to be deleted
	bool bToDelete;

	// glm::vec3 variables use during for checking of collision
	glm::vec3 tempVec3A_BottomLeft;
	glm::vec3 tempVec3A_TopRight;
	glm::vec3 tempVec3B_BottomLeft;
	glm::vec3 tempVec3B_TopRight;

	// Load Ground textures
	virtual int LoadTexture(const char* filename);

	// Distance Squared between 2 Vector3 positions
	virtual double DistanceSquaredBetween(glm::vec3 thisVector, glm::vec3 thatVector);
};
