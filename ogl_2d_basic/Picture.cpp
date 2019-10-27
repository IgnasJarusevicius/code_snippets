#include "Picture.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>

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
void main(){FragColor = texture(texture1, TexCoord);}\0";

unsigned int Picture::texture = 0;
Shader* Picture::shader = nullptr;
int Picture::width = 0;
int Picture::height = 0;

Picture::Picture(float xx, float yy) :
    BaseObj(xx, yy)
{
    LoadResources();;

    vertices.push_back(Vertex(width,height,1.0f,0.0f));
    vertices.push_back(Vertex(width,0.0f,1.0f,1.0f));
    vertices.push_back(Vertex(0.0f,0.0f,0.0f,1.0f));
    vertices.push_back(Vertex(0.0f,height,0.0f,0.0f));

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

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

void Picture::UpdateTransform()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = global_projection*model;
}

void Picture::LoadResources()
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
    int nrChannels;
    std::vector<uint32_t> data;
    unsigned char *data2 = stbi_load("img/attack e0000.bmp", &width, &height, &nrChannels, 4);
    if (data2)
    {
        int part_size = width*height;
        data.resize(part_size, 0);
        uint32_t* pIntData2 = ((uint32_t*)data2);
        uint32_t back_color = *pIntData2;
        for (int i = 0; i < part_size; pIntData2++)
            data[i++] = *pIntData2 == back_color ? 0 : *pIntData2;
    }
    else
        std::cout << "Texture failed to load" << std::endl;
    stbi_image_free(data2);

    GLenum format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Picture::~Picture()
{
    glDeleteBuffers(1, &ebo);
}

void Picture::Render()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    shader->use();
    shader->setMat4("gWVP", transform);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
