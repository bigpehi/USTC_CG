#pragma once
#include "ShaderProgram.h"
//#include "Matrices.h"

const char* VERTEX_SHADER_FILEPATH = "../Shader/vertexShader.vs";
const char* FRAGMENT_SHADER_FILEPATH = "../Shader/fragmentShader.fs";

class StaticShader : public ShaderProgram {
public:
    StaticShader() : ShaderProgram(VERTEX_SHADER_FILEPATH, FRAGMENT_SHADER_FILEPATH) {}

    void loadTransformMatrix(GLMath::Matrix4 matrix) {
        loadMatrix(location_transformMatrix, matrix);
    }

private:
    int location_transformMatrix;

protected:
    virtual void getAllUniformLocation() override {
        location_transformMatrix = getUniformLocation("transformMatrix");
    }
};