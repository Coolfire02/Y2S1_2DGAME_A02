/**
 CEnemy2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include shader
#include "RenderControl\shader.h"

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

// Include Settings
#include "GameControl\Settings.h"

// Include Physics2D
#include "Physics2D.h"

// Include Player2D
#include "Player2D.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

// Entity Manager
#include "EntityManager.h"

// Include InventoryManager
#include "InventoryManager.h"

// Include SoundController
#include "..\SoundController\SoundController.h"

class CEnemy2D : public CEntity2D
{
public:

	enum ENEMY_TYPE {
		ENEMY_GOLEM,
		ENEMY_COUNT
	};

	// Constructor
	CEnemy2D(void);

	// Destructor
	virtual ~CEnemy2D(void);

	// Init
	bool Init(ENEMY_TYPE type);

	void CollidedWith(CEntity2D*);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Set the indices of the enemy2D
	void Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis);

	// Set the number of microsteps of the enemy2D
	void Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis);

	// Set the UV coordinates of the enemy2D
	void Setvec2UVCoordinates(const float fUVCoordinate_XAxis, const float fUVCoordinate_YAxis);

	// Get the indices of the enemy2D
	glm::i32vec2 Geti32vec2Index(void) const;

	// Get the number of microsteps of the enemy2D
	glm::i32vec2 Geti32vec2NumMicroSteps(void) const;

	// Set the UV coordinates of the enemy2D
	glm::vec2 Getvec2UVCoordinates(void) const;

	// Set the handle to cPlayer to this class instance
	void SetPlayer2D(CPlayer2D* cPlayer2D);

	// boolean flag to indicate if this enemy is active
	bool bIsActive;

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	enum FSM
	{
		IDLE = 0,
		ATTACK,

		BOMB_SEARCH,
		BOMB_ATTACK,
		RUN,
		NUM_FSM
	};

	ENEMY_TYPE type;
	float enemySpeed;
	// Handle to the CPlayer2D
	CPlayer2D* cPlayer2D;

	// InventoryManager
	CInventoryManager* cInventoryManager;
	// InventoryItem
	CInventoryItem* cInventoryItem;

	glm::i32vec2 i32vec2OldIndex;

	CEntityManager2D* entityManager;

	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// The i32vec2 which stores the indices of the enemy2D in the Map2D
	glm::i32vec2 i32vec2Index;

	// The i32vec2 variable which stores The number of microsteps from the tile indices for the enemy2D. 
	// A tile's width or height is in multiples of these microsteps
	glm::i32vec2 i32vec2NumMicroSteps;

	// The vec2 variable which stores the UV coordinates to render the enemy2D
	glm::vec2 vec2UVCoordinate;

	// The i32vec2 which stores the indices of the destination for enemy2D in the Map2D
	glm::i32vec2 i32vec2Destination;
	// The i32vec2 which stores the direction for enemy2D movement in the Map2D
	glm::i32vec2 i32vec2Direction; //Cannot use since need to account for RelativeDir based off gravity
	//CPhysics2D::DIRECTION relativeDirections[2]; //Directions to move in 2 directions at once

	// Settings
	CSettings* cSettings;

	// Physics

	//CS: Animated Sprite
	CSpriteAnimation* animatedSprites;

	// Current color
	glm::vec4 currentColor;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;

	// Handler to the CSoundController
	CSoundController* cSoundController;



	// Current FSM
	FSM sCurrentFSM;

	// FSM counter - count how many frames it has been in this FSM
	int iFSMCounter;

	// Max count in a state
	const int iMaxFSMCounter = 60;

	// Load a texture
	bool LoadTexture(const char* filename, GLuint& iTextureID);

	// Constraint the enemy2D's position within a boundary
	void Constraint(DIRECTION eDirection);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Check if the enemy2D is in mid-air
	bool IsMidAir(void);

	// Update Jump or Fall
	void UpdateJumpFall(const double dElapsedTime = 0.0166666666666667);

	// Let enemy2D interact with the player
	bool InteractWithPlayer(void);

	// Let enemy interact with the map
	void InteractWithMap(void);

	// Update the health and lives
	void UpdateHealthLives(void);

	// Update direction
	void UpdateDirection(void);

	// Flip horizontal direction. For patrol use only
	void FlipHorizontalDirection(void);

	// Update position
	void UpdatePosition(void);

	// Check if  is at Top Row based on Gravity Dir
	bool IsOnTopRow();

	// Check if  is on Bottom Row based on Gravity Dir
	bool IsOnBottomRow();
};

