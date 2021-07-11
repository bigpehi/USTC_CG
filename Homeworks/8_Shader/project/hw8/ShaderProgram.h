#pragma once
#include "Vector.h"
#include "Matrices.h"

class ShaderProgram {
private:
	int vertexShaderId;
	int fragmentShaderId;
	int programId;

	const char* vertexFilePath;
	const char* fragmentFilePath;

public:
	ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);

	void start();

	void stop();

	void cleanUp();

	void reloadShader();

protected:
	virtual void getAllUniformLocation() {};
	int getUniformLocation(const char* uniformName);

	void loadFloat(int location, float value);
	void loadBoolean(int location, bool value);
	void loadVector(int location, struct GLMath::Vector3 vector);
	void loadMatrix(int location, class GLMath::Matrix4 matrix);
private:
	int loadShader(const char* filePath, int type);

	int createProgram();
};