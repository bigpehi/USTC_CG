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

	void set_glVersion(int major,int minor); // 设置opengl版本
	void set_windowSize(int height, int width); // 设置窗口宽高
	void set_windowTitle(char* _title); // 设置窗口title
	bool isKeyPressed(unsigned int keyCode); // 检测keycode对应的按键是否按下，如果按下返回true
	void processInput();
	bool isRequestClosed(unsigned int keyCode); // 是否触发了某个关闭窗口的操作
	// 回调函数们
	static void frameBufferSizeCallback(GLFWwindow*, int w, int h);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


	// 创建、更新、销毁窗口
	void create();
	void update();
	void destroy();

	struct GLFWwindow* mWindow; // GLFW中的窗口对象指针，其他的GLFW函数会频繁用到

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
	int v_major; // 主版本号
	int v_minor; // 次版本号

	int w_width; // 窗口宽
	int w_height; // 窗口高



	const char* title; // 窗口title




};