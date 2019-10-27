#pragma once
#include "BaseObj.h"
#include <string>
#include <map>

struct Character;
class Shader;

class TextObj : public BaseObj
{
public: 
    TextObj(float xx, float yy, std::string text);
    ~TextObj();
    void Render() override;
    void SetText(std::string text);
    void SetColor(glm::vec3 col);
private:   
    void UpdateTransform() override;
    void AddSymbol(char symbol);  
    float offset_x;
    float offset_y;
    std::vector<Vertex> vertices;
    unsigned int ebo;
    glm::vec3 color;  
    static void LoadResources(); 
    static std::map<char, const Character*> charMap;
    static int width;
    static int height;
    static unsigned int texture;
    static Shader* shader; 
};
