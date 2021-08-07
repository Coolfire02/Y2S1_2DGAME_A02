/**
 ItemSpawner2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "ItemSpawner.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include Math
#include "System/MyMath.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

// Include Game Manager
#include "GameManager.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CItemSpawner2D::CItemSpawner2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, cSoundController(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CItemSpawner2D::~CItemSpawner2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;
}

/**
  @brief Initialise this instance
  */
bool CItemSpawner2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();
	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	cSettings = CSettings::GetInstance();

	cMap2D = CMap2D::GetInstance();

	SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::BOMB_SMALL, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));

	if (Math::RandIntMinMax(0, 1) == 0)
	{
		SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::POWERUP_DOUBLEJUMP, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));

	}


	return true;
}

/**
 @brief Update this instance
 */
void CItemSpawner2D::Update(const double dElapsedTime)
{
	
}

void CItemSpawner2D::SpawnObjectOnRandomPlatform(CMap2D::TILE_ID type, glm::vec2 dir)
{
	SpawnObjectOnRandomPlatform(type, dir, static_cast<CMap2D::TILE_ID>(100), static_cast<CMap2D::TILE_ID>(199));
}

void CItemSpawner2D::SpawnObjectOnRandomPlatform(CMap2D::TILE_ID type, glm::vec2 dir, CMap2D::TILE_ID idStart, CMap2D::TILE_ID idEnd)
{
	vector<CCoord2D*> spaceToSpawn;
	for (int x = 1; x < cSettings->NUM_TILES_XAXIS - 1; ++x)
	{
		for (int y = 1; y < cSettings->NUM_TILES_YAXIS - 1; ++y)
		{
			int tileID = cMap2D->GetMapInfo(y, x);
			int tileUPID = cMap2D->GetMapInfo(y + dir.y, x + dir.x);
			if (tileID >= (int)idStart && tileID <= (int)idEnd 
				&& tileUPID == 0)
			{
				spaceToSpawn.push_back(new CCoord2D(x + dir.x, y + dir.y));
			}
		}
	}
	CCoord2D* selected = spaceToSpawn.at(Math::RandIntMinMax(0, spaceToSpawn.size() - 1));
	cMap2D->SetMapInfo(selected->y, selected->x, type);

	for (int i = 0; i < spaceToSpawn.size(); ++i)
	{
		delete spaceToSpawn.at(i);
		spaceToSpawn.at(i) = nullptr;
	}
}

