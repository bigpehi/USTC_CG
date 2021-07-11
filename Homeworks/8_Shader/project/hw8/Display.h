#pragma once

// 显示的窗口的宽高
class DisplayMode
{
public:
	int width;// 窗口宽高
	int height;

	DisplayMode() : width(0), height(0) {}

	DisplayMode(int w, int h) : width(w), height(h) {}
};

// OpenGl的版本号以及使用的模式（核心模式还是立即渲染模式）
class ContextAttri
{
public:
	int major; //主版本号
	int minor; // 次版本号
	bool bProfileCore; //是否使用核心模式

	ContextAttri() : major(0), minor(0), bProfileCore(false) {}

	ContextAttri(int _major, int _minor) : major(_major), minor(_minor), bProfileCore(false) {}

	void withProfileCore(bool bUseProfile) { bProfileCore = bUseProfile; }
};

// 管理窗口的总类，只有这个类是声明和定义分开的
class Display
{
private:
	DisplayMode mDisplayMode; // 窗口宽高信息
	const char* title; // 窗口title
	struct GLFWwindow* mWindow; // GLFW中的窗口对象指针，其他的GLFW函数会频繁用到

public:
	Display() : title(""), mWindow(nullptr) {}

	// 创建、更新、销毁窗口
	void create(ContextAttri attr);
	void update();
	void destroy();

	bool isRequestClosed(); // 是否触发了某个关闭窗口的操作

	void setDisplayMode(DisplayMode mode);  // 设置窗口宽高信息
	void setTitle(const char* _title); // 设置窗口title

	static void frameBufferSizeCallback(GLFWwindow*, int, int);

	bool isKeyPressed(unsigned int keyCode); // 检测keycode对应的按键是否按下，如果按下返回true

private:
	void processEvent(); // 捕获esc按键，如果触发则设置窗口状态为应当被关闭
};