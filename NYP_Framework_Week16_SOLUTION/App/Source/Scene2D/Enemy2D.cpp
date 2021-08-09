/**
 CEnemy2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Enemy2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

#include "System/MyMath.h"


// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include math.h
#include <math.h>

// Include Game Manager
#include "GameManager.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::CEnemy2D(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, cSettings(NULL)
	, cPlayer2D(NULL)
	, sCurrentFSM(FSM::IDLE)
	, iFSMCounter(0)
	, quadMesh(NULL)
	, cSoundController(NULL)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	i32vec2Destination = glm::i32vec2(0, 0);	// Initialise the iDestination

	name = "Enemy";
}

void CEnemy2D::CollidedWith(CEntity2D* entity)
{
	if (entity->name == "Bomb")
	{
		CInventoryItem* eHealth = cInventoryManager->GetItem("EnemyHealth");
		eHealth->Remove(50);
		cSoundController->PlaySoundByID(SOUND_TYPE::BOMB_EXPLOSION);
		std::cout << "Ehealth:" << eHealth->GetCount() << std::endl;
		entity->dead = true;
		if (eHealth->GetCount() <= 0)
		{
			this->dead = true;
			cSoundController->PlaySoundByID(SOUND_TYPE::GAME_OVER_WIN);
			CGameManager::GetInstance()->bLevelCompleted = true;
		}
		else if (eHealth->GetCount() <= 50)
		{
			CSoundController::GetInstance()->StopPlayingSoundByID(SOUND_TYPE::BG_ARCADE, 2, 0);
			CSoundController::GetInstance()->PlaySoundByID(SOUND_TYPE::BG_ARCADE2, 2, 1);
		}
	}
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::~CEnemy2D(void)
{
	// Delete the quadMesh
	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	// We won't delete this since it was created elsewhere
	cPlayer2D = NULL;

	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CEnemy2D::Init(ENEMY_TYPE type)
{
	this->type = type;
	this->enemySpeed = 3.f;
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	cInventoryManager = CInventoryManager::GetInstance();
	cKeyboardController = CKeyboardController::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(4, uiRow, uiCol) == false)
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
	switch (type)
	{
	case ENEMY_GOLEM:
		if (LoadTexture("Image/scene2d_golemenemy.png", iTextureID) == false)
		{
			std::cout << "Failed to load golem tile texture" << std::endl;
			return false;
		}
		//CS: Create the animated sprite and setup the animation 
		animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 40, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
		animatedSprites->AddAnimation("idle", 0, 11);
		animatedSprites->AddAnimation("right", 12, 40);
		//CS: Play the "idle" animation as default
		animatedSprites->PlayAnimation("idle", -1, enemySpeed);
		break;
	default:
		std::cout << "Failed to load enemy tile texture (None found)" << std::endl;
		return false;
	}

	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	return true;
}

/**
 @brief Update this instance
 */
void CEnemy2D::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

	switch (sCurrentFSM)
	{
	case IDLE: 
		if (iFSMCounter > 120)
		{
			sCurrentFSM = BOMB_SEARCH;
			iFSMCounter = 0;
			cout << "Switching to Bomb Search" << endl;
		}
		iFSMCounter++;
		break;
	case BOMB_SEARCH:
	{
		unsigned int x, y;
		x = y = -1;

		if (cMap2D->FindValue(CMap2D::TILE_ID::BOMB_SMALL, x, y))
		{
			glm::i32vec2 pos = glm::i32vec2(x, y);
			auto path = cMap2D->PathFind(i32vec2Index,
				pos,
				heuristic::manhattan,
				10);
			//cout << "=== Printing out the path ===" << endl;

			// Calculate new destination
			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				//std::cout << coord.x << "," << coord.y << "\n";
				if (bFirstPosition == true)
				{
					// Set a destination
					i32vec2Index = coord;
					// Calculate the direction between enemy2D and this destination
					i32vec2Direction = i32vec2Destination - i32vec2Index;
					bFirstPosition = false;
				}
				else
				{
					if ((coord - i32vec2Destination) == i32vec2Direction)
					{
						// Set a destination
						i32vec2Index = coord;
					}
					else
						break;
				}
			}


			//system("pause");

			// Attack
			// Update direction to move towards for attack
			//UpdateDirection();

			// Update the Enemy2D's position for attack
			//UpdatePosition();
		}
		break;

	}
	case ATTACK:
		if (CPhysics2D::CalculateDistance(i32vec2Index, cPlayer2D->i32vec2Index) < 25.0f)
		{
			cout << "Attacker Mode" << endl;
			// Calculate a path to the player
			//cMap2D->PrintSelf();
			//cout << "StartPos: " << i32vec2Index.x << ", " << i32vec2Index.y << endl;
			//cout << "TargetPos: " << cPlayer2D->i32vec2Index.x << ", " 
			//		<< cPlayer2D->i32vec2Index.y << endl;
			auto path = cMap2D->PathFind(	i32vec2Index, 
											cPlayer2D->i32vec2Index, 
											heuristic::manhattan,
											10);
			//cout << "=== Printing out the path ===" << endl;

			// Calculate new destination
			bool bFirstPosition = true;
			for (const auto& coord : path)
			{
				//std::cout << coord.x << "," << coord.y << "\n";
				if (bFirstPosition == true)
				{
					// Set a destination
					i32vec2Destination = coord;
					// Calculate the direction between enemy2D and this destination
					i32vec2Direction = i32vec2Destination - i32vec2Index;
					bFirstPosition = false;
				}
				else
				{
					if ((coord - i32vec2Destination) == i32vec2Direction)
					{
						// Set a destination
						i32vec2Destination = coord;
					}
					else
						break;
				}
			}


			//system("pause");

			// Attack
			// Update direction to move towards for attack
			//UpdateDirection();

			// Update the Enemy2D's position for attack
			UpdatePosition();
		}

		if (iFSMCounter > iMaxFSMCounter)
		{
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
			cout << "ATTACK : Reset counter: " << iFSMCounter << endl;
		}
		iFSMCounter++;
		
		break;
	default:
		break;
	}


	if (cKeyboardController->IsKeyDown(GLFW_KEY_G))
	{
		CInventoryItem* eHealth = cInventoryManager->GetItem("EnemyHealth");
		eHealth->Remove(50);
		cSoundController->PlaySoundByID(SOUND_TYPE::BOMB_EXPLOSION);
		std::cout << "Ehealth:" << eHealth->GetCount() << std::endl;
		if (eHealth->GetCount() <= 0)
		{
			this->dead = true;
			cSoundController->PlaySoundByID(SOUND_TYPE::GAME_OVER_WIN);
			CGameManager::GetInstance()->bLevelCompleted = true;
		}
		else if (eHealth->GetCount() <= 50)
		{
			CSoundController::GetInstance()->StopPlayingSoundByID(SOUND_TYPE::BG_ARCADE, 2, 0);
			CSoundController::GetInstance()->PlaySoundByID(SOUND_TYPE::BG_ARCADE2, 2, 1);
		}
	}


	// Update Jump or Fall
	UpdateJumpFall(dElapsedTime);
	
	// Update the Health and Lives
	UpdateHealthLives();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, i32vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, i32vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEnemy2D::PreRender(void)
{
	if (!bIsActive)
		return;

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
void CEnemy2D::Render(void)
{
	if (!bIsActive)
		return;

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

	// Render the tile
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	//CS: Render the animated sprite
	animatedSprites->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEnemy2D::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Set the indices of the enemy2D
@param iIndex_XAxis A const int variable which stores the index in the x-axis
@param iIndex_YAxis A const int variable which stores the index in the y-axis
*/
void CEnemy2D::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->i32vec2Index.x = iIndex_XAxis;
	this->i32vec2Index.y = iIndex_YAxis;
}

/**
@brief Set the number of microsteps of the enemy2D
@param iNumMicroSteps_XAxis A const int variable storing the current microsteps in the X-axis
@param iNumMicroSteps_YAxis A const int variable storing the current microsteps in the Y-axis
*/
void CEnemy2D::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis)
{
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
}

/**
 @brief Set the handle to cPlayer to this class instance
 @param cPlayer2D A CPlayer2D* variable which contains the pointer to the CPlayer2D instance
 */
void CEnemy2D::SetPlayer2D(CPlayer2D* cPlayer2D)
{
	this->cPlayer2D = cPlayer2D;

	// Update the enemy's direction
	UpdateDirection();
}

void CEnemy2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x))
	{
	case 2:
		// Erase the tree from this position
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		// Increase the Tree by 1

		cSoundController->PlaySoundByID(1);
		break;
	}
}

/**
 @brief Update the health and lives.
 */
void CEnemy2D::UpdateHealthLives(void)
{

}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CEnemy2D::LoadTexture(const char* filename, GLuint& iTextureID)
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
	unsigned char* data = cImageLoader->Load(filename, width, height, nrChannels, true);
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

/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CEnemy2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (i32vec2Index.x < 0)
		{
			i32vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (i32vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (i32vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (i32vec2Index.y < 0)
		{
			i32vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		cout << "CEnemy2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CEnemy2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}

	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (i32vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y + 1, i32vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CEnemy2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

// Check if the enemy2D is in mid-air
bool CEnemy2D::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (IsOnBottomRow()) return false;


	// Check if the tile below the player's current position is empty
	glm::vec2 relativeDir = CPhysics2D::GetRelativeDirVector(CPhysics2D::DIRECTION::DOWN);

	if ((i32vec2NumMicroSteps.x == 0 || i32vec2NumMicroSteps.y == 0) &&
		(cMap2D->GetMapInfo(i32vec2Index.y + relativeDir.y, i32vec2Index.x + relativeDir.x) <= CMap2D::TILE_ID::INTERACTABLES_END))
	{
		return true;
	}

	return false;
}

bool CEnemy2D::IsOnBottomRow() {
	switch (CPhysics2D::GetGravityDirection())
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

bool CEnemy2D::IsOnTopRow() {
	switch (CPhysics2D::GetGravityDirection())
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

// Update Jump or Fall
void CEnemy2D::UpdateJumpFall(const double dElapsedTime)
{
	//CPhysics2D::STATUS oldStatus = cPhysics2D.GetStatus();
	//float fallenMag = 0.0;
	//if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	//{
	//	// Update the elapsed time to the physics engine
	//	cPhysics2D.AddElapsedTime((float)dElapsedTime);
	//	// Call the physics engine update method to calculate the final velocity and displacement
	//	cPhysics2D.Update();

	//	// Get the displacement from the physics engine
	//	glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();
	//	std::cout << v2Displacement.x << " " << v2Displacement.y << std::endl;

	//	if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_DOWN)
	//	{
	//		// Store the current i32vec2Index.y
	//		int iIndex_YAxis_OLD = i32vec2Index.y;

	//		// Translate the displacement from pixels to indices
	//		int iDisplacement = (int)(v2Displacement.y / cSettings->TILE_HEIGHT);
	//		int iDisplacement_MicroSteps = (int)((v2Displacement.y * cSettings->iWindowHeight) - iDisplacement) /
	//			(int)cSettings->NUM_STEPS_PER_TILE_YAXIS;
	//		if (iDisplacement_MicroSteps != 0)
	//		{
	//			iDisplacement++;
	//		}

	//		iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).y;
	//		fallenMag = iDisplacement;
	//		// Update the indices
	//		i32vec2Index.y += iDisplacement;
	//		i32vec2NumMicroSteps.y = 0;

	//		// Constraint the player's position within the screen boundary
	//		Constraint(CPhysics2D::DIRECTION::UP);

	//		// Iterate through all rows until the proposed row
	//		// Check if the player will hit a tile; stop jump if so.
	//		int iIndex_YAxis_Proposed = i32vec2Index.y;

	//		if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP)
	//		{
	//			for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
	//			{
	//				// Change the player's index to the current i value
	//				i32vec2Index.y = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
	//				{
	//					i32vec2Index.y = i + 1;
	//					// Set the Physics to fall status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	//					break;
	//				}
	//			}
	//		}
	//		else {
	//			for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
	//			{
	//				// Change the player's index to the current i value
	//				i32vec2Index.y = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
	//				{
	//					i32vec2Index.y = i - 1;
	//					// Set the Physics to fall status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	//					break;
	//				}
	//			}
	//		}
	//	}
	//	else if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_LEFT || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
	//	{
	//		// Store the current i32vec2Index.x
	//		int iIndex_YAxis_OLD = i32vec2Index.x;

	//		// Translate the displacement from pixels to indices
	//		int iDisplacement = (int)(v2Displacement.x / cSettings->TILE_HEIGHT);
	//		int iDisplacement_MicroSteps = (int)((v2Displacement.x * cSettings->iWindowHeight) - iDisplacement) /
	//			(int)cSettings->NUM_STEPS_PER_TILE_YAXIS;
	//		if (iDisplacement_MicroSteps != 0)
	//		{
	//			iDisplacement++;
	//		}

	//		iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).x;

	//		// Update the indices
	//		i32vec2Index.x += iDisplacement;
	//		i32vec2NumMicroSteps.x = 0;
	//		fallenMag = iDisplacement;
	//		// Constraint the plaxer's position within the screen boundarx
	//		Constraint(CPhysics2D::DIRECTION::UP);

	//		// Iterate through all rows until the proposed row
	//		// Check if the plaxer will hit a tile; stop jump if so.
	//		int iIndex_YAxis_Proposed = i32vec2Index.x;

	//		if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
	//		{
	//			for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
	//			{
	//				// Change the plaxer's index to the current i value
	//				i32vec2Index.x = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
	//				{
	//					i32vec2Index.x = i + 1;
	//					// Set the Phxsics to fall status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	//					break;
	//				}
	//			}
	//		}
	//		else {
	//			for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
	//			{
	//				// Change the plaxer's index to the current i value
	//				i32vec2Index.x = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::UP) == false)
	//				{
	//					i32vec2Index.x = i - 1;
	//					// Set the Phxsics to fall status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	//					break;
	//				}
	//			}
	//		}
	//	}

	//	// If the player is still jumping and the initial velocity has reached zero or below zero, 
	//	// then it has reach the peak of its jump
	//	if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.ReachedPeakOfJump()))
	//	{
	//		// Set status to fall
	//		cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
	//	}
	//}
	//else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	//{
	//	// Update the elapsed time to the physics engine
	//	cPhysics2D.AddElapsedTime((float)dElapsedTime);
	//	// Call the physics engine update method to calculate the final velocity and displacement
	//	cPhysics2D.Update();
	//	// Get the displacement from the physics engine
	//	glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();


	//	if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_DOWN)
	//	{
	//		// Store the current i32vec2Index.y
	//		int iIndex_YAxis_OLD = i32vec2Index.y;

	//		// Translate the displacement from pixels to indices
	//		int iDisplacement = (int)(v2Displacement.y / cSettings->TILE_HEIGHT);
	//		int iDisplacement_MicroSteps = (int)((v2Displacement.y * cSettings->iWindowHeight) - iDisplacement) /
	//			(int)cSettings->NUM_STEPS_PER_TILE_YAXIS;
	//		if (iDisplacement_MicroSteps > 0)
	//		{
	//			iDisplacement++;
	//		}

	//		// Update the indices
	//		i32vec2Index.y += iDisplacement;
	//		i32vec2NumMicroSteps.y = 0;

	//		iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).y;
	//		fallenMag = iDisplacement;
	//		// Constraint the player's position within the screen boundary
	//		Constraint(CPhysics2D::DIRECTION::DOWN);


	//		// Iterate through all rows until the proposed row
	//		// Check if the player will hit a tile; stop fall if so.
	//		int iIndex_YAxis_Proposed = i32vec2Index.y;

	//		if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_UP)
	//		{
	//			//if I = 22, I < 24, i++ (WIll fall backwards, towards UP GRAVITY
	//			for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
	//			{
	//				// Change the player's index to the current i value
	//				i32vec2Index.y = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
	//				{
	//					// Revert to the previous position
	//					if (i != iIndex_YAxis_OLD)
	//						i32vec2Index.y = i - 1;
	//					// Set the Physics to idle status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

	//
	//					break;
	//				}
	//			}
	//		}
	//		else {
	//			for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
	//			{
	//				// Change the player's index to the current i value
	//				i32vec2Index.y = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
	//				{
	//					// Revert to the previous position
	//					if (i != iIndex_YAxis_OLD)
	//						i32vec2Index.y = i + 1;
	//					// Set the Physics to idle status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

	//
	//					break;
	//				}
	//			}
	//		}
	//	}
	//	else if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_LEFT || cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
	//	{
	//		// Store the current i32vec2Index.x
	//		int iIndex_XAxis_OLD = i32vec2Index.x;

	//		// Translate the displacement from pixels to indices
	//		int iDisplacement = (int)(v2Displacement.x / cSettings->TILE_HEIGHT);
	//		int iDisplacement_MicroSteps = (int)((v2Displacement.x * cSettings->iWindowHeight) - iDisplacement) /
	//			(int)cSettings->NUM_STEPS_PER_TILE_XAXIS;
	//		if (iDisplacement_MicroSteps > 0)
	//		{
	//			iDisplacement++;
	//		}

	//		// Update the indices
	//		i32vec2Index.x += iDisplacement;
	//		i32vec2NumMicroSteps.x = 0;

	//		iDisplacement *= cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::UP).x;
	//		fallenMag = iDisplacement;
	//		// Constraint the plaxer's position within the screen boundarx
	//		Constraint(CPhysics2D::DIRECTION::DOWN);


	//		// Iterate through all rows until the proposed row
	//		// Check if the plaxer will hit a tile; stop fall if so.
	//		int iIndex_XAxis_Proposed = i32vec2Index.x;

	//		if (cPhysics2D.GetGravityDirection() == CPhysics2D::GRAVITY_RIGHT)
	//		{
	//			//if I = 22, I < 24, i++ (WIll fall backwards, towards UP GRAVITX
	//			for (int i = iIndex_XAxis_OLD; i <= iIndex_XAxis_Proposed; i++)
	//			{
	//				// Change the plaxer's index to the current i value
	//				i32vec2Index.x = i;



	//				if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
	//				{
	//					// Revert to the previous position
	//					if (i != iIndex_XAxis_OLD)
	//						i32vec2Index.x = i - 1;
	//					// Set the Phxsics to idle status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);

	//					break;
	//				}
	//			}
	//		}
	//		else {
	//			for (int i = iIndex_XAxis_OLD; i >= iIndex_XAxis_Proposed; i--)
	//			{
	//				// Change the plaxer's index to the current i value
	//				i32vec2Index.x = i;
	//				// If the new position is not feasible, then revert to old position
	//				if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
	//				{
	//					// Revert to the previous position
	//					if (i != iIndex_XAxis_OLD)
	//						i32vec2Index.x = i + 1;
	//					// Set the Phxsics to idle status
	//					cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);


	//					break;
	//				}
	//			}
	//		}
	//	}


	//	if (IsOnBottomRow())
	//	{
	//		cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
	//	}
	//	if (oldStatus == CPhysics2D::STATUS::FALL
	//		&& cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE
	//		&& abs(fallenMag) > 0)
	//		cSoundController->PlaySoundByID(SOUND_TYPE::LANDED_GRASS);

	//}
}

/**
 @brief Let enemy2D interact with the player.
 */
bool CEnemy2D::InteractWithPlayer(void)
{
	glm::i32vec2 i32vec2PlayerPos = cPlayer2D->i32vec2Index;

	if (iFSMCounter > 120)
	{
		sCurrentFSM = IDLE;
		iFSMCounter = 0;
	}
	
	// Check if the enemy2D is within 1.5 indices of the player2D
	if (((i32vec2Index.x >= i32vec2PlayerPos.x - 0.5) && 
		(i32vec2Index.x <= i32vec2PlayerPos.x + 0.5))
		&& 
		((i32vec2Index.y >= i32vec2PlayerPos.y - 0.5) &&
		(i32vec2Index.y <= i32vec2PlayerPos.y + 0.5)))
	{
		
		cInventoryItem = cInventoryManager->GetItem("Health");
		cInventoryItem->Remove(100);


		// Since the player has been caught, then reset the FSM
		sCurrentFSM = IDLE;
		iFSMCounter = 0;
		return true;
	}
	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CEnemy2D::UpdateDirection(void)
{
	 //Set the destination to the player
	//switch (sCurrentFSM)
	//{
	//case ATTACK:
	//	i32vec2Destination = cPlayer2D->i32vec2Index;
	//	break;
	//case BOMB_SEARCH:
	//	unsigned int x, y;
	//	x = y = -1;

	//	if (cMap2D->FindValue(CMap2D::TILE_ID::BOMB_SMALL, x, y))
	//	{
	//		i32vec2Destination = glm::i32vec2(y,x);
	//	}
	//	break;
	//}

	// Calculate the direction between enemy2D and player2D
	//glm::i32vec2 i32vec2Direction;
	//i32vec2Direction = i32vec2Destination - i32vec2Index;

	//// Calculate the distance between enemy2D and player2D
	//float fDistance = CPhysics2D::CalculateDistance(i32vec2Index, i32vec2Destination);
	//if (fDistance >= 0.01f)
	//{
	//	// Calculate direction vector.
	//	// We need to round the numbers as it is easier to work with whole numbers for movements
	//	if (i32vec2Direction.x > 0) i32vec2Direction.x = 1;
	//	else if (i32vec2Direction.x < 0) i32vec2Direction.x = -1;

	//	if (i32vec2Direction.y > 0) i32vec2Direction.y = 1;
	//	else if (i32vec2Direction.y < 0) i32vec2Direction.y = -1;

	//	glm::vec2 dir1 = glm::vec2(i32vec2Direction.x, 0);
	//	glm::vec2 dir2 = glm::vec2(0, i32vec2Direction.y);
	//	relativeDirections[0] = CPhysics2D::DIRECTION::NUM_DIRECTIONS;
	//	relativeDirections[1] = CPhysics2D::DIRECTION::NUM_DIRECTIONS;
	//	for (int i = CPhysics2D::DIRECTION::UP; i < CPhysics2D::DIRECTION::UP + 4; ++i)
	//	{
	//		if (CPhysics2D::GetRelativeDirVector(static_cast<CPhysics2D::DIRECTION>(i)) == dir1)
	//		{
	//			relativeDirections[0] = static_cast<CPhysics2D::DIRECTION>(i);
	//		}
	//	}
	//	for (int i = CPhysics2D::DIRECTION::UP; i < CPhysics2D::DIRECTION::UP + 4; ++i)
	//	{
	//		if (CPhysics2D::GetRelativeDirVector(static_cast<CPhysics2D::DIRECTION>(i)) == dir2)
	//		{
	//			relativeDirections[1] = static_cast<CPhysics2D::DIRECTION>(i);
	//		}
	//	}
	//}
	//else
	//{
	//	// Since we are not going anywhere, set this to 0.
	//	i32vec2Direction = glm::i32vec2(0);
	//}
}

/**
 @brief Flip horizontal direction. For patrol use only
 */
void CEnemy2D::FlipHorizontalDirection(void)
{
	/*for (int i = 0; i < 2; ++i)
	{
		if (relativeDirections[i] == CPhysics2D::DIRECTION::LEFT)
		{
			relativeDirections[i] = CPhysics2D::DIRECTION::RIGHT;
		}
		else if (relativeDirections[i] == CPhysics2D::DIRECTION::RIGHT)
		{
			relativeDirections[i] = CPhysics2D::DIRECTION::LEFT;
		}
	}*/
}

/**
@brief Update position.
*/
void CEnemy2D::UpdatePosition(void)
{
	// Store the old position
	i32vec2OldIndex = i32vec2Index;
	
	cout << "FUCKING DIRECTION x: " << i32vec2Destination.x << "\n            y: " << i32vec2Destination.y << endl;

	// if the player is to the left or right of the enemy2D, then jump to attack
	if (i32vec2Direction.x < 0)
	{
		// Move left
		const int iOldIndex = i32vec2Index.x;
		if (i32vec2Index.x >= 0)
		{
			i32vec2NumMicroSteps.x--;
			if (i32vec2NumMicroSteps.x < 0)
			{
				i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				i32vec2Index.x--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(LEFT);

		// Find a feasible position for the enemy2D's current position
		//if (CheckPosition(LEFT) == false)
		//{
		//	FlipHorizontalDirection();
		//	i32vec2Index = i32vec2OldIndex;
		//	i32vec2NumMicroSteps.x = 0;
		//}

		// Interact with the Player
		InteractWithPlayer();
	}
	else if (i32vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = i32vec2Index.x;
		if (i32vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x++;

			if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				i32vec2NumMicroSteps.x = 0;
				i32vec2Index.x++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(RIGHT);

		// Find a feasible position for the enemy2D's current position
		//if (CheckPosition(RIGHT) == false)
		//{
		//	i32vec2Index = i32vec2OldIndex;
		//	i32vec2NumMicroSteps.x = 0;
		//}



		// Interact with the Player
		InteractWithPlayer();
	}

	if (i32vec2Direction.y < 0)
	{
		// Move left
		const int iOldIndex = i32vec2Index.y;
		if (i32vec2Index.y >= 0)
		{
			i32vec2NumMicroSteps.y--;
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				i32vec2Index.y--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(DOWN);

		// Find a feasible position for the enemy2D's current position
		//if (CheckPosition(DOWN) == false)
		//{
		//	i32vec2Index = i32vec2OldIndex;
		//	i32vec2NumMicroSteps.y = 0;
		//}

		// Interact with the Player
		InteractWithPlayer();
	}

	else if (i32vec2Direction.y > 0)
	{
		// Move right
		const int iOldIndex = i32vec2Index.y;
		if (i32vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			i32vec2NumMicroSteps.y++;

			if (i32vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				i32vec2NumMicroSteps.y = 0;
				i32vec2Index.y++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(UP);

		// Find a feasible position for the enemy2D's current position
		//if (CheckPosition(UP) == false)
		//{
		//	i32vec2Index = i32vec2OldIndex;
		//	i32vec2NumMicroSteps.y = 0;
		//}



		// Interact with the Player
		InteractWithPlayer();
	}


}
