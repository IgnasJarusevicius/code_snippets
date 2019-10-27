#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Vertex
{
    Vertex(float xx, float yy, float uu, float vv):x(xx),y(yy),z(0.0f),u(uu),v(vv){};
    float x;
    float y;
    float z;
    float u;
    float v;
};

class BaseObj
{
public: 
    BaseObj(float xx, float yy);
    virtual ~BaseObj();
    virtual void Render() = 0;
    void SetPosition(float xx, float yy);
    void SetScale(float ratio);
    void SetRotation(float rot);
    static void SetViewSize(float xx, float yy);
protected:   
    virtual void UpdateTransform() = 0; 
    std::vector<unsigned int> indices;
    unsigned int vbo;
    unsigned int vao;
    glm::mat4 transform;
    float x;
    float y;
    float scale;
    float rotation;
    static glm::mat4 global_projection;   
};
