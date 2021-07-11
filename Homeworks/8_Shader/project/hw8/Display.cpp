#include "Display.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>

void Display::create(ContextAttri attr)
{
	glfwInit(); // 初始化GLFW
	// glfwWindowHint函数是用来配置GLFW的，其第一个参数代表选项名称，第二个参数接受一个整型，表示选项的值
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, attr.major); // 主版本号
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, attr.minor); // 次版本号
	if (attr.bProfileCore) {
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 告诉GLFW我们使用的是核心模式(Core-profile)
	}

	// 创建一个窗口对象，其存放了所有和窗口相关的数据，而且会被GLFW的其他函数频繁地用到。
	mWindow = glfwCreateWindow(mDisplayMode.width, mDisplayMode.height, title, nullptr, nullptr);
	if (!mWindow) {
		std::cout << "create window failed." << std::endl;
		return;
	}
	glfwMakeContextCurrent(mWindow); // 通知GLFW将我们窗口的上下文设置为当前线程的主上下文

	if (!gladLoadGL()) {
		std::cout << "glad init Error." << std::endl;
		destroy();
		return;
	}

	glfwSetFramebufferSizeCallback(mWindow, frameBufferSizeCallback);
}

void Display::update()
{
	glfwPollEvents();// 检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）
	glfwSwapBuffers(mWindow); // 交换颜色缓冲
	processEvent();// 捕捉esc
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

// 当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数(Callback Function)，
// 它会在每次窗口大小被调整的时候被调用。
// 这个函数在最上面的create函数中被注册，这样会告诉GLFW我们希望每次调整窗口大小的时候调用这个回调函数
void Display::frameBufferSizeCallback(GLFWwindow* _window, int w, int h)
{
	glViewport(0, 0, w, h); // 设置视口维度，前两个参数控制窗口左下角的位置。后两个参数控制渲染窗口的宽度和高度（像素）
	//视口大小可以小于窗口大小
}

bool Display::isKeyPressed(unsigned int keyCode)
{
	//glfwGetKey需要一个GLFW中的窗口指针以及按键代码作为输入，如果窗口中按键被触发就会返回true
	return glfwGetKey(mWindow, keyCode) == GLFW_PRESS;
}

void Display::processEvent()
{
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) { 
		glfwSetWindowShouldClose(mWindow, true);
	}
}
