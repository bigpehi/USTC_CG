#pragma once

// ��ʾ�Ĵ��ڵĿ��
class DisplayMode
{
public:
	int width;// ���ڿ��
	int height;

	DisplayMode() : width(0), height(0) {}

	DisplayMode(int w, int h) : width(w), height(h) {}
};

// OpenGl�İ汾���Լ�ʹ�õ�ģʽ������ģʽ����������Ⱦģʽ��
class ContextAttri
{
public:
	int major; //���汾��
	int minor; // �ΰ汾��
	bool bProfileCore; //�Ƿ�ʹ�ú���ģʽ

	ContextAttri() : major(0), minor(0), bProfileCore(false) {}

	ContextAttri(int _major, int _minor) : major(_major), minor(_minor), bProfileCore(false) {}

	void withProfileCore(bool bUseProfile) { bProfileCore = bUseProfile; }
};

// �����ڵ����ֻ࣬��������������Ͷ���ֿ���
class Display
{
private:
	DisplayMode mDisplayMode; // ���ڿ����Ϣ
	const char* title; // ����title
	struct GLFWwindow* mWindow; // GLFW�еĴ��ڶ���ָ�룬������GLFW������Ƶ���õ�

public:
	Display() : title(""), mWindow(nullptr) {}

	// ���������¡����ٴ���
	void create(ContextAttri attr);
	void update();
	void destroy();

	bool isRequestClosed(); // �Ƿ񴥷���ĳ���رմ��ڵĲ���

	void setDisplayMode(DisplayMode mode);  // ���ô��ڿ����Ϣ
	void setTitle(const char* _title); // ���ô���title

	static void frameBufferSizeCallback(GLFWwindow*, int, int);

	bool isKeyPressed(unsigned int keyCode); // ���keycode��Ӧ�İ����Ƿ��£�������·���true

private:
	void processEvent(); // ����esc������������������ô���״̬ΪӦ�����ر�
};