/**
 Enemy2D
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Bomb2D.h"

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

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CBomb2D::CBomb2D(void)
	: cMap2D(NULL)
	, cSoundController(NULL)
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	i32vec2Index = glm::i32vec2(0);

	name = "Bomb";

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CBomb2D::~CBomb2D(void)
{
	// We won't delete this since it was created elsewhere
	cSoundController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
}

/**
  @brief Initialise this instance
  */
bool CBomb2D::Init(CPhysics2D::GRAVITY_DIRECTION dir, int x, int y)
{
	cPhysics2D.SetGravityDirection(dir);
	cPhysics2D.setGravityMagnitude(7.5f);
	cPhysics2D.SetInitialVelocity(glm::vec2(0.f, -10.f));
	this->type = type;
	this->enemySpeed = 3.f;
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to CBomb2D

	// Set the start position of the Player to iRow and iCol
	i32vec2Index.x = x;
	i32vec2Index.y = y;
	i32vec2OldIndex = i32vec2Index;
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	

	if (LoadTexture("Image/scene2d_bomb.tga", iTextureID) == false)
	{
		std::cout << "Failed to bomb texture texture" << std::endl;
	}
	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(1, 1, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);
	animatedSprites->AddAnimation("idle", 0, 1);
	animatedSprites->PlayAnimation("idle", -1, enemySpeed);


	


	//CS: Init the color to white
	currentColor = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);

	// Get the handler to the CSoundController
	cSoundController = CSoundController::GetInstance();

	return true;
}



/**
 @brief Update this instance
 */
void CBomb2D::Update(const double dElapsedTime)
{
	
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
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
					InteractWithMap();
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
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
					InteractWithMap();
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
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
					// If the new position is not feasible, then revert to old position
					InteractWithMap();
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
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
					InteractWithMap();
					if (CheckPosition(CPhysics2D::DIRECTION::DOWN) == false)
					{
						cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
						break;
					}
				}
			}
		}
	}


	// Update the Health and Lives
	UpdateHealthLives();

	//CS: Update the animated sprite
	animatedSprites->Update(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, i32vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, i32vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);
}

void CBomb2D::CollidedWith(CEntity2D* entity)
{

}

void CBomb2D::Constraint(CPhysics2D::DIRECTION eDirection)
{
	glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(eDirection);
	if (relativeDir.x == -1)
	{
		if (i32vec2Index.x <= 0)
		{
			i32vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
			dead = true;
		}
	}
	else if (relativeDir.x == 1)
	{
		if (i32vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			i32vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
			dead = true;
		}
	}
	else if (relativeDir.y == 1)
	{
		if (i32vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			i32vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
			dead = true;
		}
	}
	else if (relativeDir.y == -1)
	{
		if (i32vec2Index.y <= 0)
		{
			i32vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
			dead = true;
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
bool CBomb2D::CheckPosition(CPhysics2D::DIRECTION eDirection)
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
		cout << "CBomb2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}


bool CBomb2D::PlayerIsOnBottomRow() {
	switch (cPhysics2D.GetGravityDirection())
	{
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_DOWN:
		if (i32vec2Index.y == 0)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_UP:
		if (i32vec2Index.y == cSettings->NUM_TILES_YAXIS - 1)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_LEFT:
		if (i32vec2Index.x == 0)
			return true;
		break;
	case CPhysics2D::GRAVITY_DIRECTION::GRAVITY_RIGHT:
		if (i32vec2Index.x == cSettings->NUM_TILES_XAXIS - 1)
			return true;
		break;
	}
	return false;
}

bool CBomb2D::PlayerIsOnTopRow() {
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
bool CBomb2D::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (PlayerIsOnBottomRow()) return false;


	// Check if the tile below the player's current position is empty
	glm::vec2 relativeDir = cPhysics2D.GetRelativeDirVector(CPhysics2D::DIRECTION::DOWN);

	if ((i32vec2NumMicroSteps.x == 0 || i32vec2NumMicroSteps.y == 0) &&
		(cMap2D->GetMapInfo(i32vec2Index.y + relativeDir.y, i32vec2Index.x + relativeDir.x) == 0))
	{
		return true;
	}

	return false;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CBomb2D::PreRender(void)
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
void CBomb2D::Render(void)
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
void CBomb2D::PostRender(void)
{
	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
*/
bool CBomb2D::LoadTexture(const char* filename, GLuint& iTextureID)
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

/**
 @brief Let player interact with the map. You can add collectibles such as powerups and health here.
 */
void CBomb2D::InteractWithMap(void)
{
	if (dead) return;

	int id = cMap2D->GetMapInfo(i32vec2Index.y, i32vec2Index.x);

	if (id > CMap2D::TILE_ID::INTERACTABLES_END && id < CMap2D::TILE_ID::BLOCK_END)
	{
		cMap2D->SetMapInfo(i32vec2Index.y, i32vec2Index.x, 0);
		dead = true;
		
		// Explosion sound
		//cSoundController->PlaySoundByID(1);
	}
}

/**
 @brief Update the health and lives.
 */
void CBomb2D::UpdateHealthLives(void)
{
	
}
