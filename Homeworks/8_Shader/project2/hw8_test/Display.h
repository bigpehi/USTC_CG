#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Display {
public:
	Display();

	void set_glVersion(int major,int minor); // ����opengl�汾
	void set_windowSize(int height, int width); // ���ô��ڿ��
	void set_windowTitle(char* _title); // ���ô���title
	bool isKeyPressed(unsigned int keyCode); // ���keycode��Ӧ�İ����Ƿ��£�������·���true
	void processInput();
	bool isRequestClosed(unsigned int keyCode); // �Ƿ񴥷���ĳ���رմ��ڵĲ���
	// �ص�������
	static void frameBufferSizeCallback(GLFWwindow*, int w, int h);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


	// ���������¡����ٴ���
	void create();
	void update();
	void destroy();

	struct GLFWwindow* mWindow; // GLFW�еĴ��ڶ���ָ�룬������GLFW������Ƶ���õ�

	int get_width();
	int get_height();

	// camera
	static Camera camera;
	static float lastX;
	static float lastY;
	static bool firstMouse;

	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

private:
	int v_major; // ���汾��
	int v_minor; // �ΰ汾��

	int w_width; // ���ڿ�
	int w_height; // ���ڸ�



	const char* title; // ����title




};