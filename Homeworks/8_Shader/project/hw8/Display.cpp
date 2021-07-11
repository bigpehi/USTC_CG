#include "Display.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

void Display::create(ContextAttri attr)
{
	glfwInit(); // ��ʼ��GLFW
	// glfwWindowHint��������������GLFW�ģ����һ����������ѡ�����ƣ��ڶ�����������һ�����ͣ���ʾѡ���ֵ
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, attr.major); // ���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, attr.minor); // �ΰ汾��
	if (attr.bProfileCore) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // ����GLFW����ʹ�õ��Ǻ���ģʽ(Core-profile)
	}

	// ����һ�����ڶ������������кʹ�����ص����ݣ����һᱻGLFW����������Ƶ�����õ���
	mWindow = glfwCreateWindow(mDisplayMode.width, mDisplayMode.height, title, nullptr, nullptr);
	if (!mWindow) {
		std::cout << "create window failed." << std::endl;
		return;
	}
	glfwMakeContextCurrent(mWindow); // ֪ͨGLFW�����Ǵ��ڵ�����������Ϊ��ǰ�̵߳���������

	if (!gladLoadGL()) {
		std::cout << "glad init Error." << std::endl;
		destroy();
		return;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);
}

void Display::update()
{
	glfwPollEvents();// �����û�д���ʲô�¼�������������롢����ƶ��ȣ������´���״̬�������ö�Ӧ�Ļص�����������ͨ���ص������ֶ����ã�
	glfwSwapBuffers(mWindow); // ������ɫ����
	processEvent();// ��׽esc
}

void Display::destroy()
{
	glfwDestroyWindow(mWindow);
	glfwTerminate();
	mWindow = nullptr;
}

bool Display::isRequestClosed()
{
	return glfwWindowShouldClose(mWindow);
}

void Display::setDisplayMode(DisplayMode mode)
{
	mDisplayMode = mode;
}

void Display::setTitle(const char* _title)
{
	title = _title;
}

// ���û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ����������ǿ��ԶԴ���ע��һ���ص�����(Callback Function)��
// ������ÿ�δ��ڴ�С��������ʱ�򱻵��á�
// ����������������create�����б�ע�ᣬ���������GLFW����ϣ��ÿ�ε������ڴ�С��ʱ���������ص�����
void Display::frameBufferSizeCallback(GLFWwindow* _window, int w, int h)
{
	glViewport(0, 0, w, h); // �����ӿ�ά�ȣ�ǰ�����������ƴ������½ǵ�λ�á�����������������Ⱦ���ڵĿ�Ⱥ͸߶ȣ����أ�
	//�ӿڴ�С����С�ڴ��ڴ�С
}

bool Display::isKeyPressed(unsigned int keyCode)
{
	//glfwGetKey��Ҫһ��GLFW�еĴ���ָ���Լ�����������Ϊ���룬��������а����������ͻ᷵��true
	return glfwGetKey(mWindow, keyCode) == GLFW_PRESS;
}

void Display::processEvent()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) { 
		glfwSetWindowShouldClose(mWindow, true);
	}
}
