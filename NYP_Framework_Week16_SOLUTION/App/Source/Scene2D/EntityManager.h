/**
 CEntityManager2D
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


class CEntityManager2D : public CSingletonTemplate<CEntityManager2D>
{
	friend CSingletonTemplate<CEntityManager2D>;
public:

	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	void RenderEntities();

	void AddEntity(CEntity2D*);

	void Exit(void);
protected:

	//Collider Codes - To be moved into Collider singleton class when have time
	std::vector<CEntity2D*> entities;

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
	CEntityManager2D(void);

	// Destructor
	virtual ~CEntityManager2D(void);
};

