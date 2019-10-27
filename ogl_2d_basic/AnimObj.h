#pragma once
#include "BaseObj.h"
#include <string>
#include <map>

class Shader;

class AnimObj : public BaseObj
{
public: 
    AnimObj(float xx, float yy);
    ~AnimObj();
    void Render() override;
    void Step(float delta_time);
private:   
    void UpdateTransform() override; 
    float offset_x;
    float offset_y;
    std::vector<Vertex> vertices;
    unsigned int ebo; //element buffer object
    float frame;
    static void LoadResources(); 
    static int width;
    static int height;
    static unsigned int texture;
    static Shader* shader; 
};
