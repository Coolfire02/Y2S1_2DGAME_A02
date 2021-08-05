#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include <string>
using namespace std;

class CCollider
{
public:
	// Attributes of the Shader
	glm::mat4 model; // make sure to initialize matrix to identity matrix first
	glm::mat4 view;
	glm::mat4 projection;

	// Bounding Box specifications
	// Bounding Box top right position
	glm::vec3 vec3TopRight;
	// Bounding Box bottom left position
	glm::vec3 vec3BottomLeft;
	// Bounding Box colour
	glm::vec4 vec4Colour;

	// Boolean flag to indicate if Bounding Box is displayed
	bool bIsDisplayed;
	// Line width
	float fLineWidth;

	// Constructor(s)
	CCollider(void);
	// Destructor
	virtual ~CCollider(void);

	// Initialise this class instance
	bool Init(void);

	// Set a shader to this class instance
	virtual void SetLineShader(const std::string& _name);

	// PreRender
	virtual void PreRender(void);
	// Render
	virtual void Render(void);
	// PostRender
	virtual void PostRender(void);

protected:
	std::string sLineShaderName;
	unsigned int VAO, VBO;
};

