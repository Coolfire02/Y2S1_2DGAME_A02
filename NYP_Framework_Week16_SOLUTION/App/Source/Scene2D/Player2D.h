/**
 CPlayer2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include Physics2D
#include "Physics2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include EntityManager
#include "EntityManager.h"

#include "ItemSpawner.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CEntity2D
{
	friend CSingletonTemplate<CPlayer2D>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	glm::i32vec2 i32vec2OldIndex;
	glm::i32vec2 i32vec2OldMicroIndex;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	CEntityManager2D* cEntityManager2D;

	// Physics
	CPhysics2D cPhysics2D;

	double bombThrowCD;
	double nextSwitchCD;
	double autoSpawnBombCD;

	float jumpBoostCD;

	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	int dJumpCount;

	float jumpCD;

	void SwitchToMap(CPhysics2D::GRAVITY_DIRECTION);

	// InventoryManager
	CInventoryManager* cInventoryManager;
	// InventoryItem
	CInventoryItem* cInventoryItem;

	CItemSpawner2D* cItemSpawner;

	// Count the number of jumps
	int jumpCount;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Load a texture
	bool LoadTexture(const char* filename, GLuint& iTextureID);

	// Constraint the player's position within a boundary
	void Constraint(CPhysics2D::DIRECTION eDirection = CPhysics2D::DIRECTION::LEFT);

	// Check if a position is possible to move into
	bool CheckPosition(CPhysics2D::DIRECTION eDirection);

	// Check if Player is at Top Row based on Gravity Dir
	bool PlayerIsOnTopRow();

	// Check if Player is on Bottom Row based on Gravity Dir
	bool PlayerIsOnBottomRow();

	// Move in a Direction
	void Move(CPhysics2D::DIRECTION eDirection, const double dElapsedTime);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Check if the player is in mid-air
	bool IsMidAir(void);

	// Update Jump or Fall
	void UpdateJumpFall(const double dElapsedTime = 0.0166666666666667);

	// Let player interact with the map
	void InteractWithMap(void);

	// Update the health and lives
	void UpdateHealthLives(void);
};

