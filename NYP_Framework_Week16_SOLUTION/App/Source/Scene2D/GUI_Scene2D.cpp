/**
 CGUI_Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: cSettings(NULL)
	, m_fProgressBar(0.0f)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Show the mouse pointer
	if (cSettings->bDisableMousePointer == true)
		glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		// Hide the cursor
		if (cSettings->bShowMousePointer == false)
			glfwSetInputMode(cSettings->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}

/**
  @brief Initialise this instance
  */
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	//glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Initialise the cInventoryManager
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Tree as one of the inventory items
	cInventoryItem = cInventoryManager->Add("Tree", "Image/Scene2D_TreeTile.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("Bomb", "Image/scene2d_bomb.tga", 5, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("EnemyHealth", "Image/scene2d_golemenemy_icon.png", 100, 100);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	cInventoryItem = cInventoryManager->Add("DoubleJump", "Image/scene2d_jumpboost.png", 100, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);

	return true;
}

/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create an invisible window which covers the entire OpenGL window
	ImGui::Begin("Invisible window", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
	ImGui::SetWindowFontScale(1.5f);

	// Display the FPS
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());

	//// Render a progress bar
	//m_fProgressBar += 0.001f;
	//if (m_fProgressBar > 1.0f)
	//	m_fProgressBar = 0.0f;
	//ImVec4 col = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	//ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
	//col = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
	//ImGui::PushStyleColor(ImGuiCol_FrameBg, col);
	//ImGui::ProgressBar(m_fProgressBar, ImVec2(100.0f, 20.0f));
	//ImGui::PopStyleColor();
	//ImGui::PopStyleColor();

	// Render the Health
	ImGuiWindowFlags healthWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Health", NULL, healthWindowFlags);
	ImGui::SetWindowPos(ImVec2(25.0f, 25.0f));
	ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
	cInventoryItem = cInventoryManager->GetItem("Health");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
			ImGui::ProgressBar(cInventoryItem->GetCount() / 
				(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f, 20.0f));
		ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();

	ImGui::Begin("EnemyHealth", NULL, healthWindowFlags);
	ImGui::SetWindowPos(ImVec2(25.0f, 50.0f));
	ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
	cInventoryItem = cInventoryManager->GetItem("EnemyHealth");
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::ProgressBar(cInventoryItem->GetCount() /
		(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f, 20.0f));
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();

	cInventoryItem = cInventoryManager->GetItem("DoubleJump");
	if (cInventoryItem->GetCount() > 0)
	{
		ImGui::Begin("DoubleJump", NULL, healthWindowFlags);
		ImGui::SetWindowPos(ImVec2(550.0f, 50.0f));
		ImGui::SetWindowSize(ImVec2(100.0f, 25.0f));
		ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
			ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
			ImVec2(0, 1), ImVec2(1, 0));
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
		ImGui::ProgressBar(cInventoryItem->GetCount() /
			(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f, 20.0f));
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::End();
	}


	// Render the inventory items
	cInventoryItem = cInventoryManager->GetItem("Bomb");
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.7f, 0.5f, 1.0f));  // Set a background color
	ImGuiWindowFlags inventoryWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Image", NULL, inventoryWindowFlags);
	ImGui::SetWindowPos(ImVec2(550.0f, 550.0f));
	ImGui::SetWindowSize(ImVec2(200.0f, 25.0f));
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x, cInventoryItem->vec2Size.y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::SetWindowFontScale(1.5f);
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Bombs: %d / %d",
		cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
	ImGui::End();
	ImGui::PopStyleColor();



	//// IMGUI window demo
	//inventoryWindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));  // Set a background color
	//ImGui::Begin("TestImage", NULL, inventoryWindowFlags);
	//	ImGui::SetWindowPos(ImVec2(500.0f, 500.0f));
	//	ImGui::SetWindowSize(ImVec2(200.0f, 100.0f));

	//	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));  // Set a background color
	//	ImGui::BeginChild("TestImage2", ImVec2(175.0f, 75.0f), inventoryWindowFlags);
	//		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));  // Set a background color
	//		ImGui::BeginChild("TestImage3", ImVec2(150.0f, 50.0f), inventoryWindowFlags);
	//		ImGui::EndChild();
	//		ImGui::PopStyleColor();
	//	ImGui::EndChild();
	//	ImGui::PopStyleColor();
	//ImGui::End();
	//ImGui::PopStyleColor();

	ImGui::End();
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}
