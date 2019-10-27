#include "TextObj.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct Character
{
    int id;
    int x_pos;
    int y_pos;
    int width;
    int height;
    int Xoffset;
    int Yoffset;
    int origW;
    int origH;
};

const std::vector<Character> font_lut =
{
    {32,	0,	0,	0,	0,	14,	56,	14,	56},
    {97,	124,	153,	26,	26,	2,	19,	27,	56},
    {98,	324,	0,	23,	35,	3,	10,	27,	56},
    {99,	139,	181,	22,	26,	2,	19,	25,	56},
    {100,	349,	0,	23,	35,	2,	10,	27,	56},
    {101,	17,	153,	24,	26,	2,	19,	27,	56},
    {102,	288,	38,	14,	35,	0,	10,	14,	56},
    {103,	249,	0,	23,	36,	2,	19,	27,	56},
    {04,	336,	37,	22,	35,	3,	10,	27,	56},
    {105,	281,	38,	5,	35,	3,	10,	11,	56},
    {106,	93,	0,	10,	45,	-2,	10,	11,	56},
    {107,	360,	37,	22,	35,	3,	10,	25,	56},
    {108,	374,	0,	5,	35,	3,	10,	11,	56},
    {109,	152,	153,	35,	26,	3,	19,	41,	56},
    {110,	115,	181,	22,	26,	3,	19,	27,	56},
    {111,	189,	179,	24,	26,	2,	19,	27,	56},
    {112,	299,	0,	23,	36,	3,	19,	27,	56},
    {113,	274,	0,	23,	36,	2,	19,	27,	56},
    {114,	2,	152,	13,	26,	3,	19,	16,	56},
    {115,	91,	181,	22,	26,	1,	19,	25,	56},
    {116,	6,	118,	14,	32,	0,	13,	14,	56},
    {117,	44,	181,	21,	26,	3,	19,	27,	56},
    {118,	97,	153,	25,	26,	0,	19,	25,	56},
    {119,	189,	151,	37,	26,	-1,	19,	35,	56},
    {120,	163,	181,	24,	26,	0,	19,	25,	56},
    {121,	222,	0,	25,	36,	0,	19,	25,	56},
    {122,	67,	181,	22,	26,	1,	19,	25,	56},
    {65,	128,	83,	33,	33,	0,	12,	33,	56},
    {66,	0,	83,	27,	33,	4,	12,	33,	56},
    {67,	133,	45,	32,	34,	2,	11,	35,	56},
    {68,	292,	111,	30,	33,	4,	12,	35,	56},
    {69,	51,	118,	27,	33,	4,	12,	33,	56},
    {70,	324,	146,	24,	33,	4,	12,	30,	56},
    {71,	167,	44,	33,	34,	2,	11,	38,	56},
    {72,	170,	116,	28,	33,	4,	12,	35,	56},
    {73,	6,	47,	6,	33,	4,	12,	14,	56},
    {74,	228,	150,	21,	33,	0,	12,	25,	56},
    {75,	230,	115,	29,	33,	4,	12,	33,	56},
    {76,	275,	147,	22,	33,	4,	12,	27,	56},
    {77,	198,	80,	33,	33,	4,	12,	41,	56},
    {78,	80,	118,	28,	33,	4,	12,	35,	56},
    {79,	202,	41,	34,	34,	2,	11,	38,	56},
    {80,	22,	118,	27,	33,	4,	12,	33,	56},
    {81,	112,	0,	34,	43,	2,	11,	38,	56},
    {82,	324,	111,	30,	33,	4,	12,	35,	56},
    {83,	102,	47,	29,	34,	2,	11,	33,	56},
    {84,	140,	118,	28,	33,	1,	12,	30,	56},
    {85,	261,	112,	29,	33,	3,	12,	35,	56},
    {86,	163,	81,	33,	33,	0,	12,	33,	56},
    {87,	233,	77,	47,	33,	0,	12,	46,	56},
    {88,	62,	83,	31,	33,	1,	12,	33,	56},
    {89,	29,	83,	31,	33,	1,	12,	33,	56},
    {90,	200,	115,	28,	33,	1,	12,	30,	56},
    {48,	48,	47,	25,	34,	1,	11,	27,	56},
    {49,	251,	150,	22,	33,	3,	12,	27,	56},
    {50,	282,	75,	23,	34,	2,	11,	27,	56},
    {51,	75,	47,	25,	34,	1,	11,	27,	56},
    {52,	356,	111,	25,	33,	1,	12,	27,	56},
    {53,	350,	146,	24,	33,	2,	12,	27,	56},
    {54,	22,	47,	24,	34,	2,	11,	27,	56},
    {55,	299,	146,	23,	33,	2,	12,	27,	56},
    {56,	358,	74,	24,	34,	2,	11,	27,	56},
    {57,	307,	75,	23,	34,	2,	11,	27,	56},
    {46,	250,	185,	6,	6,	4,	39,	14,	56},
    {44,	9,	180,	6,	12,	4,	39,	14,	56},
    {33,	14,	47,	6,	33,	4,	12,	14,	56},
    {63,	332,	75,	24,	34,	2,	11,	27,	56},
    {45,	258,	185,	13,	4,	2,	30,	16,	56},
    {43,	17,	181,	25,	24,	2,	17,	29,	56},
    {92,	320,	38,	14,	35,	0,	10,	14,	56},
    {47,	304,	38,	14,	35,	0,	10,	14,	56},
    {40,	52,	0,	13,	45,	3,	10,	16,	56},
    {41,	36,	0,	14,	45,	0,	10,	16,	56},
    {58,	215,	179,	6,	26,	4,	19,	14,	56},
    {59,	376,	146,	6,	32,	4,	19,	14,	56},
    {37,	238,	38,	41,	34,	1,	11,	44,	56},
    {38,	95,	83,	31,	33,	1,	12,	33,	56},
    {96,	370,	181,	11,	8,	2,	9,	16,	56},
    {39,	2,	180,	5,	11,	2,	12,	9,	56},
    {42,	349,	181,	19,	17,	0,	12,	19,	56},
    {35,	110,	118,	28,	33,	0,	12,	27,	56},
    {36,	193,	0,	27,	39,	0,	9,	27,	56},
    {61,	297,	182,	25,	16,	2,	21,	29,	56},
    {91,	67,	0,	11,	45,	3,	10,	14,	56},
    {93,	80,	0,	11,	45,	0,	10,	14,	56},
    {64,	148,	0,	43,	42,	3,	10,	50,	56},
    {94,	324,	181,	23,	18,	0,	11,	23,	56},
    {123,	0,	0,	16,	45,	0,	10,	16,	56},
    {125,	18,	0,	16,	45,	0,	10,	16,	56},
    {95,	223,	193,	29,	4,	-1,	51,	27,	56},
    {126,	223,	185,	25,	6,	2,	26,	29,	56},
    {4, 	281,	182,	14,	11,	2,	12,	17,	56},
    {62,	43,	153,	25,	26,	2,	16,	29,	56},
    {60,	70,	153,	25,	26,	2,	16,	29,	56},
    {124,	105,	0,	5,	45,	4,	10,	13,	56},
};

static const char *vertexShaderSource =
"#version 330 core\n\
layout (location = 0) in vec3 aPos;\n\
layout (location = 1) in vec2 aTexCoord;\n\
uniform mat4 gWVP;\n\
out vec2 TexCoord;\n\
void main(){\n\
	gl_Position = gWVP * vec4(aPos, 1.0);\n\
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n\
    }\0";

static const char *fragmentShaderSource =
"#version 330 core\n\
out vec4 FragColor;\n\
in vec2 TexCoord;\n\
uniform sampler2D texture1;\n\
uniform vec3 color;\n\
void main(){FragColor = vec4(color, texture(texture1, TexCoord)[3]);}\0";

unsigned int TextObj::texture = 0;
Shader* TextObj::shader = nullptr;
int TextObj::width = 0;
int TextObj::height = 0;
std::map<char, const Character*> TextObj::charMap;

TextObj::TextObj(float xx, float yy, std::string text) :
    BaseObj(xx, yy)
{
    LoadResources();
    offset_x = 0.0f;
    offset_y = 0.0f;
    color = glm::vec3(0.0f);

    SetText(text);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indices.size(), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);         // position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));         // texture coord attribute
    glEnableVertexAttribArray(1);
    UpdateTransform();
}

void TextObj::UpdateTransform()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = global_projection*model;
}

void TextObj::LoadResources()
{
    if (shader == nullptr)
    {
        shader = new Shader(vertexShaderSource, fragmentShaderSource);
        shader->setInt("texture1", 0);
    }
    if (texture != 0)
        return;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int nrChannels;
    unsigned char *data = stbi_load("img/myfont.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
                format = GL_RED;
        else if (nrChannels == 3)
                format = GL_RGB;
        else if (nrChannels == 4)
                format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
        std::cout << "Texture failed to load" << std::endl;
    stbi_image_free(data);
    for (unsigned i = 0; i < font_lut.size(); i++)
        charMap[font_lut[i].id] = &font_lut[i];
}

TextObj::~TextObj()
{
    glDeleteBuffers(1, &ebo);
}

void TextObj::SetText(std::string text)
{
    for (unsigned i = 0; i < text.length(); i++)
        AddSymbol(text[i]);
}

void TextObj::SetColor(glm::vec3 col)
{
    color = col;
}

void TextObj::Render()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->use();
    shader->setVec3("color", color);
    shader->setMat4("gWVP", transform);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void TextObj::AddSymbol(char symbol)
{
    if (symbol == '\n')
    {
        offset_y -= charMap.begin()->second->origH;
        offset_x = 0.0f;
        return;
    }
    auto char_data = charMap[symbol];
    float x1 = offset_x;
    float x2 = offset_x + char_data->width;
    float y1 = offset_y;
    float y2 = offset_y+char_data->height;
    float u1 = float(char_data->x_pos)/float(width);
    float v1 = float(char_data->y_pos)/float(height);
    float u2 = float(char_data->x_pos+char_data->width+1)/float(width);
    float v2 = float(char_data->y_pos+char_data->height+1)/float(height);
    x1 += char_data->Xoffset;
    x2 += char_data->Xoffset;
    y1 += char_data->origH-char_data->Yoffset-char_data->height;
    y2 += char_data->origH-char_data->Yoffset-char_data->height;
    int ind = vertices.size();
    vertices.push_back(Vertex(x2,y2,u2,v1));
    vertices.push_back(Vertex(x2,y1,u2,v2));
    vertices.push_back(Vertex(x1,y1,u1,v2));
    vertices.push_back(Vertex(x1,y2,u1,v1));
    offset_x += char_data->origW;
    indices.push_back(ind);
    indices.push_back(ind+1);
    indices.push_back(ind+3);
    indices.push_back(ind+1);
    indices.push_back(ind+2);
    indices.push_back(ind+3);
}


