#include "Collider.h"

// Include Shader Manager
#include "..\RenderControl\ShaderManager.h"

#include <GLFW/glfw3.h>

/**
 @brief Default Constructor
 */
CCollider::CCollider(void)
	: vec3TopRight(glm::vec3(0.5f, 0.5f, 0.5f))
	, vec3BottomLeft(glm::vec3(-0.5f, -0.5f, -0.5f))
	, vec4Colour(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f))
	, bIsDisplayed(true)
	, fLineWidth(3.0)
{
}

/**
 @brief Default Destructor
 */
CCollider::~CCollider(void)
{
}

/**
 @brief Initialise this class instance
 */
bool CCollider::Init(void)
{
	// set up vertex data (and buffer(s)) and configure vertex attributes
	float vertices[] = {
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		vec3BottomLeft.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		vec3BottomLeft.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3BottomLeft.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,

		vec3BottomLeft.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3TopRight.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3TopRight.y, vec3TopRight.z, vec4Colour.x, vec4Colour.y, vec4Colour.z,
		vec3BottomLeft.x, vec3TopRight.y, vec3BottomLeft.z, vec4Colour.x, vec4Colour.y, vec4Colour.z
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture coord attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glLineWidth(fLineWidth);

	return true;
}

/**
 @brief Set a shader to this class instance
 */
void CCollider::SetLineShader(const std::string& _name)
{
	sLineShaderName = _name;
}

/**
 @brief PreRender
 */
void CCollider::PreRender(void)
{
	// Use the shader defined for this class
	CShaderManager::GetInstance()->Use(sLineShaderName);
}

/**
 @brief Render
 */
void CCollider::Render(void)
{
	if (!bIsDisplayed)
		return;

	CShaderManager::GetInstance()->activeShader->setMat4("model", model);
	CShaderManager::GetInstance()->activeShader->setMat4("view", view);
	CShaderManager::GetInstance()->activeShader->setMat4("projection", projection);

	// render box
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_LOOP, 0, 36);
}

/**
 @brief PostRender
 */
void CCollider::PostRender(void)
{

}
