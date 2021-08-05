/**
 CEntity3D
 By: Toh Da Jun
 Date: Apr 2020
 */
#include "Entity3D.h"

// Include ImageLoader
#include "..\System\ImageLoader.h"

#include <iostream>
using namespace std;

/**
@brief Default Constructor
*/
CEntity3D::CEntity3D()
	: cSettings(NULL)
	, cCollider(NULL)
	, sShaderName("")
	, VAO(0)
	, VBO(0)
	, iTextureID(0)
	, model(glm::mat4(1.0f))
	, view(glm::mat4(1.0f))
	, projection(glm::mat4(1.0f))
	, eType(OTHERS)
	, vec3Position(0.0f)
	, vec3Front(glm::vec3(0.0f, 0.0f, -1.0f))
	, vec3PreviousPosition(0.0f)
	, vec3Scale(1.0f)
	, fRotationAngle(0.0f)
	, vec3RotationAxis(1.0f)
	, vec4Colour(1.0f)
	, fMovementSpeed(2.5f)
	, bToDelete(false)
{
}

/**
@brief Default Destructor
*/
CEntity3D::~CEntity3D(void)
{
	if (cCollider)
	{
		delete cCollider;
		cCollider = NULL;
	}

	// We set this to NULL, since it was created elsewhere so we don't delete it here
	cSettings = NULL;
}

/**
@brief Initialise this instance to default values
*/
bool CEntity3D::Init(void)
{
	// Get the handler to the CSettings Singleton
	cSettings = CSettings::GetInstance();

	return true;
}


/**
 @brief Set the name of the shader to be used in this class instance
 @param _name The name of the Shader instance in the CShaderManager
 */
void CEntity3D::SetShader(const std::string& _name)
{
	this->sShaderName = _name;
}

// Set methods
void CEntity3D::SetTextureID(const GLuint iTextureID)
{
	this->iTextureID = iTextureID;
}
void CEntity3D::SetType(const TYPE eType)
{
	this->eType = eType;
}
void CEntity3D::SetPosition(const glm::vec3 vec3Position)
{
	this->vec3Position = vec3Position;
}
void CEntity3D::SetPreviousPosition(const glm::vec3 vec3PreviousPosition)
{
	this->vec3PreviousPosition = vec3PreviousPosition;
}
void CEntity3D::SetFront(const glm::vec3 vec3Front)
{
	this->vec3Front = vec3Front;
}
void CEntity3D::SetScale(const glm::vec3 vec3Scale)
{
	this->vec3Scale = vec3Scale;
}
void CEntity3D::SetColliderScale(const glm::vec3 vec3Scale)
{
	this->vec3ColliderScale = vec3Scale;
}

void CEntity3D::SetRotation(const float fRotationAngle, const glm::vec3 vec3RotationAxis)
{
	this->fRotationAngle = fRotationAngle;
	this->vec3RotationAxis = vec3RotationAxis;
}
void CEntity3D::SetColour(const glm::vec4 vec4Colour)
{
	this->vec4Colour = vec4Colour;
}
void CEntity3D::SetMovementSpeed(const float fMovementSpeed)
{
	this->fMovementSpeed = fMovementSpeed;
}

// Get methods
const GLuint CEntity3D::GetTextureID(void) const
{
	return iTextureID;
}
const CEntity3D::TYPE CEntity3D::GetType(void) const
{
	return eType;
}
const glm::vec3 CEntity3D::GetPosition(void) const
{
	return vec3Position;
}
const glm::vec3 CEntity3D::GetPreviousPosition(void) const
{
	return vec3PreviousPosition;
}
const glm::vec3 CEntity3D::GetFront(void) const
{
	return vec3Front;
}
const glm::vec3 CEntity3D::GetScale(void) const
{
	return vec3Scale;
}
const glm::vec3 CEntity3D::GetColliderScale(void) const
{
	return vec3ColliderScale;
}
const float CEntity3D::GetRotationAngle(void) const
{
	return fRotationAngle;
}
const glm::vec3 CEntity3D::GetRotationAxis(void) const
{
	return vec3RotationAxis;
}
const glm::vec4 CEntity3D::GetColour(void) const
{
	return vec4Colour;
}
const float CEntity3D::GetMovementSpeed(void) const
{
	return fMovementSpeed;
}

// These methods are for marking this CEntity3D for deletion
/**
 @brief Mark this CEntity3D for deletion
 @param bToDelete A const bool variable which means this CEntity3D is to be deleted if true, else false 
 */
void CEntity3D::SetToDelete(const bool bToDelete)
{
	this->bToDelete = bToDelete;
}

/**
 @brief Check if this CEntity3D is to be deleted
 @return A bool value of true means this CEntity3D is to be deleted, else false
 */
const bool CEntity3D::IsToDelete(void) const
{
	return bToDelete;
}

/**
@brief Activate the CCollider for this class instance
@param cLineShader A Shader* variable which stores a shader which renders lines
*/
void CEntity3D::ActivateCollider(const std::string& _name)
{
	cCollider = new CCollider();
	cCollider->Init();
	cCollider->SetLineShader(_name);
}

/**
@brief Get Sphere Radius
*/
float CEntity3D::GetSphereRadius(void)
{
	if (cCollider == NULL)
	{
		std::cout << "cCollider has not been initialised for this CEntity3D." << std::endl;
	}

	// Calculate sphere radius
	glm::vec3 tempVec3A_BottomLeft = vec3ColliderScale * cCollider->vec3BottomLeft;
	glm::vec3 tempVec3A_TopRight = vec3ColliderScale * cCollider->vec3TopRight;

	return glm::length(tempVec3A_TopRight - tempVec3A_BottomLeft) * 0.5f;
}

/**
@brief Check for collision with another Entity3D
@param cCEntity3D A const CEntity3D* variable which is to be tested against this CEntity3D instance
@return true is they are colliding with each other, else false
*/
bool CEntity3D::CheckForCollision(const CEntity3D* cEntity3D)
{
	bool bResult = false;

	tempVec3A_BottomLeft = vec3Position + vec3ColliderScale * cCollider->vec3BottomLeft;
	tempVec3A_TopRight = vec3Position + vec3ColliderScale * cCollider->vec3TopRight;
	tempVec3B_BottomLeft = cEntity3D->vec3Position + vec3ColliderScale * cEntity3D->cCollider->vec3BottomLeft;
	tempVec3B_TopRight = cEntity3D->vec3Position + vec3ColliderScale * cEntity3D->cCollider->vec3TopRight;

	// Check for collision using Sphere-Sphere intersection test
	// If true, then check using AABB 
	if (((glm::length(tempVec3A_TopRight - tempVec3A_BottomLeft) +
		 glm::length(tempVec3B_TopRight - tempVec3B_BottomLeft)) * 0.5f) >
		 glm::length(vec3Position - cEntity3D->vec3Position))
	{
		// Check if the bottom left of the cEntity3D is within this collider
		if ((tempVec3A_BottomLeft.x <= tempVec3B_BottomLeft.x) &&
			(tempVec3A_BottomLeft.y <= tempVec3B_BottomLeft.y) &&
			(tempVec3A_BottomLeft.z <= tempVec3B_BottomLeft.z))
		{
			if ((tempVec3B_BottomLeft.x <= tempVec3A_TopRight.x) &&
				(tempVec3B_BottomLeft.y <= tempVec3A_TopRight.y) &&
				(tempVec3B_BottomLeft.z <= tempVec3A_TopRight.z))
			{
				bResult = true;
			}
		}

		// Check if the top right of the cEntity3D is within this collider
		if ((tempVec3A_BottomLeft.x <= tempVec3B_TopRight.x) &&
			(tempVec3A_BottomLeft.y <= tempVec3B_TopRight.y) &&
			(tempVec3A_BottomLeft.z <= tempVec3B_TopRight.z))
		{
			if ((tempVec3B_TopRight.x <= tempVec3A_TopRight.x) &&
				(tempVec3B_TopRight.y <= tempVec3A_TopRight.y) &&
				(tempVec3B_TopRight.z <= tempVec3A_TopRight.z))
			{
				bResult = true;
			}
		}

		// Check if the bottom left of this collider is within this cEntity3D
		if ((tempVec3B_BottomLeft.x <= tempVec3A_BottomLeft.x) &&
			(tempVec3B_BottomLeft.y <= tempVec3A_BottomLeft.y) &&
			(tempVec3B_BottomLeft.z <= tempVec3A_BottomLeft.z))
		{
			if ((tempVec3A_BottomLeft.x <= tempVec3B_TopRight.x) &&
				(tempVec3A_BottomLeft.y <= tempVec3B_TopRight.y) &&
				(tempVec3A_BottomLeft.z <= tempVec3B_TopRight.z))
			{
				bResult = true;
			}
		}

		// Check if the top right of this collider is within this cEntity3D
		if ((tempVec3B_BottomLeft.x <= tempVec3A_TopRight.x) &&
			(tempVec3B_BottomLeft.y <= tempVec3A_TopRight.y) &&
			(tempVec3B_BottomLeft.z <= tempVec3A_TopRight.z))
		{
			if ((tempVec3A_TopRight.x <= tempVec3B_TopRight.x) &&
				(tempVec3A_TopRight.y <= tempVec3B_TopRight.y) &&
				(tempVec3A_TopRight.z <= tempVec3B_TopRight.z))
			{
				bResult = true;
			}
		}
	}

	return bResult;
}

/**
 @brief Store position for rollback
 */
void CEntity3D::StorePositionForRollback(void)
{
	vec3PreviousPosition = vec3Position;
}

/**
 @brief Rollback the position to the previous position
 */
void CEntity3D::RollbackPosition(void)
{
	vec3Position = vec3PreviousPosition;
}

/**
@brief Load a texture, assign it a code and store it in MapOfTextureIDs.
@param filename A const char* variable which contains the file name of the texture
@return true if the initialisation is successful, else false
*/
int CEntity3D::LoadTexture(const char* filename)
{
	// Texture ID
	GLuint iTextureID = 0;

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
		return 0;
	}
	// Free up the memory of the file data read in
	free(data);

	return iTextureID;
}

/**
@brief Distance Squared between 2 Vector3 positions
@param thisVector glm::vec3
@param thatVector glm::vec3
@return double
*/
double CEntity3D::DistanceSquaredBetween(glm::vec3 thisVector, glm::vec3 thatVector)
{
	return	((thatVector.x - thisVector.x)*(thatVector.x - thisVector.x) +
			(thatVector.y - thisVector.y)*(thatVector.y - thisVector.y) +
			(thatVector.z - thisVector.z)*(thatVector.z - thisVector.z));
}
