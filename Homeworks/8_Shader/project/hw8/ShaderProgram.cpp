#include "ShaderProgram.h"
#include "glad/glad.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

ShaderProgram::ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath)
{
	vertexFilePath = vertexShaderPath;
	fragmentFilePath = fragmentShaderPath;
	programId = createProgram(); // ���룬��������shader�������ӵ�һ��
	getAllUniformLocation();
}

void ShaderProgram::start()
{
	glUseProgram(programId); // ����shader���������glUseProgram��������֮��ÿ����ɫ�����ú���Ⱦ���ö���ʹ�����shader���������
}

void ShaderProgram::stop()
{
	glUseProgram(0);
}

void ShaderProgram::cleanUp()
{
	glDeleteShader(vertexShaderId); // ɾ��shader����Ϊ�����Ѿ����������������
	glDeleteShader(fragmentShaderId);
	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteProgram(programId);
}

void ShaderProgram::reloadShader()
{
	// ��mian�����м���R���Ĵ������ж��Ƿ���Ҫ���¼���shader
	// ���¼���shader��ֻ��Ҫ���±������Ӷ�Ӧ�ĳ����ټ������ɵ�shader������󼴿�
	int reload_program_id = createProgram();
	if (reload_program_id) {
		glDeleteProgram(programId);
		programId = reload_program_id;
	}
}

int ShaderProgram::loadShader(const char* filePath, int type)
{
	std::string strShaderCode;
	std::ifstream shaderFile;

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // �����쳣

	try {
		// ���ļ�����
		std::stringstream shaderStream;
		shaderFile.open(filePath);
		shaderStream << shaderFile.rdbuf();

		shaderFile.close();
		strShaderCode = shaderStream.str();
	}
	catch (std::exception e) {
		std::cout << "load shader error." << std::endl;
		return -1;
	}

	int shaderId = glCreateShader(type); // ����shader
	const char* cShaderCode = strShaderCode.c_str(); // shaderԴ��
	glShaderSource(shaderId, 1, &cShaderCode, NULL); // ��shader����ʹ��룬�ڶ�����ָ���˴��ݵ�Դ���ַ�������������ֻ��һ��
	glCompileShader(shaderId); // ����shader

	// �ж�shader�����Ƿ����ɹ�
	int success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
		std::cout << "compile shader Error. info:" << infoLog << std::endl;
	}

	return shaderId;
}

int ShaderProgram::createProgram()
{
	vertexShaderId = loadShader(vertexFilePath, GL_VERTEX_SHADER); // �����ļ�·�����ز�����shader
	fragmentShaderId = loadShader(fragmentFilePath, GL_FRAGMENT_SHADER);
	// ������ɫ�����ڶ������ˣ�ʣ�µ������ǰ�������ɫ���������ӵ�һ��������Ⱦ����ɫ������(Shader Program)��
	int pID = glCreateProgram();
	glAttachShader(pID, vertexShaderId);
	glAttachShader(pID, fragmentShaderId);
	glLinkProgram(pID); // ����shader����Ľ������һ�����ͱ�������ָ������������
	// �ж��Ƿ����ӳɹ�
	int success;
	glGetProgramiv(pID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(pID, 512, NULL, infoLog);
		std::cout << "link program Error. Info:" << infoLog << std::endl;
	}
	// ɾ��shader����Ϊ�����Ѿ��������������Ͳ�����Ҫ��
	glDeleteShader(vertexShaderId); 
	glDeleteShader(fragmentShaderId);
	return pID;
}

int ShaderProgram::getUniformLocation(const char* uniformName) {
	return glGetUniformLocation(programId, uniformName); // ��ö�����ɫ���ж����uniform���ͱ�����λ�ã������Ϳ��Դ�ֵ��uniform������
}

void ShaderProgram::loadFloat(int location, float value) {
	glUniform1f(location, value); // ����һ��float����ɫ���е�uniform������CPU->GPU
}
void ShaderProgram::loadBoolean(int location, bool value) {
	int toValue = value ? 1 : 0;
	glUniform1f(location, toValue);
}
void ShaderProgram::loadVector(int location, struct GLMath::Vector3 vector) {
	glUniform3f(location, vector.x, vector.y, vector.z); // ��������float����ɫ��
}
void ShaderProgram::loadMatrix(int location, class GLMath::Matrix4 matrix) {
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix.get()); // ����һ��������ɫ����GL_FALSE��ת�ã��о�����ת�ã�
}