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
	programId = createProgram(); // 编译，并将两个shader对象链接到一起
	getAllUniformLocation();
}

void ShaderProgram::start()
{
	glUseProgram(programId); // 激活shader程序对象，在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个shader程序对象了
}

void ShaderProgram::stop()
{
	glUseProgram(0);
}

void ShaderProgram::cleanUp()
{
	glDeleteShader(vertexShaderId); // 删除shader，因为他们已经被编译进程序里了
	glDeleteShader(fragmentShaderId);
	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteProgram(programId);
}

void ShaderProgram::reloadShader()
{
	// 在mian函数中监听R键的触发，判断是否需要重新加载shader
	// 重新加载shader就只需要重新编译链接对应的程序再激活生成的shader程序对象即可
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

	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // 捕获异常

	try {
		// 读文件到流
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

	int shaderId = glCreateShader(type); // 创建shader
	const char* cShaderCode = strShaderCode.c_str(); // shader源码
	glShaderSource(shaderId, 1, &cShaderCode, NULL); // 绑定shader对象和代码，第二参数指定了传递的源码字符串数量，这里只有一个
	glCompileShader(shaderId); // 编译shader

	// 判断shader代码是否编译成功
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
	vertexShaderId = loadShader(vertexFilePath, GL_VERTEX_SHADER); // 根据文件路径加载并编译shader
	fragmentShaderId = loadShader(fragmentFilePath, GL_FRAGMENT_SHADER);
	// 两个着色器现在都编译了，剩下的事情是把两个着色器对象链接到一个用来渲染的着色器程序(Shader Program)中
	int pID = glCreateProgram();
	glAttachShader(pID, vertexShaderId);
	glAttachShader(pID, fragmentShaderId);
	glLinkProgram(pID); // 链接shader程序的结果就是一个整型变量，其指向这个程序对象
	// 判断是否链接成功
	int success;
	glGetProgramiv(pID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(pID, 512, NULL, infoLog);
		std::cout << "link program Error. Info:" << infoLog << std::endl;
	}
	// 删除shader，因为他们已经被编译进程序里就不再需要了
	glDeleteShader(vertexShaderId); 
	glDeleteShader(fragmentShaderId);
	return pID;
}

int ShaderProgram::getUniformLocation(const char* uniformName) {
	return glGetUniformLocation(programId, uniformName); // 获得顶点着色器中定义的uniform类型变量的位置，这样就可以传值给uniform变量了
}

void ShaderProgram::loadFloat(int location, float value) {
	glUniform1f(location, value); // 发送一个float到着色器中的uniform变量，CPU->GPU
}
void ShaderProgram::loadBoolean(int location, bool value) {
	int toValue = value ? 1 : 0;
	glUniform1f(location, toValue);
}
void ShaderProgram::loadVector(int location, struct GLMath::Vector3 vector) {
	glUniform3f(location, vector.x, vector.y, vector.z); // 发送三个float到着色器
}
void ShaderProgram::loadMatrix(int location, class GLMath::Matrix4 matrix) {
	glUniformMatrix4fv(location, 1, GL_FALSE, matrix.get()); // 发送一个矩阵到着色器，GL_FALSE不转置（列矩阵不用转置）
}