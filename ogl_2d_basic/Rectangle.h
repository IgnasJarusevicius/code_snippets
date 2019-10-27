#pragma once
#include "BaseObj.h"

class Shader;
struct Point2D;

class Rectangle : public BaseObj
{
public: 
    Rectangle(float xx, float yy, float width, float height);
    virtual void Render() override;
protected:    
    void UpdateTransform() override; 
    std::vector<Point2D> vertices;   
    static void LoadResources(); 
    static Shader* shader; 
};

class RectangleOutline : public Rectangle
{
public:
    RectangleOutline(float x, float y, float w, float h);
    void Render() override;
};
