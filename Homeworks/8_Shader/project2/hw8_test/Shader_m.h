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
    
    // 构造一个shader对象时需要指定顶点着色器和片段着色器的位置
    Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
    
    // 激活shader程序对象
    void use() { 
        glUseProgram(ID);
    }

    // 设置一个bool类型的着色器中的uniform值
    void setBool(const std::string& name, bool value) const;

    // 设置一个int类型的着色器中的uniform值
    void setInt(const std::string& name, int value) const;


    // 设置一个float类型的着色器中的uniform值
    void setFloat(const std::string& name, float value) const;

    // 设置一个bool类型的着色器中的uniform值
    void setVec2(const std::string& name, const glm::vec2& value) const;

    // 设置一个vec2(两个float)类型的着色器中的uniform值
    void setVec2(const std::string& name, float x, float y) const;

    // 设置一个vec3（一个float数组）类型的着色器中的uniform值
    void setVec3(const std::string& name, const glm::vec3& value) const;

    // 设置一个vec3(三个float)类型的着色器中的uniform值
    void setVec3(const std::string& name, float x, float y, float z) const;

    // 设置一个vec4(一个float数组)类型的着色器中的uniform值
    void setVec4(const std::string& name, const glm::vec4& value) const;

    // 设置一个vec4(四个float)类型的着色器中的uniform值
    void setVec4(const std::string& name, float x, float y, float z, float w);

    // 设置一个二维矩阵的着色器中的uniform值
    void setMat2(const std::string& name, const glm::mat2& mat) const;

    // 设置一个三维矩阵的着色器中的uniform值
    void setMat3(const std::string& name, const glm::mat3& mat) const;

    // 设置一个四维矩阵的着色器中的uniform值
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    // 检查着色器是否编译成功，其会在构造函数编译链接着色器代码后调用
    void checkCompileErrors(GLuint shader, std::string type);
};
#endif
