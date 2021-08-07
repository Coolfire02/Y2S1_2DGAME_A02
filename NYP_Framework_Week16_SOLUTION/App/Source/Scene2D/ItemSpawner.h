/**
 CItemSpawner2D
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

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"

// Include Physics2D
#include "Physics2D.h"

// Include CMap2D
#include "Map2D.h"

// Include vector
#include <vector>

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

struct CCoord2D {
	int x;
	int y;
	CCoord2D(int x = 0, int y = 0) : x(x), y(y) {};
};

class CItemSpawner2D : public CSingletonTemplate<CItemSpawner2D>
{
	friend CSingletonTemplate<CItemSpawner2D>;
public:

	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	// Spawn Object on Platform Tiles
	void SpawnObjectOnRandomPlatform(CMap2D::TILE_ID type, glm::vec2 dir);

	void SpawnObjectOnRandomPlatform(CMap2D::TILE_ID type, glm::vec2 dir, CMap2D::TILE_ID idStart, CMap2D::TILE_ID idEnd);

protected:

	glm::i32vec2 i32vec2OldIndex;
	glm::i32vec2 i32vec2OldMicroIndex;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	CSettings* cSettings;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Physics
	CPhysics2D cPhysics2D;

	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// InventoryManager
	CInventoryManager* cInventoryManager;
	// InventoryItem
	CInventoryItem* cInventoryItem;

	// Handler to the CSoundController
	CSoundController* cSoundController;

	// Constructor
	CItemSpawner2D(void);

	// Destructor
	virtual ~CItemSpawner2D(void);
};

