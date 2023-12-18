#pragma once
#include <glad/glad.h>
#include <fstream>
#include <errno.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../Shader/Shader.h"
#include "../Light/Light.h"
#include "../ObjReader/ObjReader.h"

class Background {
public:
    float data[30] = {
        // position           texture coord
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // ¥ª¤U¨¤
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f,  // ¥k¤U¨¤
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,  // ¥k¤W¨¤

         -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,  // ¥ª¤U¨¤
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // ¥k¤W¨¤
         -1.0f,  1.0f, 0.0f,   0.0f, 1.0f   // ¥ª¤W¨¤
    };
    Shader shader;
    unsigned int texture;
    unsigned int VAO, VBO;
    Background(char* imgPath);
    void draw();
};