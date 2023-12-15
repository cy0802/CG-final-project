#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object/Object.h"
#include "Light/Light.h"

Light light0(glm::vec3(1.0f, -1.0f, 1.0f));
glm::vec3 camera = glm::vec3(1.0, -0.0, 0.0);
Object landscape((char*)"..\\resource\\landscape.obj", (char*)"..\\resource\\landscape\\landscpae.mtl", (char*)"..\\resource\\landscape.png");
const unsigned int SCR_WIDTH = 1300;
const unsigned int SCR_HEIGHT = 1300;
GLFWwindow* window;
unsigned int VAO, VBO;
unsigned int texture;

void init();
void processInput(GLFWwindow* window);
// void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main() {
    init();
    landscape.shader = Shader((char*)"..\\resource\\landscape.vert", (char*)"..\\resource\\landscape.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(landscape.data), landscape.data, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(2);
    landscape.setup(light0, camera);
    landscape.rotate(180.0, 'x');
    texture = landscape.loadTexture();
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.8, 0.8, 0.8, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 viewpos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        // glm::mat4 view = glm::lookAt(camera, viewpos, up);
        landscape.shader.setMat4((char*)"view", translate);
        // landscape.shader.setMat4((char*)"view", view * translate);
        // landscape.shader.setVec3((char*)"viewPos", camera);
        landscape.shader.setMat4((char*)"local", landscape.rotation * landscape.scale);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, landscape.sizeofData);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "OpenGL Error: " << error << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

	return 0;
}


void init() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Object", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // glfwSetCursorPosCallback(window, mouseCallback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    glDebugMessageCallback(MessageCallback, 0);
    glEnable(GL_DEPTH_TEST);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// TODO: bug here
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::cout << "keyCallback()\n";
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT) {
            landscape.rotate(1.0, 'y');
        }
        if (key == GLFW_KEY_RIGHT) {
            landscape.rotate(-1.0, 'y');
        }
        if (key == GLFW_KEY_UP) {
            landscape.rotate(1.0, 'x');
        }
        if (key == GLFW_KEY_DOWN) {
            landscape.rotate(-1.0, 'x');
        }
    }
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "GL CALLBACK: " << message << std::endl;
}