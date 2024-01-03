#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Object/Object.h"
#include "Light/Light.h"
#include "Background/Background.h"
#include "../Includes/stb_image.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION


// ªù¤f camera position: > -1.69564e-07 ¸õ
Light light0(glm::vec3(1.0f, -1.0f, 1.0f));
//glm::vec3 camera = glm::vec3(1.0, -0.0, 0.0);
Camera camera(glm::vec3(0.0f, -0.7f, 4.25f));
//Object landscape((char*)"..\\resource\\landscape.obj", nullptr, (char*)"..\\resource\\landscape.jpg");
Object temple((char*)"..\\resource\\temple.obj", (char*)"..\\resource\\temple.mtl", (char*)"..\\resource\\temple.png");
// Object field((char*)"..\\resource\\grassfield.obj", nullptr, (char*)"..\\resource\\grassfield.png");
const unsigned int SCR_WIDTH = 1300;
const unsigned int SCR_HEIGHT = 1000;
GLFWwindow* window;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void init();
void processInput(GLFWwindow* window);
// void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei length, const GLchar* message, const void* userParam);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

int main() {
    init();
    Background background((char*)"..\\resource\\sky.jpg");
    //landscape.shader = Shader((char*)"..\\resource\\landscape.vert", (char*)"..\\resource\\landscape.frag");
    //landscape.setup(light0, camera);
    //// landscape.adjust(glm::vec3(174.0, 73.0, 0.0), glm::vec3(10.0, 10.0, 8.0), glm::vec3(0.4f, 1.0f, 0.0f));
    //landscape.adjust(glm::vec3(170.0, -83.0, 0.0), glm::vec3(10.0, 10.0, 8.0), glm::vec3(-2.0f, 2.0f, 0.0f));
    //landscape.loadTexture();
    //landscape.loadNormalMap((char*)"..\\resource\\landscape-normal.jpeg");
       
    
    temple.shader = Shader((char*)"..\\resource\\landscape.vert", (char*)"..\\resource\\landscape.frag");
    temple.setup(light0, camera.Position);
    // original translation (0.0f, 0.8f, 3.0f)
    temple.adjust(glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.3, 1.3, 1.3), glm::vec3(0.0, -1.1, 0.0));
    // temple.adjust(glm::vec3(177.0, -183.0, 0.0), glm::vec3(1.2, 1.4, 1.2), glm::vec3(0.0f, 0.0f, 0.0f));
    // temple.adjust(glm::vec3(168.0, -197.0, 0.0), glm::vec3(0.7, 0.7, 0.8), glm::vec3(0.0f, 0.6f, 3.0f));
    temple.loadTexture();

    /*field.shader = Shader((char*)"..\\resource\\landscape.vert", (char*)"..\\resource\\landscape.frag");
    field.setup(light0, camera);
    field.adjust(glm::vec3(-10.0, 0.0, 0.0), glm::vec3(4.0, 4.0, 4.0), glm::vec3(0.0f, 0.7f, 3.0f));
    field.loadTexture();
    field.loadNormalMap((char*)"..\\resource\\grassfield-normal.png");*/

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);
        glClearColor(0.8, 0.8, 0.8, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 viewpos = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        // glm::mat4 view = glm::lookAt(camera, viewpos, up);
        //landscape.shader.use();
        //landscape.shader.setMat4((char*)"view", landscape.translation);
        //// landscape.shader.setMat4((char*)"view", view * translate);
        //landscape.shader.setVec3((char*)"viewPos", camera);
        //landscape.shader.setMat4((char*)"local", landscape.rotation * landscape.scale);
        //landscape.draw();
        
        glDisable(GL_DEPTH_TEST);
        background.draw();

        temple.projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        temple.shader.use();
        temple.shader.setMat4((char*)"projection", temple.projection);
        temple.shader.setVec3((char*)"viewPos", camera.Position);
        temple.shader.setMat4((char*)"view", camera.GetViewMatrix() * temple.translation);
        temple.shader.setMat4((char*)"local", temple.rotation * temple.scale);
        glEnable(GL_DEPTH_TEST);
        temple.draw();
        

        /*field.shader.use();
        field.shader.setVec3((char*)"viewPos", camera);
        field.shader.setMat4((char*)"view", field.translation);
        field.shader.setMat4((char*)"local", field.rotation * field.scale);
        field.draw();*/


        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cout << "OpenGL Error: " << error << std::endl;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
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
    Object* target = &temple;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT)
            target->rotate(1.0, 'y');
        if (key == GLFW_KEY_RIGHT) 
            target->rotate(-1.0, 'y');
        if (key == GLFW_KEY_UP)
            target->rotate(1.0, 'x');
        if (key == GLFW_KEY_DOWN)
            target->rotate(-1.0, 'x');
        if (key == GLFW_KEY_1) 
            target->zoomin(0.05);
        if (key == GLFW_KEY_2)
            target->zoomin(-0.05);
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_W)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (key == GLFW_KEY_S)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (key == GLFW_KEY_A)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (key == GLFW_KEY_D)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        std::cout << "current rotate angle: " << target->rotateAngle.x << " "
            << target->rotateAngle.y << " " << target->rotateAngle.z << "\n";
        std::cout << "current scaling factor: " << target->scalingFactor.x << " " 
            << target->scalingFactor.y << " " << target->scalingFactor.z << "\n";
        std::cout << "current camera position: " << camera.Position.x << " " 
            << camera.Position.y << " " << camera.Position.z << "\n";
    }
}
void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cerr << "GL CALLBACK: " << message << std::endl;
}