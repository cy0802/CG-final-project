#pragma once
#include "Object.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Includes/stb_image.h"


Object::Object(char* objfile, char* mtlfile, char* textureImg) {
    this->init(objfile, mtlfile, textureImg);
}

void Object::init(char* objfile, char* mtlfile, char* textureImg) {
    std::vector<float> v = ObjReader::read(objfile);
    float _scale = *(v.end() - 1);
    if (*(v.end() - 2) > _scale) _scale = *(v.end() - 2);
    if (*(v.end() - 3) > _scale) _scale = *(v.end() - 3);
    // this->scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0 / _scale, 1.0 / _scale, 1.0 / _scale));
    // std::cout << "scale: " << 100.0 / _scale << "\n";
    this->scalingFactor = glm::vec3(1.0 / _scale, 1.0 / _scale, 1.0 / _scale);
    this->scale = glm::scale(glm::mat4(1.0f), this->scalingFactor);
    v.pop_back(); v.pop_back(); v.pop_back();
    this->sizeofData = v.size() / 8;
    std::copy(v.begin(), v.end(), this->data);
    rotation = rotationX = rotationY = rotationZ = glm::mat4(1.0f);
    // readMtl(mtlfile);
    this->texturePath = textureImg;
}

void Object::readMtl(char* mtlPath) {
    // read mtl: material file
    std::ifstream mtlFile;
    mtlFile.open(mtlPath);
    if (mtlFile.fail()) {
        std::cout << "error while opening the mtl file\n";
        char buffer[500];
        strerror_s(buffer, 500, errno);
        std::cout << "Error: " << buffer;
        exit(1);
    }
    else {
        std::cout << "open mtl file successfully\n";
    }
    std::string dataType;
    while (mtlFile >> dataType) {
        if (dataType.compare("Ka") == 0) {
            mtlFile >> ambient.x >> ambient.y >> ambient.z;
        }
        else if (dataType.compare("Kd") == 0) {
            mtlFile >> diffuse.x >> diffuse.y >> diffuse.z;
        }
        else if (dataType.compare("Ks") == 0) {
            mtlFile >> specular.x >> specular.y >> specular.z;
        }
        else if (dataType.compare("Ns") == 0) {
            mtlFile >> opticalDensity;
        }
        else {
            char buf[100];
            mtlFile.getline(buf, 100);
            // std::cout << buf << "\n";
        }
    }
}

void Object::adjust(glm::vec3 rotateAngle, glm::vec3 _scalingFactor, glm::vec3 translation) {
    rotate(rotateAngle.x, 'x');
    rotate(rotateAngle.y, 'y');
    rotate(rotateAngle.z, 'z');
    this->scale = glm::scale(this->scale, _scalingFactor);
    this->scalingFactor = this->scalingFactor * _scalingFactor;
    this->translation = glm::translate(glm::mat4(1.0f), translation);
}

void Object::setup(Light light, glm::vec3 camera) {
    this->shader.use();
    this->shader.setInt((char*)"texture_", 0);
    this->shader.setMat4((char*)"projection", this->projection);
    this->shader.setMat4((char*)"model", this->model);

    // this->shader.setVec3((char*)"objectColor", this->color);
    // this->shader.setMat4((char*)"local", this->rotation);
    this->shader.setMat4((char*)"local", this->rotation * this->scale);
    // this->shader.setVec3((char*)"viewPos", camera);

    // this->shader.setVec3((char*)"light.position", light.position);
    // this->shader.setVec3((char*)"light.color", light.color);

    // this->shader.setVec3((char*)"material.ambient", this->ambient);
    // this->shader.setVec3((char*)"material.diffuse", this->diffuse);
    // this->shader.setVec3((char*)"material.specular", this->specular);
}
unsigned int Object::loadTexture() {
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    std::cout << "(loadTexture): " << this->texturePath << std::endl;
    unsigned char* textureImg = stbi_load(this->texturePath, &width, &height, &nrChannels, 0);
    if (textureImg) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImg);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "successfully load texture\nheight = " << height
            << ", width = " << width << '\n';
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(textureImg);
    return this->texture;
}
void Object::rotate(float angle, char axis) {
	if (axis == 'x') {
		rotationX = rotationX * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateAngle.x += angle;
	} else if (axis == 'y') {
		rotationY = rotationY * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateAngle.y += angle;
	} else {
		rotationZ = rotationZ * glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        rotateAngle.z += angle;
	}
	rotation = rotationX * rotationY * rotationZ;
}
void Object::zoomin(float _scale) {
    float rate = (scalingFactor.x + _scale) / scalingFactor.x;
    scalingFactor = glm::vec3(scalingFactor.x + _scale, rate * scalingFactor.y, rate * scalingFactor.z);
    this->scale = glm::scale(glm::mat4(1.0f), this->scalingFactor);
}