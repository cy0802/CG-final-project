#include "Shader.h"
Shader::Shader() {
	id = -1;
};
Shader::Shader(char* verShaderPath, char* fragShaderPath) {
	std::string verCode, fragCode;
	std::ifstream verFile, fragFile;
	std::stringstream verStream, fragStream;
	verFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		verFile.open(verShaderPath);
		verStream << verFile.rdbuf();
		verFile.close();
		verCode = verStream.str();
		fragFile.open(fragShaderPath);
		fragStream << fragFile.rdbuf();
		fragFile.close();
		fragCode = fragStream.str();
	} catch (std::ifstream::failure& e) {
		std::cout << "error while opening shader files: " << e.what() << "\n";
	}
	

	const char* _verCode = verCode.c_str();
	// std::cout << _verCode << std::endl;
	const char* _fragCode = fragCode.c_str();
	// std::cout << _fragCode << std::endl;
	unsigned int verID, fragID;
	// vertex shader
	verID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(verID, 1, &_verCode, NULL);
	glCompileShader(verID);
	errorChecking(verID, "VERTEX");
	
	// fragment shader
	fragID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragID, 1, &_fragCode, NULL);
	glCompileShader(fragID);
	errorChecking(fragID, "FRAGMENT");
	
	// shader program
	id = glCreateProgram();
	glAttachShader(id, verID);
	glAttachShader(id, fragID);
	glLinkProgram(id);
	errorChecking(id, "PROGRAM");
	
	// delete the shaders
	glDeleteShader(verID);
	glDeleteShader(fragID);
}

void Shader::use() {
	glUseProgram(id);
}

void Shader::set4float(char name[], float x, float y, float z, float w) {
	glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}
void Shader::setVec3(char name[], glm::vec3 vec) {
	glUniform3fv(glGetUniformLocation(id, name), 1, &vec[0]);
}
void Shader::setMat4(char name[], glm::mat4 matrix) {
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &matrix[0][0]);
}
void Shader::setInt(char name[], unsigned int x) {
	glUniform1i(glGetUniformLocation(id, name), x);
}
void Shader::errorChecking(unsigned int shaderID, std::string type) {
	int success;
	char infoLog[1024];
	if (type != "PROGRAM") {
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shaderID, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" 
				<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	} else {
		glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shaderID, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" 
				<< infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}