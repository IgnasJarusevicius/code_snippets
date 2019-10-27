#include "BaseObj.h"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 BaseObj::global_projection = glm::ortho(0.0f, 480.0f, 0.0f, 360.0f);

BaseObj::BaseObj(float xx, float yy) :
    transform(glm::mat4(1.0f)),
    x(xx),
    y(yy),
    scale(1.0f),
    rotation(0.0f)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
}

void BaseObj::SetViewSize(float xx, float yy)
{
    global_projection = glm::ortho(0.0f, xx, 0.0f, yy);
}

void BaseObj::SetRotation(float rot)
{
    rotation = rot;
    UpdateTransform();
}

void BaseObj::SetPosition(float xx, float yy)
{
    x = xx;
    y = yy;
    UpdateTransform();
}

void BaseObj::SetScale(float ratio)
{
    scale = ratio;
    UpdateTransform();
}

BaseObj::~BaseObj()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}





