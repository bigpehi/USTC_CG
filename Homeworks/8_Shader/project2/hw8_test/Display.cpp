#include "Display.h"
#include <iostream>

float Display::lastX = 0;
float Display::lastY = 0;
bool Display::firstMouse = true;
Camera Display::camera;

Display::Display()
{
	title = "Default Window";
	v_major = 3;
	v_minor = 3;
	w_width = 800;
	w_height = 600;

	lastX = w_width / 2.0f;
	lastY = w_height / 2.0f;
	firstMouse = true;

}

void Display::set_glVersion(int major, int minor)
{
	v_major = major;
	v_minor = minor;
}

void Display::set_windowSize(int height, int width)
{
	w_height = height;
	w_width = width;
}

void Display::set_windowTitle(char* _title)
{
	if (_title)
	{
		title = _title;
	}
}


void Display::frameBufferSizeCallback(GLFWwindow*, int w, int h)
{
	glViewport(0, 0, w, h); // 设置视口维度，前两个参数控制窗口左下角的位置。后两个参数控制渲染窗口的宽度和高度（像素）
}

void Display::mouse_callback(GLFWwindow*, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void Display::scroll_callback(GLFWwindow*, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

bool Display::isKeyPressed(unsigned int keyCode)
{
	//glfwGetKey需要一个GLFW中的窗口指针以及按键代码作为输入，如果窗口中按键被触发就会返回true
	return glfwGetKey(mWindow, keyCode) == GLFW_PRESS;
}

void Display::processInput()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mWindow, true);

	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

bool Display::isRequestClosed(unsigned int keyCode)
{
	return glfwWindowShouldClose(mWindow);
}

void Display::create()
{
	glfwInit(); // 初始化GLFW
	// glfwWindowHint函数是用来配置GLFW的，其第一个参数代表选项名称，第二个参数接受一个整型，表示选项的值
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, v_major); // 主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, v_minor); // 次版本号
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 告诉GLFW我们使用的是核心模式(Core-profile)
	// 创建一个窗口对象，其存放了所有和窗口相关的数据，而且会被GLFW的其他函数频繁地用到。
	mWindow = glfwCreateWindow(w_width, w_height, title, nullptr, nullptr);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(mWindow); // 通知GLFW将我们窗口的上下文设置为当前线程的主上下文

	if (!gladLoadGL()) {
		std::cout << "glad init Error." << std::endl;
		destroy();
		return;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback); // 设置关于窗口大小的回调函数
	glfwSetCursorPosCallback(mWindow, mouse_callback); // 相机随鼠标移动
	glfwSetScrollCallback(mWindow, scroll_callback); // 相机随滚轮变焦

	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// tell GLFW to capture our mouse
}

void Display::update()
{
	glfwPollEvents();// 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
	glfwSwapBuffers(mWindow); // 交换颜色缓冲
	processInput();// 捕捉按键
}

void Display::destroy()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
	mWindow = nullptr;
}

int Display::get_width()
{
	return w_width;
}

int Display::get_height()
{
	return w_height;
}



