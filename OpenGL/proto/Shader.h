#pragma once
#include <glad/glad.h>
#include "Math.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
class Shader
{

public:
	Shader(const char* vertexPath, const char* fragmentPath);
	void use();

	// getter/setter
	void setBool(const std::string& valiableName, bool  value) const;
	void setInt(const std::string& valiableName, int   value) const;
	void setFloat(const std::string& valiableName, float value) const;

	void setVec3(const std::string& valiableName, float x, float y, float z);
	void setVec3(const std::string& valiableName, Vector3& vec);
	void setVec4(const std::string& valiableName, float x, float y, float z, float w);
	void setVec4(const std::string& valiableName, Vector3& vec, float w);

	void setMatrix(const std::string& valiableName, const float* matrix);

	unsigned int GetID() { return ID; }
	void setTextureUniformString(std::string textureUniformName, unsigned int textureUnitStageNum);
	std::string getShaderStageUniformName(unsigned int textureUnitStageNum);

private:
	unsigned int ID;
	const unsigned int mMaxTextureUnitNum = 8;
	std::string mTextureUniformStrings[8];
};
