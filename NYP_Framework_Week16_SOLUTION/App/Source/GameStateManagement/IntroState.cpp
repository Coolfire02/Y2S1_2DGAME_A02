// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "IntroState.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

#include "System\filesystem.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
CIntroState::CIntroState(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
CIntroState::~CIntroState(void)
{
}

/**
 @brief Init this class instance
 */
bool CIntroState::Init(void)
{
	cout << "CIntroState::Init()\n" << endl;

	// Include Shader Manager
	CShaderManager::GetInstance()->Use("2DShader");
	CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	// Load the sounds into CSoundController
	soundController = CSoundController::GetInstance();
	soundController->Init();
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_BombExplosion.wav"), SOUND_TYPE::BOMB_EXPLOSION, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_DirtLand.wav"), SOUND_TYPE::LANDED_GRASS, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_DirtWalk.ogg"), SOUND_TYPE::WALKING_GRASS, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_GameOver.wav"), SOUND_TYPE::GAME_OVER_LOSE, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_GameWin.wav"), SOUND_TYPE::GAME_OVER_WIN, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_ItemPickup.wav"), SOUND_TYPE::ITEM_PICKUP, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_LevelRotation.wav"), SOUND_TYPE::LEVEL_ROTATION, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Warp.wav"), SOUND_TYPE::WARP, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_Selector.wav"), SOUND_TYPE::SELECTOR, true);
	//Loopables
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_GameBG1.wav"), SOUND_TYPE::BG_ARCADE, true, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_GameBG2.wav"), SOUND_TYPE::BG_ARCADE2, true, true);
	soundController->LoadSound(FileSystem::getPath("Sounds\\Sound_MainMenu.wav"), SOUND_TYPE::BG_MAINMENU, true, true);

	CSoundController::GetInstance()->PlaySoundByID(SOUND_TYPE::BG_MAINMENU);

	//Create Background Entity
	background = new CBackgroundEntity("Image/IntroBackground.png");
	background->SetShader("2DShader");
	background->Init();

	return true;
}

/**
 @brief Update this class instance
 */
bool CIntroState::Update(const double dElapsedTime)
{
	cout << "CIntroState::Update()\n" << endl;
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// End BG Main Menu Music
		soundController->StopPlayingSoundByID(SOUND_TYPE::BG_MAINMENU, 3.0, 0.0);

		// Load the menu state
		cout << "Loading MenuState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("MenuState");
		return true;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void CIntroState::Render()
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Draw the background
 	background->Render();
}

/**
 @brief Destroy this class instance
 */
void CIntroState::Destroy(void)
{
	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	cout << "CIntroState::Destroy()\n" << endl;
}