#include "Rectangle.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

struct Point2D
{
    Point2D(float xx, float yy):x(xx),y(yy){};
    float x;
    float y;
};

static const char *vertexShaderSource =
"#version 330 core\n\
layout (location = 0) in vec2 aPos;\n\
uniform mat4 gWVP;\n\
void main(){\n\
	gl_Position = gWVP * vec4(aPos, 0.0, 1.0);\n\
    }\0";

static const char *fragmentShaderSource =
"#version 330 core\n\
out vec4 FragColor;\n\
uniform vec3 color = vec3(1.0,1.0,1.0);\n\
void main(){FragColor = vec4(color,1.0);}\0";

Shader* Rectangle::shader = nullptr;

Rectangle::Rectangle(float xx, float yy, float width, float height) :
    BaseObj(xx, yy)
{
    LoadResources();;

    vertices.push_back(Point2D(width,height));
    vertices.push_back(Point2D(width,0.0f));
    vertices.push_back(Point2D(0.0f,0.0f));
    vertices.push_back(Point2D(0.0f,height));
    vertices.push_back(Point2D(width,height));
    vertices.push_back(Point2D(0.0f,0.0f));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Point2D)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);         // position attribute
    glEnableVertexAttribArray(0);
    UpdateTransform();
}

void Rectangle::UpdateTransform()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = global_projection*model;
}

void Rectangle::LoadResources()
{
    if (shader == nullptr)
        shader = new Shader(vertexShaderSource, fragmentShaderSource);
}

void Rectangle::Render()
{
    shader->use();
    shader->setMat4("gWVP", transform);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

RectangleOutline::RectangleOutline(float x, float y, float w, float h):
    Rectangle(x,  y, w, h)
{
}

void RectangleOutline::Render()
{
    shader->use();
    shader->setMat4("gWVP", transform);
    glBindVertexArray(vao);
    glDrawArrays(GL_LINE_STRIP, 0, 5);
}




