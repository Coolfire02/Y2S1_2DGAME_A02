/**
 Player2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include "System/MyMath.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

// Include Game Manager
#include "GameManager.h"

#include "Bomb2D.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, bombThrowCD(0.0f)
	, jumpBoostCD(0.0f)
	, dJumpCount(0)
	, cItemSpawner(NULL)
	, cSoundController(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	name = "Player";
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cInventoryManager = NULL;

	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cItemSpawner = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(3, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	//Set it to fall upon entering new level
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	//CS: Reset double jump
	jumpCount = 0;

	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();
	nextSwitchCD = Math::RandFloatMinMax(12.5f, 20.f);

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	cItemSpawner = CItemSpawner2D::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(3, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	i32vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Load the player texture
	if (LoadTexture("Image/scene2d_player.png", iTextureID) == false)
	{
		std::cout << "Failed to load player tile texture" << std::endl;
		return false;
	}
	
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(3, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 1);
	animatedSprites->AddAnimation("right", 0, 3);
	animatedSprites->AddAnimation("left", 4, 7);
	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	/*cItemSpawner->SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::BOMB_SMALL, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));

	if (Math::RandIntMinMax(0, 1) == 0)
	{
		cItemSpawner->SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::POWERUP_DOUBLEJUMP, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));

	}*/

	cEntityManager2D = CEntityManager2D::GetInstance();

	// Get the handler to the CInventoryManager instance
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Lives icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Lives", "Image/Scene2D_Lives.tga", 3, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Add a Health icon as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Health", "Image/Scene2D_Health.tga", 1000, 1000);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	return true;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	cInventoryItem = cInventoryManager->GetItem("DoubleJump");
	if (cInventoryItem->GetCount() > 0)
	{
		jumpBoostCD += dElapsedTime * 3;
		if (jumpBoostCD >= 1.0)
		{
			jumpBoostCD = 0.0;
			cInventoryItem->Remove(1);
		}
	}
	if (autoSpawnBombCD > 0.0)
	{
		autoSpawnBombCD -= dElapsedTime;
		unsigned int a, b;
		if (autoSpawnBombCD <= 0)
		{
			if (!cMap2D->FindValue(CMap2D::TILE_ID::BOMB_SMALL, a, b))
			{
				autoSpawnBombCD = Math::RandFloatMinMax(2.f, 5.f);
				cItemSpawner->SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::BOMB_SMALL, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));
			}
		}
		
	}

	if (bombThrowCD > 0.0)
	{
		bombThrowCD -= dElapsedTime;
	}
	if (jumpCD > 0.0)
	{
		jumpCD -= dElapsedTime;
	}
	if (nextSwitchCD > 0.0)
	{
		nextSwitchCD -= dElapsedTime;
		if (nextSwitchCD <= 0)
		{

			//Play warp sound
			CSoundController::GetInstance()->PlaySoundByID(SOUND_TYPE::LEVEL_ROTATION);

			cMap2D->ClearInteractables();
			nextSwitchCD = Math::RandFloatMinMax(5.5f, 18.f);
			std::vector<int> nums;
			nums.push_back(0);
			nums.push_back(1);
			nums.push_back(2);
			nums.push_back(3);
			nums.erase(nums.begin() + (int)cPhysics2D.GetGravityDirection());
			switch (nums.at(Math::RandIntMinMax(0, 2)))
			{
			case 0:
				cPhysics2D.SetGravityDirection(CPhysics2D::GRAVITY_DIRECTION::GRAVITY_DOWN);
				break;
			case 1:
				cPhysics2D.SetGravityDirection(CPhysics2D::GRAVITY_DIRECTION::GRAVITY_UP);
				break;
			case 2:
				cPhysics2D.SetGravityDirection(CPhysics2D::GRAVITY_DIRECTION::GRAVITY_RIGHT);
				break;
			case 3:
				cPhysics2D.SetGravityDirection(CPhysics2D::GRAVITY_DIRECTION::GRAVITY_LEFT);
				break;
			}
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			cMap2D->SetCurrentLevel(cPhysics2D.GetGravityDirection());

			cItemSpawner->SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::BOMB_SMALL, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));
			
			if (Math::RandIntMinMax(0, 1) == 0)
			{
				cItemSpawner->SpawnObjectOnRandomPlatform(CMap2D::TILE_ID::POWERUP_DOUBLEJUMP, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));
			
			}

			glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP);
			relativeDir *= 0.1;
			cPhysics2D.SetInitialVelocity(relativeDir);
		}
	}
	// Get keyboard updates
	if (cKeyboardController->IsKeyDown(GLFW_KEY_SPACE) && jumpCD <= 0.0)
	{
		jumpCD = 0.15f;
		if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			dJumpCount = 1;

			glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP);
			relativeDir *= 0.33;
			cPhysics2D.SetInitialVelocity(relativeDir);
			// Play a sound for jump
			//cSoundController->PlaySoundByID(3);
		}
		else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP ||
			cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL && dJumpCount < 2
			)
		{
			cInventoryItem = cInventoryManager->GetItem("DoubleJump");
			if (cInventoryItem->GetCount() > 0)
			{
				dJumpCount += 1;
				cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);

				glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP);
				relativeDir *= 0.33;
				cPhysics2D.SetInitialVelocity(relativeDir);
				// Play a sound for jump
				//cSoundController->PlaySoundByID(3);
			}
		}
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_R))
	{
		cItemSpawner->SpawnObjectOnRandomPlatform(CMap2D::BOMB_SMALL, cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP));
	}

	

	if (cKeyboardController->IsKeyDown(GLFW_KEY_F))
	{
		cInventoryItem = cInventoryManager->GetItem("Bomb");
		if (cInventoryItem->GetCount() > 0 && bombThrowCD <= 0)
		{
			bombThrowCD = 0.5f;
			cPhysics2D.SetInitialVelocity(glm::vec2(0.f, 0.1f));
			cInventoryItem->Remove(1);
			CBomb2D* bomb = new CBomb2D();
			bomb->Init(cPhysics2D.GetGravityDirection(), i32vec2Index.x, i32vec2Index.y);
			bomb->SetShader("2DColorShader");
			cEntityManager2D->AddEntity(bomb);
		}
		
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
	{
		Move(CPhysics2D::DIRECTION::LEFT, dElapsedTime);
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
	{
		Move(CPhysics2D::DIRECTION::RIGHT, dElapsedTime);
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		//cPhysics2D.SetGravityDirection(CPhysics2D::GRAVITY_DIRECTION::GRAVITY_RIGHT);
		//cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		//cMap2D->SetCurrentLevel(cPhysics2D.GetGravityDirection());

		//glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP);
		//relativeDir *= 0.1 ;
		//cPhysics2D.SetInitialVelocity(relativeDir);
	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
	{

	}

	// Update Jump or Fall
	//CS: Will cause error when debugging. Set to default elapsed time
	UpdateJumpFall(dElapsedTime);

	// Interact with the Map
	InteractWithMap();

	// Update the Health and Lives
	UpdateHealthLives();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, i32vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, i32vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

void CPlayer2D::SwitchToMap(CPhysics2D::GRAVITY_DIRECTION)
{

}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D::Render(void)
{
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtime_color");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
													vec2UVCoordinate.y,
													0.0f));
	transform = glm::rotate(transform, atan2f(CPhysics2D::GetGravityDirVector().y, CPhysics2D::GetGravityDirVector().x) + Math::HALF_PI, glm::vec3(0, 0, 1));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(currentColor));

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	//CS: Render the animated sprite
	animatedSprites->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CPlayer2D::LoadTexture(const char* filename, GLuint& iTextureID)
{
	// Variables used in loading the texture
	int width, height, nrChannels;
	
	// texture 1
	// ---------
	glGenTextures(1, &iTextureID);
	glBindTexture(GL_TEXTURE_2D, iTextureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	CImageLoader* cImageLoader = CImageLoader::GetInstance();
	unsigned char *data = cImageLoader->Load(filename, width, height, nrChannels, true);
	if (data)
	{
		if (nrChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		return false;
	}
	// Free up the memory of the file data read in
	free(data);

	return true;
}

void CPlayer2D::Move(CPhysics2D::DIRECTION eDirection, const double dElapsedTime)
{
	// Store the old position
	i32vec2OldIndex = i32vec2Index;
	i32vec2OldMicroIndex = i32vec2NumMicroSteps;

	glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(eDirection);

	if (relativeDir.x == -1) // "A" Key
	{
		if (i32vec2Index.x >= 0)
		{
			i32vec2NumMicroSteps.x--;
			if (i32vec2NumMicroSteps.x < 0)
			{
				i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				i32vec2Index.x--;
			}
		}
		Constraint(eDirection);

		// If the new position is not feasible, then revert to old position
		if (CheckPosition(eDirection) == false)
		{
			i32vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if player is in mid-air, such as walking off a platform
		if (IsMidAir() == true && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}

		//CS: Play the "left" animation
		animatedSprites->PlayAnimation("left", -1, 1.0f);

		//CS: Change Color
	
	}

	else if (relativeDir.x == 1) // "D" Key
	{
		// Calculate the new position to the right
		if (i32vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x++;

			if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				i32vec2NumMicroSteps.x = 0;
				i32vec2Index.x++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(eDirection);

		// If the new position is not feasible, then revert to old position
		if (CheckPosition(eDirection) == false)
		{
			i32vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if player is in mid-air, such as walking off a platform
		if (IsMidAir() == true && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}

		//CS: Play the "right" animation
		animatedSprites->PlayAnimation("right", -1, 1.0f);

	
	}

	if (relativeDir.y == 1) // "W" Key
	{
		// Calculate the new position up
		if (i32vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			i32vec2NumMicroSteps.y++;
			if (i32vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				i32vec2NumMicroSteps.y = 0;
				i32vec2Index.y++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(eDirection);

		// If the new position is not feasible, then revert to old position
		if (CheckPosition(eDirection) == false)
		{
			i32vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.y = 0;
		}

		// Check if player is in mid-air, such as walking off a platform
		if (IsMidAir() == true && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}

		//CS: Play the "idle" animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);

	}
	else if (relativeDir.y == -1) // "S" Key
	{
		// Calculate the new position down
		if (i32vec2Index.y >= 0)
		{
			i32vec2NumMicroSteps.y--;
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				i32vec2Index.y--;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(eDirection);

		// If the new position is not feasible, then revert to old position
		if (CheckPosition(eDirection) == false)
		{
			i32vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.y = 0;
		}

		// Check if player is in mid-air, such as walking off a platform
		if (IsMidAir() == true && cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}

		//CS: Play the "idle" animation
		animatedSprites->PlayAnimation("idle", -1, 1.0f);


	}

	if (i32vec2Index.x != i32vec2OldIndex.x || i32vec2Index.y != i32vec2OldIndex.y && cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
	{
		cSoundController->GetInstance()->PlaySoundByID(SOUND_TYPE::WALKING_GRASS);
	}

}


/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D::Constraint(CPhysics2D::DIRECTION eDirection)
{
	glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(eDirection);
	if (relativeDir.x == -1)
	{
		if (i32vec2Index.x <= 0)
		{
			i32vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (relativeDir.x == 1)
	{
		if (i32vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (relativeDir.y == 1)
	{
		if (i32vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (relativeDir.y == -1)
	{
		if (i32vec2Index.y <= 0)
		{
			i32vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CPlayer2D::CheckPosition(CPhysics2D::DIRECTION eDirection)
{
	glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(eDirection);
	if (relativeDir.x == -1)
	{

		if (i32vec2Index.x < 0)
		{
			i32vec2NumMicroSteps.x = 0;
			return false;
		}

		// If the new position is between 2 rows, then check both rows as well
		// If the 2 grids are not accessible, then return false
		if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100) ||
			(cMap2D->GetMapInfo(i32vec2Index.y + (i32vec2NumMicroSteps.y > 0 ? 1 : 0), i32vec2Index.x) >= 100))
		{
			return false;
		}
		
	}
	else if (relativeDir.x == 1)
	{

		if (i32vec2Index.x >= cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x = 0;
			return false;
		}

		// If the new position is between 2 rows, then check both rows as well
		//// If the 2 grids are not accessible, then return false
		if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + (i32vec2NumMicroSteps.x > 0 ? 1 : 0)) >= 100) ||
			(cMap2D->GetMapInfo(i32vec2Index.y + (i32vec2NumMicroSteps.y > 0 ? 1 : 0), i32vec2Index.x + (i32vec2NumMicroSteps.x > 0 ? 1 : 0)) >= 100))
		{
			return false;
		}
		


	}
	else if (relativeDir.y == 1)
	{

		if (i32vec2Index.y >= cSettings->NUM_TILES_YAXIS)
		{
			i32vec2NumMicroSteps.y = 0;
			return false;
		}


		// If the 2 grids are not accessible, then return false
		if ((cMap2D->GetMapInfo(i32vec2Index.y + (i32vec2NumMicroSteps.y > 0 ? 1 : 0), i32vec2Index.x) >= 100) ||
			(cMap2D->GetMapInfo(i32vec2Index.y + (i32vec2NumMicroSteps.y > 0 ? 1 : 0), i32vec2Index.x + (i32vec2NumMicroSteps.x > 0 ? 1 : 0)) >= 100))
		{
			return false;
		}
		
	}
	else if (relativeDir.y == -1)
	{

		if (i32vec2Index.y < 0)
		{
			i32vec2NumMicroSteps.y = 0;
			return false;
		}

		//// If the new position is between 2 columns, then check both columns as well
		// If the 2 grids are not accessible, then return false
		if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100) ||
			(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + (i32vec2NumMicroSteps.x > 0 ? 1 : 0)) >= 100))
		{
			return false;
		}
		
	}
	else
	{
		cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}


bool CPlayer2D::PlayerIsOnBottomRow() {
	switch (cPhysics2D.GetGravityDirection())
	{
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_DOWN:
		if (i32vec2Index.y <= 0)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_UP:
		if (i32vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_LEFT:
		if (i32vec2Index.x <= 0)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_RIGHT:
		if (i32vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
			return true;
		break;
	}
	return false;
}

bool CPlayer2D::PlayerIsOnTopRow() {
	switch (cPhysics2D.GetGravityDirection())
	{
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_DOWN:
		if (i32vec2Index.y == cSettings->NUM_TILES_YAXIS - 1)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_UP:
		if (i32vec2Index.y == 0)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_LEFT:
		if (i32vec2Index.x == cSettings->NUM_TILES_XAXIS - 1)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_RIGHT:
		if (i32vec2Index.x == 0)
			return true;
		break;
	}
	return false;
}

// Check if the player is in mid-air
bool CPlayer2D::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (PlayerIsOnBottomRow()) return false;


	// Check if the tile below the player's current position is empty
	glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::DOWN);

	if ((i32vec2NumMicroSteps.x == 0 || i32vec2NumMicroSteps.y == 0) &&
		(cMap2D->GetMapInfo(i32vec2Index.y + relativeDir.y, i32vec2Index.x + relativeDir.x) <= CMap2D::TILE_ID::INTERACTABLES_END))
	{
		return true;
	}

	return false;
}

// Update Jump or Fall
void CPlayer2D::UpdateJumpFall(const double dElapsedTime)
{
	CPhysics2D::STATUS oldStatus = cPhysics2D.GetStatus();
	float fallenMag = 0.0;
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();
		std::cout << v2Displacement.x << " " << v2Displacement.y << std::endl;

		if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_DOWN)
		{
			// Store the current i32vec2Index.y
			int iIndex_YAxis_OLD = i32vec2Index.y;

			// Translate the displacement from pixels to indices
			int iDisplacement = (int)(v2Displacement.y / cSettings->TILE_HEIGHT);
			int iDisplacement_MicroSteps = (int)((v2Displacement.y * cSettings->iWindowHeight) - iDisplacement) /
				(int)cSettings->NUM_STEPS_PER_TILE_YAXIS;
			if (iDisplacement_MicroSteps != 0)
			{
				iDisplacement++;
			}

			iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).y;
			fallenMag = iDisplacement;
			// Update the indices
			i32vec2Index.y += iDisplacement;
			i32vec2NumMicroSteps.y = 0;

			// Constraint the player's position within the screen boundary
			Constraint(CPhysics2D::DIRECTION::UP);

			// Iterate through all rows until the proposed row
			// Check if the player will hit a tile; stop jump if so.
			int iIndex_YAxis_Proposed = i32vec2Index.y;

			if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP)
			{
				for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
				{
					// Change the player's index to the current i value
					i32vec2Index.y = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
					{
						i32vec2Index.y = i + 1;
						// Set the Physics to fall status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						break;
					}
				}
			}
			else {
				for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
				{
					// Change the player's index to the current i value
					i32vec2Index.y = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
					{
						i32vec2Index.y = i - 1;
						// Set the Physics to fall status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						break;
					}
				}
			}
		}
		else if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_LEFT || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
		{
			// Store the current i32vec2Index.x
			int iIndex_YAxis_OLD = i32vec2Index.x;

			// Translate the displacement from pixels to indices
			int iDisplacement = (int)(v2Displacement.x / cSettings->TILE_HEIGHT);
			int iDisplacement_MicroSteps = (int)((v2Displacement.x * cSettings->iWindowHeight) - iDisplacement) /
				(int)cSettings->NUM_STEPS_PER_TILE_YAXIS;
			if (iDisplacement_MicroSteps != 0)
			{
				iDisplacement++;
			}

			iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).x;

			// Update the indices
			i32vec2Index.x += iDisplacement;
			i32vec2NumMicroSteps.x = 0;
			fallenMag = iDisplacement;
			// Constraint the plaxer's position within the screen boundarx
			Constraint(CPhysics2D::DIRECTION::UP);

			// Iterate through all rows until the proposed row
			// Check if the plaxer will hit a tile; stop jump if so.
			int iIndex_YAxis_Proposed = i32vec2Index.x;

			if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
			{
				for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
				{
					// Change the plaxer's index to the current i value
					i32vec2Index.x = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
					{
						i32vec2Index.x = i + 1;
						// Set the Phxsics to fall status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						break;
					}
				}
			}
			else {
				for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
				{
					// Change the plaxer's index to the current i value
					i32vec2Index.x = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
					{
						i32vec2Index.x = i - 1;
						// Set the Phxsics to fall status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
						break;
					}
				}
			}
		}

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.ReachedPeakOfJump()))
		{
			// Set status to fall
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.AddElapsedTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();


		if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_DOWN)
		{
			// Store the current i32vec2Index.y
			int iIndex_YAxis_OLD = i32vec2Index.y;

			// Translate the displacement from pixels to indices
			int iDisplacement = (int)(v2Displacement.y / cSettings->TILE_HEIGHT);
			int iDisplacement_MicroSteps = (int)((v2Displacement.y * cSettings->iWindowHeight) - iDisplacement) /
				(int)cSettings->NUM_STEPS_PER_TILE_YAXIS;
			if (iDisplacement_MicroSteps > 0)
			{
				iDisplacement++;
			}

			// Update the indices
			i32vec2Index.y += iDisplacement;
			i32vec2NumMicroSteps.y = 0;

			iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).y;
			fallenMag = iDisplacement;
			// Constraint the player's position within the screen boundary
			Constraint(CPhysics2D::DIRECTION::DOWN);
			
			
			// Iterate through all rows until the proposed row
			// Check if the player will hit a tile; stop fall if so.
			int iIndex_YAxis_Proposed = i32vec2Index.y;

			if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP)
			{
				//if I = 22, I < 24, i++ (WIll fall backwards, towards UP GRAVITY
				for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
				{
					// Change the player's index to the current i value
					i32vec2Index.y = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						// Revert to the previous position
						if (i != iIndex_YAxis_OLD)
							i32vec2Index.y = i - 1;
						// Set the Physics to idle status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

						dJumpCount = 0;
						break;
					}
				}
			}
			else {
				for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
				{
					// Change the player's index to the current i value
					i32vec2Index.y = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						// Revert to the previous position
						if (i != iIndex_YAxis_OLD)
							i32vec2Index.y = i + 1;
						// Set the Physics to idle status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

						dJumpCount = 0;
						break;
					}
				}
			}
		}
		else if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_LEFT || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
		{
			// Store the current i32vec2Index.x
			int iIndex_XAxis_OLD = i32vec2Index.x;

			// Translate the displacement from pixels to indices
			int iDisplacement = (int)(v2Displacement.x / cSettings->TILE_HEIGHT);
			int iDisplacement_MicroSteps = (int)((v2Displacement.x * cSettings->iWindowHeight) - iDisplacement) /
				(int)cSettings->NUM_STEPS_PER_TILE_XAXIS;
			if (iDisplacement_MicroSteps > 0)
			{
				iDisplacement++;
			}

			// Update the indices
			i32vec2Index.x += iDisplacement;
			i32vec2NumMicroSteps.x = 0;

			iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).x;
			fallenMag = iDisplacement;
			// Constraint the plaxer's position within the screen boundarx
			Constraint(CPhysics2D::DIRECTION::DOWN);


			// Iterate through all rows until the proposed row
			// Check if the plaxer will hit a tile; stop fall if so.
			int iIndex_XAxis_Proposed = i32vec2Index.x;

			if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
			{
				//if I = 22, I < 24, i++ (WIll fall backwards, towards UP GRAVITX
				for (int i = iIndex_XAxis_OLD; i <= iIndex_XAxis_Proposed; i++)
				{
					// Change the plaxer's index to the current i value
					i32vec2Index.x = i;
		
					

					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						// Revert to the previous position
						if (i != iIndex_XAxis_OLD)
							i32vec2Index.x = i - 1;
						// Set the Phxsics to idle status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

						dJumpCount = 0;
						break;
					}
				}
			}
			else {
				for (int i = iIndex_XAxis_OLD; i >= iIndex_XAxis_Proposed; i--)
				{
					// Change the plaxer's index to the current i value
					i32vec2Index.x = i;
					// If the new position is not feasible, then revert to old position
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						// Revert to the previous position
						if (i != iIndex_XAxis_OLD)
							i32vec2Index.x = i + 1;
						// Set the Phxsics to idle status
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

						dJumpCount = 0;
						break;
					}
				}
			}
		}


		if (PlayerIsOnBottomRow())
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
		}
		if(oldStatus == CPhysics2D::STATUS::FALL 
			&& cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE
			&& abs(fallenMag) > 0)
			cSoundController->PlaySoundByID(SOUND_TYPE::LANDED_GRASS);
	
	}
}

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CPlayer2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x))
	{
	case CMap2D::TILE_ID::BOMB_SMALL:
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		cInventoryItem = cInventoryManager->GetItem("Bomb");
		cInventoryItem->Add(1);
		cSoundController->PlaySoundByID(SOUND_TYPE::ITEM_PICKUP);
		break;
	case CMap2D::TILE_ID::POWERUP_DOUBLEJUMP:
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		cInventoryItem = cInventoryManager->GetItem("DoubleJump");
		cSoundController->PlaySoundByID(SOUND_TYPE::ITEM_PICKUP);
		cInventoryItem->Add(100);
		break;
	case CMap2D::TILE_ID::ACID_DOWN:
	case CMap2D::TILE_ID::ACID_UP:
	case CMap2D::TILE_ID::ACID_LEFT:
	case CMap2D::TILE_ID::ACID_RIGHT:
		// Decrease the health by 1
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(1);

		if (cInventoryItem->GetCount() <= 0)
		{
			//Level Complete
			CGameManager::GetInstance()->bLevelCompleted = true;
		}

		break;
	//case 2:
	//	// Erase the tree from this position
	//	cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
	//	// Increase the Tree by 1
	//	cInventoryItem = cInventoryManager->GetItem("Tree");
	//	cInventoryItem->Add(1);
	//	// Play a bell sound
	//	cSoundController->PlaySoundByID(1);
	//	break;
	//case 10:
	//	// Increase the lives by 1
	//	cInventoryItem = cInventoryManager->GetItem("Lives");
	//	cInventoryItem->Add(1);
	//	// Erase the life from this position
	//	cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
	//	break;
	//case 20:
	//	// Decrease the health by 1
	//	cInventoryItem = cInventoryManager->GetItem("Health");
	//	cInventoryItem->Remove(1);
	//	break;
	//case 21:
	//	// Increase the health
	//	cInventoryItem = cInventoryManager->GetItem("Health");
	//	cInventoryItem->Add(1);
	//	break;
	case 99:
		// Level has been completed
		CGameManager::GetInstance()->bLevelCompleted = true;
		break;
	default:
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CPlayer2D::UpdateHealthLives(void)
{
	// Update health and lives
	cInventoryItem = cInventoryManager->GetItem("Health");
	// Check if a life is lost
	if (cInventoryItem->GetCount() <= 0)
	{
		// Reset the Health to max value
		cInventoryItem->iItemCount = cInventoryItem->GetMaxCount();
		// But we reduce the lives by 1.
		cInventoryItem = cInventoryManager->GetItem("Lives");
		cInventoryItem->Remove(1);
		// Check if there is no lives left...
		if (cInventoryItem->GetCount() < 0)
		{
			// Player loses the game
			CGameManager::GetInstance()->bPlayerLost = true;
		}
	}
}
