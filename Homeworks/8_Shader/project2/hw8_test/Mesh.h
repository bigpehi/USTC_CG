#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader_m.h"

#include <string>
#include <vector>
using namespace std;

// 一个顶点
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // TBN用于法线贴图中的切线空间，暂时用不上
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

// 一个纹理数据
struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // 网格组成：顶点（位置，法向量，纹理顶点），索引，纹理（类型，地址）
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // 构造函数初始化vertices，indices，textures，并调用setupMesh函数初始化缓冲，setupMesh调用draw函数绘制网格
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    // 绑定不同的纹理采样器到着色器中的不同uniform变量，绑定纹理和VAO并绘图
    void Draw(Shader& shader);


private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
    
};
#endif