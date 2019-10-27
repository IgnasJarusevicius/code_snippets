#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "TextObj.h"
#include "AnimObj.h"
#include "Picture.h"
#include "Rectangle.h"
#include <chrono>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

constexpr unsigned SCR_WIDTH = 480;
constexpr unsigned SCR_HEIGHT = 360;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Test", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    BaseObj::SetViewSize(SCR_WIDTH, SCR_HEIGHT);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glewExperimental = GL_TRUE;
    GLenum res = glewInit();
    if (res != GLEW_OK) {
            fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
            return -1;
    }

    auto text = new TextObj(192.0f, 256.0f, "Text");
    text->SetColor(glm::vec3(1.0f,0.5f,0.25f));
    auto obj = new AnimObj(64.0f, 128.0f);
    auto obj2 = new Picture(64.0f, 256.0f);
    auto obj3 = new RectangleOutline(50.0f,50.0f,200.0f,20.0f);
    auto obj4 = new Rectangle(50.0f,80.0f,200.0f,20.0f);
    text->SetRotation(-45.0f);
    //text->SetScale(0.7f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    auto startTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = endTime - startTime;
        obj->Step(deltaTime.count());
        obj->Render();
        text->Render();
        obj2->Render();
        obj3->Render();
        obj4->Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
        startTime = endTime;
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    BaseObj::SetViewSize(width, height);
}
