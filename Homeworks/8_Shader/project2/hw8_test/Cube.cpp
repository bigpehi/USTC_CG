#include "Cube.h"

Cube::Cube(glm::vec3 _position, glm::vec3 _size)
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    size = glm::vec3(1.0f, 1.0f, 1.0f);
	position = _position;
	size = _size;

    genVerteces();
}

Cube::Cube(float x, float y, float z, float l, float w, float h)
{
	position = glm::vec3(x, y, z);
	size = glm::vec3(l, w, h);

    genVerteces();
}

void Cube::genVerteces()
{
    float vertices[] = {
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,

    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
    };
    for (size_t i = 0; i < 36*6; i++)
    {
        for (size_t j = 0; j < 3; j++)
        {
            if (i % 6 == j) {
                vertices[i] *= size[j];
                vertices[i] += position[j];
            }
        }

    }
    

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // cube的法线属性，光源不需要，但是普通cube作为受光物体是需要的
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Cube::Draw()
{
    glBindVertexArray(cubeVAO); // 绑定VAO
    glDrawArrays(GL_TRIANGLES, 0, 36); // 绘图
    glBindVertexArray(0); // 解绑VAO

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

glm::vec3 Cube::getPosition()
{
    return position;
}


