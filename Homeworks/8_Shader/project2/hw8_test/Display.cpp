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
	glViewport(0, 0, w, h); // �����ӿ�ά�ȣ�ǰ�����������ƴ������½ǵ�λ�á�����������������Ⱦ���ڵĿ�Ⱥ͸߶ȣ����أ�
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
	//glfwGetKey��Ҫһ��GLFW�еĴ���ָ���Լ�����������Ϊ���룬��������а����������ͻ᷵��true
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
	glfwInit(); // ��ʼ��GLFW
	// glfwWindowHint��������������GLFW�ģ����һ����������ѡ�����ƣ��ڶ�����������һ�����ͣ���ʾѡ���ֵ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, v_major); // ���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, v_minor); // �ΰ汾��
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
	// ����һ�����ڶ������������кʹ�����ص����ݣ����һᱻGLFW����������Ƶ�����õ���
	mWindow = glfwCreateWindow(w_width, w_height, title, nullptr, nullptr);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(mWindow); // ֪ͨGLFW�����Ǵ��ڵ�����������Ϊ��ǰ�̵߳���������

	if (!gladLoadGL()) {
		std::cout << "glad init Error." << std::endl;
		destroy();
		return;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback); // ���ù��ڴ��ڴ�С�Ļص�����
	glfwSetCursorPosCallback(mWindow, mouse_callback); // ���������ƶ�
	glfwSetScrollCallback(mWindow, scroll_callback); // �������ֱ佹

	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// tell GLFW to capture our mouse
}

void Display::update()
{
	glfwPollEvents();// �����û�д���ʲô�¼�������������롢����ƶ��ȣ������´���״̬�������ö�Ӧ�Ļص�����������ͨ���ص������ֶ����ã�
	glfwSwapBuffers(mWindow); // ������ɫ����
	processInput();// ��׽����
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



