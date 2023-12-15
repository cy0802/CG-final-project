#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Shader/Shader.h"
#include "../ObjReader/ObjReader.h"
class Light {
public:
	glm::vec3 position;
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	Light(glm::vec3 position) {
		this->position = position;
	}
};