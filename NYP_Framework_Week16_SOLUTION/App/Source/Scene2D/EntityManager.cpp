/**
 ItemSpawner2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "EntityManager.h"

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
CEntityManager2D::CEntityManager2D(void)
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
CEntityManager2D::~CEntityManager2D(void)
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
bool CEntityManager2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();
	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	cSettings = CSettings::GetInstance();

	cMap2D = CMap2D::GetInstance();

	for (int i = 0; i < 100; ++i)
	{
		entities.push_back(nullptr);
	}


	return true;
}

/**
 @brief Update this instance
 */
void CEntityManager2D::Update(const double dElapsedTime)
{

	std::vector<CEntity2D*>::iterator it;
	for (it = entities.begin(); it != entities.end(); ++it)
	{
		if ((*it) == nullptr) continue;
		if ((*it)->dead)
		{
			delete (*it);
			(*it) = nullptr;
			continue;
		}

		(*it)->Update(dElapsedTime);

		//Collision
		for (auto& coll : entities)
		{
			if (coll != nullptr && coll != (*it))
			{
				float entity_x = (*it)->i32vec2Index.x + (*it)->i32vec2NumMicroSteps.x * 0.25f;
				float entity_y = (*it)->i32vec2Index.y + (*it)->i32vec2NumMicroSteps.y * 0.25f;
				float coll_x = coll->i32vec2Index.x + coll->i32vec2NumMicroSteps.x * 0.25f;
				float coll_y = coll->i32vec2Index.y + coll->i32vec2NumMicroSteps.y * 0.25f;
				float width = 1.0f;
				if (entity_x <= coll_x + width &&
					entity_x + width > coll_x &&
					entity_y <= coll_y + width &&
					entity_y + width > coll_y)
				{
					//Collision Detected
					coll->CollidedWith((*it));
					(*it)->CollidedWith(coll);
				}
			}
		}
	}
}

void CEntityManager2D::AddEntity(CEntity2D* entity)
{
	for (auto& tE : entities) {
		if (tE == nullptr) {
			tE = entity;
			break;
		}
	}
}

void CEntityManager2D::RenderEntities()
{
	for (auto& entity : entities)
	{
		if (entity != nullptr && !entity->dead)
		{
			entity->PreRender();
			entity->Render();
			entity->PostRender();
		}
	}
}

void CEntityManager2D::Exit()
{
	for (auto& entity : entities)
	{
		if (entity)
		{
			delete entity;
			entity = nullptr;
		}
	}
}



