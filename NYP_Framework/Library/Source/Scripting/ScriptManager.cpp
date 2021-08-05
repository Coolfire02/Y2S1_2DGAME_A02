#include "ScriptManager.h"
#include <iostream>
#include <fstream>

/**
 @brief Constructor
 */
CScriptManager::CScriptManager(void)
	: pLuaState(NULL)
	, pWriteLuaState(NULL)
{
	Init();
}

/**
 @brief Destructor
 */
CScriptManager::~CScriptManager(void)
{
	Destroy();
}

/**
 @brief Initialise this class instance
 */
bool CScriptManager::Init(void)
{
	// Check if the Lua State is valid.
	// Open the Lua State if not
	if (pLuaState == NULL)
	{
		pLuaState = luaL_newstate();
	}

	// If the Lua State is valid, then return true
	if (pLuaState)
		return true;

	// Check if the Lua State is valid.
	// Open the Lua State if not
	if (pWriteLuaState == NULL)
	{
		pWriteLuaState = luaL_newstate();
	}

	// If the Lua State is valid, then return true
	if (pWriteLuaState)
		return true;

	return true;
}

/**
 @brief Initialise this class instance - overloaded
 */
bool CScriptManager::Init(	const std::string& filename, 
						const std::string& Writefilename, 
						const bool bDisplayFileContent)
{
	// Check if the Lua State is valid.
	// Open the Lua State if not
	if (pLuaState == NULL)
	{
		//pLuaState = lua_open();
		pLuaState = luaL_newstate();
	}
	else
	{
		// Clean the Lua Stack
		clean(pLuaState);
	}

	if (pLuaState)
		luaL_openlibs(pLuaState);
	else
	{
		// Clean the Lua Stack
		clean(pLuaState);
	}

	// Open the Lua file and create the Lua State
	// Return false if unable to open the Lua file
	if (luaL_loadfile(pLuaState, filename.c_str()) || lua_pcall(pLuaState, 0, 0, 0)) {
		cout << "Error: script not loaded (" << filename << ")" << endl;
		pLuaState = NULL;
		return false;
	}

	// Check if the Lua State is valid.
	// Open the Lua State if not
	if (pWriteLuaState == NULL)
	{
		//pWriteLuaState = lua_open();
		pWriteLuaState = luaL_newstate();
	}
	else
	{
		// Clean the Lua Stack
		clean(pWriteLuaState);
	}

	if (pWriteLuaState)
		luaL_openlibs(pWriteLuaState);
	else
	{
		// Clean the Lua Stack
		clean(pWriteLuaState);
	}

	// Open the Lua file and create the Lua State
	// Return false if unable to open the Lua file
	if (luaL_loadfile(pWriteLuaState, Writefilename.c_str()) || lua_pcall(pWriteLuaState, 0, 0, 0)) {
		cout << "Error: script not loaded (" << Writefilename << ")" << endl;
		pWriteLuaState = NULL;
		return false;
	}

	// Display the file contents for debugging purposes
	if (bDisplayFileContent)
	{
		string STRING;
		ifstream infile;
		infile.open(Writefilename.c_str());
		while (!infile.eof()) // To get you all the lines.
		{
			getline(infile, STRING); // Saves the line in STRING.
			cout << STRING << endl; // Prints our STRING.
		}
		infile.close();
	}

	return true;
}

/**
 @brief Destroy this class instance
 */
void CScriptManager::Destroy(void)
{
	if (pLuaState)
	{
		// Clean the Lua Stack
		clean(pLuaState);
		// Close the Lua State
		lua_close(pLuaState);
		pLuaState = NULL;
	}
	if (pWriteLuaState)
	{
		// Clean the Lua Stack
		clean(pWriteLuaState);
		// Close the Lua State
		lua_close(pWriteLuaState);
		pWriteLuaState = NULL;
	}
}

/**
 @brief Stack Dump
 */
void CScriptManager::stackDump(lua_State* pStackDumpThisState)
{
	if (pStackDumpThisState == NULL)
	{
		cout << "Lua State" << pStackDumpThisState << "has NOT been initialised" << endl;
		return;
	}

	int i = lua_gettop(pStackDumpThisState);
	printf(" ----------------  Stack Dump ----------------\n");
	while (i) {
		int t = lua_type(pStackDumpThisState, i);
		switch (t) {
		case LUA_TSTRING:
			printf("%d:`%s'\n", i, lua_tostring(pStackDumpThisState, i));
			break;
		case LUA_TBOOLEAN:
			printf("%d: %s\n", i, lua_toboolean(pStackDumpThisState, i) ? "true" : "false");
			break;
		case LUA_TNUMBER:
			printf("%d: %g\n", i, lua_tonumber(pStackDumpThisState, i));
			break;
		default: 
			printf("%d: %s\n", i, lua_typename(pStackDumpThisState, t));
			break;
		}
		i--;
	}
	printf("--------------- Stack Dump Finished ---------------\n");
}

/**
 @brief PrintSelf
 */
void CScriptManager::PrintSelf(void)
{
	cout << "********************************************************************************" << endl;
	cout << "CScriptManager::PrintSelf" << endl;
	if (isLuaStateValid(pLuaState))
	{
		cout << "Lua State, " << pLuaState << " , has been initialised" << endl;
		stackDump(pLuaState);
	}
	else
		cout << "Lua State, " << pLuaState << " , has NOT been initialised" << endl;
	cout << "********************************************************************************" << endl;
	if (isLuaStateValid(pWriteLuaState))
	{
		cout << "Lua State, " << pWriteLuaState << " , has been initialised" << endl;
		stackDump(pWriteLuaState);
	}
	else
		cout << "Lua State, " << pWriteLuaState << " , has NOT been initialised" << endl;
	cout << "********************************************************************************" << endl;
}

/**
 @brief Print error while using Lua functions
 */
void CScriptManager::printError(const std::string& sVariableName, const std::string& sReason)
{
	cout << "Error: Unable to get [" << sVariableName << "] : " << sReason << endl;
}

/**
 @brief Get an Vector of Integers
 */
std::vector<int> CScriptManager::getIntVector(const std::string& name)
{
	std::vector<int> v;
	lua_gettostack(name.c_str());
	if (lua_isnil(pLuaState, -1)) { // array is not found
		return std::vector<int>();
	}
	lua_pushnil(pLuaState);
	while (lua_next(pLuaState, -2)) {
		v.push_back((int)lua_tonumber(pLuaState, -1));
		lua_pop(pLuaState, 1);
	}
	clean(pLuaState);
	return v;
}

/**
 @brief Get an Vector of String
 */
void CScriptManager::getStringVector(const std::string& name, std::vector<string>& keys, std::vector<int>& values)
{
	lua_gettostack(name.c_str());
	if (lua_isnil(pLuaState, -1)) { // array is not found
		return;
	}
	// stack now contains: -1 => nil; -2 => table
	lua_pushnil(pLuaState);
	while (lua_next(pLuaState, -2))
	{
		// stack now contains: -1 => value; -2 => key; -3 => table
		// copy the key so that lua_tostring does not modify the original
		lua_pushvalue(pLuaState, -2);
		// stack now contains: -1 => key; -2 => value; -3 => key; -4 => table

		// Push the key and value into the 2 vectors
		keys.push_back(lua_tostring(pLuaState, -1));
		values.push_back((int)lua_tonumber(pLuaState, -2));

		// pop value + copy of key, leaving original key
		lua_pop(pLuaState, 2);
		// stack now contains: -1 => key; -2 => table
	}

	// Clean the stack
	clean(pLuaState);
}

/**
 @brief Get a table of keys
 */
std::vector<std::string> CScriptManager::getTableKeys(const std::string& name)
{
	// function for getting table keys
	std::string code =	"function getKeys(name) "
						"s = \"\""
						"for k, v in ipairs(_G[name]) do "
						"    s = s..k..\",\" "
						"    end "
						"return s "
						"end";
	// Execute the code
	luaL_loadstring(pLuaState, code.c_str());
	lua_pcall(pLuaState, 0, 0, 0);
	// Get function; get ready to execute getKeys method
	lua_getglobal(pLuaState, "getKeys");
	lua_pushstring(pLuaState, name.c_str());
	// Execute getKeys method
	lua_pcall(pLuaState, 1, 1, 0);
	std::string test = lua_tostring(pLuaState, -1);
	std::vector<std::string> strings;
	std::string temp = "";
	std::cout << "TEMP:" << test << std::endl;
	// Loop through and get all the keys from the Lua State
	for (unsigned int i = 0; i < test.size(); i++) {
		if (test.at(i) != ',')
		{
			// Store the characters of a key into temp
			temp += test.at(i);
		}
		else
		{
			// Push the keys inside a temp into strings
			strings.push_back(temp);
			// Reset temp to empty string
			temp = "";
		}
	}
	// Clean the Lua State
	clean(pLuaState);
	// Return the string containing the keys
	return strings;
}


// Get distance square value through the ScriptManager Class
float CScriptManager::getDistanceSquareValue(const char* variableName, 
							glm::vec3 source, glm::vec3 destination)
{
	if (!pWriteLuaState) {
		printError(variableName, "Script is not loaded");
		return -1.0f;
	}

	lua_getglobal(pWriteLuaState, variableName);
	// If the variableName is not found...
	if (lua_isnil(pWriteLuaState, -1))
	{
		printError(variableName, " is not defined");
		return false;
	}

	lua_pushnumber(pWriteLuaState, source.x);
	lua_pushnumber(pWriteLuaState, source.y);
	lua_pushnumber(pWriteLuaState, source.z);
	lua_pushnumber(pWriteLuaState, destination.x);
	lua_pushnumber(pWriteLuaState, destination.y);
	lua_pushnumber(pWriteLuaState, destination.z);
	// Do a Lua call with debugging information returned.
	float distanceSquare = -1.0f;
	if (lua_pcall(pWriteLuaState, 6, 1, 0) != 0)
		std::cout << "Unable to call " << variableName << " : " << std::endl;
	else
		distanceSquare = (float)lua_tonumber(pWriteLuaState, -1);

	// Clean the stack
	clean(pWriteLuaState);

	return distanceSquare;
}

// Get variable number of values through the ScriptManager Class
bool CScriptManager::getVariableValues(const char* variableName,
		int& minValue, int& maxValue, int& avgValue, int& numValues,
		const int varCount, ...)
{
	if (!pWriteLuaState) {
		printError(variableName, "Script is not loaded");
		return false;
	}

	// Get the Lua function to the Lua Stack
	lua_getglobal(pWriteLuaState, variableName);
	// If the variableName is not found...
	if (lua_isnil(pWriteLuaState, -1))
	{
		printError(variableName, " is not defined");
		return false;
	}

	// Send the parameters to the Lua Stack
	double fVariable;
	va_list paramList;
	va_start(paramList, varCount);
	for (int i = 0; i<varCount; i++)
	{
		fVariable = va_arg(paramList, double);
		lua_pushnumber(pWriteLuaState, fVariable);
	}
	va_end(paramList);

	// Call the Lua function
	if (lua_pcall(pWriteLuaState, varCount, 4, 0) != 0)
		cout << "Error running function '" << variableName << 
			"': " << lua_tostring(pWriteLuaState, -1) << endl;

	// Retrieve results
	numValues = (int)lua_tonumber(pWriteLuaState, -1);
	lua_pop(pWriteLuaState, 1);
	avgValue = (int)lua_tonumber(pWriteLuaState, -1);
	lua_pop(pWriteLuaState, 1);
	maxValue = (int)lua_tonumber(pWriteLuaState, -1);
	lua_pop(pWriteLuaState, 1);
	minValue = (int)lua_tonumber(pWriteLuaState, -1);
	lua_pop(pWriteLuaState, 1);

	// Clean the stack
	clean(pWriteLuaState);

	return true;
}

/**
 @brief Clean the Lua State
 */
inline void CScriptManager::clean(lua_State* pCleanThisState)
{
	// If no Lua State was not specified, then clean the default Lua State
	if (pCleanThisState == NULL)
	{
		cout << "*** Lua State to clean was not defined. ***" << endl;
		pCleanThisState = pLuaState;
	}

	// Get the number of entries in the Lua Stack
	int n = lua_gettop(pCleanThisState);
	// Pop the number of entries in the Lua Stack
	lua_pop(pCleanThisState, n);
}

/**
 @brief Get to Stack
 */
bool CScriptManager::lua_gettostack(const string& variableName)
{
	level = 0;
	string var = "";

	// Loop through all the variables
	for (unsigned int i = 0; i < variableName.size(); i++)
	{
		if (variableName.at(i) == '.')
		{
			// If the variable is a Global variable...
			if (level == 0)
			{
				lua_getglobal(pLuaState, var.c_str());
			}
			else
			{
				lua_getfield(pLuaState, -1, var.c_str());
			}

			// If the variable is not found...
			if (lua_isnil(pLuaState, -1))
			{
				printError(variableName, var + " is not defined");
				return false;
			}
			else
			{
				var = "";
				level++;
			}
		}
		else
		{
			var += variableName.at(i);
		}
	}

	// If the variable is a Global variable...
	if (level == 0)
	{
		lua_getglobal(pLuaState, var.c_str());
	}
	else
	{
		lua_getfield(pLuaState, -1, var.c_str());
	}
	// If the variable is not found...
	if (lua_isnil(pLuaState, -1))
	{
		printError(variableName, var + " is not defined");
		return false;
	}

	return true;
}

/**
 @brief Check if the Lua State is valid
 */
bool CScriptManager::isLuaStateValid(lua_State* pCheckThisState)
{
	if (pCheckThisState)
		return true;

	cout << "Lua State has not been initialised" << endl;
	return false;
}
