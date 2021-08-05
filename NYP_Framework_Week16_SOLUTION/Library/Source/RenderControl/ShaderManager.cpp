#include "ShaderManager.h"
#include <stdexcept>      // std::invalid_argument

/**
@brief Constructor
*/
CShaderManager::CShaderManager(void) 
	: activeShader(nullptr)
{
}

/**
@brief Destructor
*/
CShaderManager::~CShaderManager(void)
{
	// Clear the memory
	Exit();
}

/**
@brief Update the active Scene
*/
/*
void CShaderManager::Update(const double dElapsedTime)
{
	// Check for change of scene
	if (nextScene != activeShader)
	{
		if (activeShader)
		{
			// Scene is valid, need to call appropriate function to exit
			activeShader->Release();
		}
		
		activeShader = nextScene;
		activeShader->Init();
	}

	if (activeShader)
		activeShader->Update(dElapsedTime);
}
*/

/**
@brief PreRender the active Scene
*/
/*
void CShaderManager::PreRender(void)
{
	if (activeShader)
		activeShader->PreRender();
}
*/

/**
@brief Render the active Scene
*/
/*
void CShaderManager::Render(void)
{
	if (activeShader)
		activeShader->Render();
}
*/

/**
@brief PostRender the active Scene
*/
/*
void CShaderManager::PostRender(void)
{
	if (activeShader)
		activeShader->PostRender();
}
*/

/**
@brief Exit by deleting the shader
*/
void CShaderManager::Exit(void)
{
	// Delete all scenes stored and empty the entire map
	//activeShader->Release();
	//activeShader = nullptr;
	std::map<std::string, CShader*>::iterator it, end;
	end = shaderMap.end();
	for (it = shaderMap.begin(); it != end; ++it)
	{
		delete it->second;
		it->second = nullptr;
	}
	shaderMap.clear();
}

/**
@brief Add a Scene to this Shader Manager
*/
void CShaderManager::Add(	const std::string& _name, 
							const char* vertexPath, 
							const char* fragmentPath, 
							const char* geometryPath)
{
	if (Check(_name))
	{
		// Scene Exist, unable to proceed
		//throw std::exception("Duplicate shader name provided");
		//Change to avoid crashing
		return;
	}

	CShader* cNewShader = new CShader(vertexPath, fragmentPath);

	// Nothing wrong, add the scene to our map
	shaderMap[_name] = cNewShader;
}

/**
@brief Remove a Shader from this Shader Manager
*/
void CShaderManager::Remove(const std::string& _name)
{
	// Does nothing if it does not exist
	if (!Check(_name))
		return;

	CShader* target = shaderMap[_name];
	if (target == activeShader)
	{
		throw std::exception("Unable to remove active Shader");
	}

	// Delete and remove from our map
	delete target;
	shaderMap.erase(_name);
}

/**
@brief Use a Shader
*/
void CShaderManager::Use(const std::string& _name)
{
	if (!Check(_name))
	{
		// Scene does not exist
		throw std::exception("Shader does not exist");
	}

	// if Shader exist, set the activeShader pointer to that Shader
	activeShader = shaderMap[_name];
	activeShader->use();
}

/**
@brief Check if a Scene exists in this Shader Manager
*/
bool CShaderManager::Check(const std::string& _name)
{
	return shaderMap.count(_name) != 0;
}