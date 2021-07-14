#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader_m.h"

#include <string>
#include <vector>
using namespace std;

// һ������
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    // TBN���ڷ�����ͼ�е����߿ռ䣬��ʱ�ò���
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

// һ����������
struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // ������ɣ����㣨λ�ã��������������㣩���������������ͣ���ַ��
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // ���캯����ʼ��vertices��indices��textures��������setupMesh������ʼ�����壬setupMesh����draw������������
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

    // �󶨲�ͬ���������������ɫ���еĲ�ͬuniform�������������VAO����ͼ
    void Draw(Shader& shader);


private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
    
};
#endif