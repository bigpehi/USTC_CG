#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader_m.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Cube {

public:
	Cube(glm::vec3 _position, glm::vec3 _size);
	Cube(float x = 0., float y = 0., float z = 0., float l = 1., float w = 1., float h = 1.);
	void genVerteces();
	void Draw();
	glm::vec3 getPosition();

private:
	glm::vec3 position;
	glm::vec3 size;

	unsigned int VBO, cubeVAO;

	//Shader cubeShader;
};