/**
CScriptManager
By: Toh Da Jun
Date: Mar 2020
*/
#pragma once

// Include SingletonTemplate
#include "../DesignPatterns/SingletonTemplate.h"

// Lua's headers
#include "lua.hpp"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

class CScriptManager : public CSingletonTemplate<CScriptManager>
{
	friend CSingletonTemplate<CScriptManager>;
public:
	// Constructor
	CScriptManager(void);

	// Destructor
	virtual ~CScriptManager(void);

	// Initialise this class instance
	bool Init(void);
	// Initialise this class instance - overloaded
	bool Init(const string& filename, const std::string& Writefilename, const bool bDisplayFileContent = false);
	// Destroy this class instance
	void Destroy(void);
	// PrintSelf
	void PrintSelf(void);
	// Print error while using Lua functions
	void printError(const string& sKeyAndValue, const string& reason);

	// Get an Vector of Integers
	vector<int> getIntVector(const string& name);
	// Get an Vector of String
	void getStringVector(const std::string& name, std::vector<string>& keys, std::vector<int>& values);
	// Get a table of keys
	vector<string> getTableKeys(const string& name);
	// Get distance square value through the ScriptManager Class
	float getDistanceSquareValue(const char* variableName, glm::vec3 source, glm::vec3 destination);
	// Get variable number of values through the ScriptManager Class
	bool getVariableValues(const char* variableName,
			int& minValue, int& maxValue, int& avgValue, int& numValues,
			const int varCount, ...);

	// Clean the Lua State
	inline void clean(lua_State* pCleanThisState = NULL);

	// Generic get method
	template<typename T>
	T get(const std::string& sKeyAndValue)
	{
		if (!pLuaState) {
			printError(sKeyAndValue, "Script is not loaded");
			return lua_getdefault<T>();
		}

		T result;
		// Check if the variable exists in the Lua file
		if (lua_gettostack(sKeyAndValue))
		{
			// variable succesfully on top of stack
			result = lua_get<T>(sKeyAndValue);
		}
		else 
		{
			result = lua_getdefault<T>();
		}

		// Clear the Lua Stack
		clean(pLuaState);
		return result;
	}

	// Get to Stack
	bool lua_gettostack(const std::string& sKeyAndValue);

	// Generic get which return 0. This is a Template
	template<typename T>
	T lua_get(const std::string& sKeyAndValue) {
		return 0;
	}

	// Generic default get which return 0. This is a Template
	template<typename T>
	T lua_getdefault() {
		return 0;
	}

	// Generic set method
	template<typename T>
	bool set(const std::string& sKeyAndValue, const std::string& sKey, const T value, const int bOverwrite = 0)
	{
		if (!pLuaState) {
			printError(sKeyAndValue, "Script is not loaded");
			return false;
		}

		bool result = false;
		// If overwriting existing variable, then don't check if it exists in the Lua file
		if (bOverwrite == 1)
		{
			// variable succesfully on top of stack
			result = lua_set<T>(sKeyAndValue, sKey, value, bOverwrite);
		}
		else
		{
			// Check if the variable exists in the Lua file
			if (lua_gettostack(sKeyAndValue))
			{
				// variable succesfully on top of stack
				result = lua_set<T>(sKeyAndValue, sKey, value, bOverwrite);
			}
		}

		// Clear the Lua Stack
		clean(pLuaState);
		return result;
	}

	// Generic set. This is a Template
	template<typename T>
	bool lua_set(const std::string& sKeyAndValue, const std::string& sKey, const T value, const int bOverwrite = 0) {
		return false;
	}

	// Generic default set. This is a Template
	template<typename T>
	T lua_setdefault() {
		return false;
	}

protected:
	// Check if the Lua State is valid
	bool isLuaStateValid(lua_State* pCheckThisState = NULL);
	// Stack dump
	void stackDump(lua_State* pStackDumpThisState = NULL);

	// Lua State for Reading In
	lua_State* pLuaState;
	// Lua State for Writing Out
	lua_State* pWriteLuaState;
	std::string filename;
	int level;
};

/********************************************************************************/
// Helper template methods for GET
/********************************************************************************/
/**
@brief Template Get method for bool data type.
*/
template <>
inline bool CScriptManager::lua_get<bool>(const std::string& variableName)
{
	if (lua_toboolean(pLuaState, -1) == 0)
		return false;
	return true;
}

/**
@brief Template Get method for float data type.
*/
template <>
inline float CScriptManager::lua_get<float>(const std::string& variableName)
{
	if (!lua_isnumber(pLuaState, -1)) {
		printError(variableName, "Not a number");
	}
	return (float)lua_tonumber(pLuaState, -1);
}

/**
@brief Template Get method for int data type.
*/
template <>
inline int CScriptManager::lua_get<int>(const std::string& variableName)
{
	if (!lua_isnumber(pLuaState, -1)) {
		printError(variableName, "Not a number");
	}
	return (int)lua_tonumber(pLuaState, -1);
}

/**
@brief Template Get method for string data type.
*/
template <>
inline std::string CScriptManager::lua_get<std::string>(const std::string& variableName)
{
	std::string s = "null";
	if (lua_isstring(pLuaState, -1)) {
		s = std::string(lua_tostring(pLuaState, -1));
	}
	else {
		printError(variableName, "Not a string");
	}
	return s;
}

/**
@brief Template Get method for glm::vec3 data type.
*/
template <>
inline glm::vec3 CScriptManager::lua_get<glm::vec3>(const std::string& variableName)
{
	glm::vec3 aVector(0.0f, 0.0f, 0.0f);
	if (lua_istable(pLuaState, -1)) {
		lua_rawgeti(pLuaState, -1, 1);
		aVector.x = (float)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);
		lua_rawgeti(pLuaState, -1, 2);
		aVector.y = (float)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);
		lua_rawgeti(pLuaState, -1, 3);
		aVector.z = (float)lua_tonumber(pLuaState, -1);
		lua_pop(pLuaState, 1);
	}
	else {
		printError(variableName, "Not a Vector3");
	}
	return aVector;
}

/**
@brief Default get method for std::string. The Generic get method will call this method.
*/
template<>
inline std::string CScriptManager::lua_getdefault<std::string>()
{
	return "null";
}

/**
@brief Default get method for glm::vec3. The Generic get method will call this method.
*/
template<>
inline glm::vec3 CScriptManager::lua_getdefault<glm::vec3>()
{
	glm::vec3 aVector(0.0f);
	return aVector;
}

/********************************************************************************/
// Helper template methods for SET
/********************************************************************************/
/**
@brief Template Set method for bool data type.
*/
template <>
inline bool CScriptManager::lua_set<bool>(const std::string& sKeyAndValue, const std::string& sKey, 
	const bool bValue, const int bOverwrite)
{
	// Prepare the Lua State to point to the SetToLuaFile method
	lua_getglobal(pWriteLuaState, "SetToLuaFile");
	// Prepare the new key and value char array
	char arrNewKeyAndValue[80];
	if (bValue == true)
		sprintf_s(arrNewKeyAndValue, "%s = true", sKeyAndValue.c_str());
	else
		sprintf_s(arrNewKeyAndValue, "%s = false", sKeyAndValue.c_str());

	// Push 2 parameters to the SetToLuaFile method
	lua_pushstring(pWriteLuaState, arrNewKeyAndValue);
	lua_pushinteger(pWriteLuaState, bOverwrite);
	if (bOverwrite == 2)
	{
		// Prepare the old key and value char array
		char arrOldKeyAndValue[80];
		if (get<bool>(sKeyAndValue))
			sprintf_s(arrOldKeyAndValue, "%s = true", sKeyAndValue.c_str());
		else
			sprintf_s(arrOldKeyAndValue, "%s = false", sKeyAndValue.c_str());
		// Push the 3rd parameters to the SetToLuaFile method
		lua_pushfstring(pWriteLuaState, arrOldKeyAndValue);

		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 3, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	else
	{
		if (lua_pcall(pWriteLuaState, 2, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	return true;
}

/**
@brief Template Set method for float data type.
*/
template <>
inline bool CScriptManager::lua_set<float>(const std::string& sKeyAndValue, const std::string& sKey, const float fValue, const int bOverwrite)
{
	// Prepare the Lua State to point to the SetToLuaFile method
	lua_getglobal(pWriteLuaState, "SetToLuaFile");
	// Prepare the new key and value char array
	char arrNewKeyAndValue[80];
	sprintf_s(arrNewKeyAndValue, "%s = %f", sKeyAndValue.c_str(), fValue);

	// Push 2 parameters to the SetToLuaFile method
	lua_pushstring(pWriteLuaState, arrNewKeyAndValue);
	lua_pushinteger(pWriteLuaState, bOverwrite);
	if (bOverwrite == 2)
	{
		// Prepare the old key and value char array
		char arrOldKeyAndValue[80];
		sprintf_s(arrOldKeyAndValue, "%s = %f", sKey.c_str(), get<float>(sKeyAndValue));
		// Push the 3rd parameters to the SetToLuaFile method
		lua_pushfstring(pWriteLuaState, arrOldKeyAndValue);

		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 3, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	else
	{
		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 2, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}

	return true;
}

/**
@brief Template Set method for int data type.
*/
template <>
inline bool CScriptManager::lua_set<int>(const std::string& sKeyAndValue, const std::string& sKey, const int iValue, const int bOverwrite)
{
	// Prepare the Lua State to point to the SetToLuaFile method
	lua_getglobal(pWriteLuaState, "SetToLuaFile");
	// Prepare the new key and value char array
	char arrNewKeyAndValue[80];
	sprintf_s(arrNewKeyAndValue, "%s = %d", sKey.c_str(), iValue);

	// Push 2 parameters to the SetToLuaFile method
	lua_pushstring(pWriteLuaState, arrNewKeyAndValue);
	lua_pushinteger(pWriteLuaState, bOverwrite);
	if (bOverwrite == 2)
	{
		// Prepare the old key and value char array
		char arrOldKeyAndValue[80];
		sprintf_s(arrOldKeyAndValue, "%s = %d", sKey.c_str(), get<int>(sKeyAndValue));
		// Push the 3rd parameters to the SetToLuaFile method
		lua_pushfstring(pWriteLuaState, arrOldKeyAndValue);

		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 3, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	else
	{
		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 2, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}

	return true;
}

/**
@brief Template Set method for string data type.
*/
template <>
inline bool CScriptManager::lua_set<std::string>(const std::string& sKeyAndValue, const std::string& sKey, const std::string sValue, const int bOverwrite)
{
	// Prepare the Lua State to point to the SetToLuaFile method
	lua_getglobal(pWriteLuaState, "SetToLuaFile");
	// Prepare the new key and value char array
	char arrNewKeyAndValue[80];
	sprintf_s(arrNewKeyAndValue, "%s = \"%s\"", sKeyAndValue.c_str(), sValue.c_str());

	// Push 2 parameters to the SetToLuaFile method
	lua_pushstring(pWriteLuaState, arrNewKeyAndValue);
	lua_pushinteger(pWriteLuaState, bOverwrite);
	if (bOverwrite == 2)
	{
		// Prepare the old key and value char array
		char arrOldKeyAndValue[80];
		sprintf_s(arrOldKeyAndValue, "%s = \"%s\"", sKey.c_str(), get<string>(sKeyAndValue).c_str());
		// Push the 3rd parameters to the SetToLuaFile method
		lua_pushfstring(pWriteLuaState, arrOldKeyAndValue);

		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 3, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	else
	{
		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 2, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	return true;
}

/**
@brief Template Set method for glm::vec3 data type.
*/
template <>
inline bool CScriptManager::lua_set<glm::vec3>(const std::string& sKeyAndValue, const std::string& sKey, 
												const glm::vec3 sValue, const int bOverwrite)
{
	// Prepare the Lua State to point to the SetToLuaFile method
	lua_getglobal(pWriteLuaState, "SetToLuaFile");
	// Prepare the new key and value char array
	char arrNewKeyAndValue[80];
	sprintf_s(arrNewKeyAndValue, "%s = {%0.1f, %0.1f, %0.1f}", sKey.c_str(), sValue.x, sValue.y, sValue.z);

	// Push 2 parameters to the SetToLuaFile method
	lua_pushstring(pWriteLuaState, arrNewKeyAndValue);
	lua_pushinteger(pWriteLuaState, bOverwrite);
	if (bOverwrite == 2)
	{
		// Prepare the old key and value char array
		char arrOldKeyAndValue[80];
		glm::vec3 oldVec3 = get<glm::vec3>(sKeyAndValue);
		sprintf_s(arrOldKeyAndValue, "%s = {%0.1f, %0.1f, %0.1f}", sKey.c_str(), oldVec3.x, oldVec3.y, oldVec3.z);
		// Push the 3rd parameters to the SetToLuaFile method
		lua_pushfstring(pWriteLuaState, arrOldKeyAndValue);

		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 3, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	else
	{
		// Execute the SetToLuaFile method
		if (lua_pcall(pWriteLuaState, 2, 0, 0) != 0)
		{
			std::cout << "Unable to save to Lua file : " << lua_tostring(pWriteLuaState, -1) << std::endl;
			return false;
		}
	}
	return true;
}

/**
 @brief Default set method for std::string. The Generic set method will call this method.
 */
template<>
inline std::string CScriptManager::lua_setdefault<std::string>() {
	return "null";
}

/**
@brief Default set method for glm::vec3. The Generic set method will call this method.
*/
template<>
inline glm::vec3 CScriptManager::lua_setdefault<glm::vec3>()
{
	glm::vec3 aVector(0.0f);
	return aVector;
}