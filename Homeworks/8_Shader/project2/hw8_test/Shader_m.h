#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    unsigned int ID;
    
    // ����һ��shader����ʱ��Ҫָ��������ɫ����Ƭ����ɫ����λ��
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    
    // ����shader�������
    void use() { 
        glUseProgram(ID);
    }

    // ����һ��bool���͵���ɫ���е�uniformֵ
    void setBool(const std::string& name, bool value) const;

    // ����һ��int���͵���ɫ���е�uniformֵ
    void setInt(const std::string& name, int value) const;


    // ����һ��float���͵���ɫ���е�uniformֵ
    void setFloat(const std::string& name, float value) const;

    // ����һ��bool���͵���ɫ���е�uniformֵ
    void setVec2(const std::string& name, const glm::vec2& value) const;

    // ����һ��vec2(����float)���͵���ɫ���е�uniformֵ
    void setVec2(const std::string& name, float x, float y) const;

    // ����һ��vec3��һ��float���飩���͵���ɫ���е�uniformֵ
    void setVec3(const std::string& name, const glm::vec3& value) const;

    // ����һ��vec3(����float)���͵���ɫ���е�uniformֵ
    void setVec3(const std::string& name, float x, float y, float z) const;

    // ����һ��vec4(һ��float����)���͵���ɫ���е�uniformֵ
    void setVec4(const std::string& name, const glm::vec4& value) const;

    // ����һ��vec4(�ĸ�float)���͵���ɫ���е�uniformֵ
    void setVec4(const std::string& name, float x, float y, float z, float w);

    // ����һ����ά�������ɫ���е�uniformֵ
    void setMat2(const std::string& name, const glm::mat2& mat) const;

    // ����һ����ά�������ɫ���е�uniformֵ
    void setMat3(const std::string& name, const glm::mat3& mat) const;

    // ����һ����ά�������ɫ���е�uniformֵ
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // �����ɫ���Ƿ����ɹ�������ڹ��캯������������ɫ����������
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif
