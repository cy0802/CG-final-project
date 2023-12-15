#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <glm/glm.hpp>
class Shader {
public:
	unsigned int id;
	Shader(char* verShaderPath, char* fragShaderPath);
	Shader();
	void use();
	void set4float(char name[], float x, float y, float z, float w);
	void setMat4(char name[], glm::mat4 matrix);
	void setVec3(char name[], glm::vec3);
	void setInt(char name[], unsigned int x);
private:
	void errorChecking(unsigned int shaderID, std::string type);
};