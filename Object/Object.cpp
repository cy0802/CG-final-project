#pragma once
#include "Object.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../Includes/stb_image.h"


Object::Object(char* objfile, char* mtlfile, char* textureImg) {
    this->init(objfile, mtlfile, textureImg);
}

Object::~Object() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Object::init(char* objfile, char* mtlfile, char* textureImg) {
    std::cout << "Read Obj from " << objfile << "\n";
    std::vector<float> v = ObjReader::read(objfile);
    std::cout << "---------------------------------------------------------\n";
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
    std::cout << "Read Mtl from " << mtlfile << "\n";
    readMtl(mtlfile);
    std::cout << "---------------------------------------------------------\n";
    this->texturePath = textureImg;
}

void Object::readMtl(char* mtlPath) {
    if (mtlPath == nullptr) {
        ambient = glm::vec3(0.3, 0.3, 0.3);
        diffuse = glm::vec3(1.0, 1.0, 1.0);
        specular = glm::vec3(0.5, 0.5, 0.5);
        return;
    }
    // read mtl: material file
    std::ifstream mtlFile;
    mtlFile.open(mtlPath);
    if (mtlFile.fail()) {
        std::cout << "error while opening the mtl file\n";
        char buffer[500];
        strerror_s(buffer, 500, errno);
        std::cout << "Error: " << buffer;
        
        return;
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
        else if (dataType.compare("Ni") == 0) {
            mtlFile >> opticalDensity;
        }
        else {
            char buf[100];
            mtlFile.getline(buf, 100);
            // std::cout << buf << "\n";
        }
    }
    std::cout << "Mtl Ambient: (" << ambient.x << ", " << ambient.y << ", " << ambient.z << ")\n";
    std::cout << "Mtl Diffuse: (" << diffuse.x << ", " << diffuse.y << ", " << diffuse.z << ")\n";
    std::cout << "Mtl Specular: (" << specular.x << ", " << specular.y << ", " << specular.z << ")\n";
}

void Object::adjust(glm::vec3 rotateAngle, glm::vec3 _scalingFactor, glm::vec3 translation) {
    rotate(rotateAngle.x, 'x');
    rotate(rotateAngle.y, 'y');
    rotate(rotateAngle.z, 'z');
    this->scale = glm::scale(this->scale, _scalingFactor);
    this->scalingFactor = this->scalingFactor * _scalingFactor;
    position = translation;
    this->translation = glm::translate(glm::mat4(1.0f), position);
}

void Object::draw() {
    glBindVertexArray(VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (useNormalMap) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, normalMapTexture);
    }
    this->shader.use();
    this->shader.setInt((char*)"texture_", 0);
    this->shader.setMat4((char*)"local", rotation * scale);
    this->shader.setInt((char*)"useNormalMap", useNormalMap);
    if(useNormalMap) this->shader.setInt((char*)"normalMap", 1);
    glDrawArrays(GL_TRIANGLES, 0, sizeofData);
}

void Object::setup(Light light, glm::vec3 camera) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(this->data), this->data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);

    this->shader.use();
    this->shader.setInt((char*)"texture_", 0);
    this->shader.setMat4((char*)"projection", this->projection);
    this->shader.setMat4((char*)"model", this->model);

    // this->shader.setVec3((char*)"objectColor", this->color);
    // this->shader.setMat4((char*)"local", this->rotation);
    this->shader.setMat4((char*)"view", this->translation);
    this->shader.setMat4((char*)"local", this->rotation * this->scale);
    this->shader.setVec3((char*)"viewPos", camera);

    this->shader.setVec3((char*)"light.position", light.position);
    this->shader.setVec3((char*)"light.color", light.color);

    this->shader.setVec3((char*)"material.ambient", this->ambient);
    this->shader.setVec3((char*)"material.diffuse", this->diffuse);
    this->shader.setVec3((char*)"material.specular", this->specular);
}
void Object::loadTexture() {
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
    // return this->texture;
}
void Object::loadNormalMap(char* normalMapPath) {
    useNormalMap = true;
    glGenTextures(1, &normalMapTexture);
    glBindTexture(GL_TEXTURE_2D, normalMapTexture);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load normal map image, create texture, and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    std::cout << "(loadNormalMap): " << normalMapPath << std::endl;
    unsigned char* normalMapImg = stbi_load(normalMapPath, &width, &height, &nrChannels, 0);
    if (normalMapImg) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, normalMapImg);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "successfully load normal map\nheight = " << height
            << ", width = " << width << '\n';
    }
    else {
        std::cout << "Failed to load normal map" << std::endl;
        std::cout << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(normalMapImg);
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
void Object::move(float dis, char direction) {
    if (direction == 'x') {
        position.x += dis;
    }
    else if (direction == 'y') {
        position.y += dis;
    }
    else if (direction == 'z') {
        position.z += dis;
    }
    translation = glm::translate(glm::mat4(1.0), position);
}