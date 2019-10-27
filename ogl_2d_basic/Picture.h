#pragma once
#include "BaseObj.h"

class Shader;

class Picture : public BaseObj
{
public: 
    Picture(float xx, float yy);
    ~Picture();
    void Render() override;
private:
    
    void UpdateTransform() override; 
    std::vector<Vertex> vertices;
    unsigned int ebo; //element buffer object
    glm::mat4 transform;
    
    static void LoadResources();  
    static int width;
    static int height;
    static unsigned int texture;
    static Shader* shader; 
};
