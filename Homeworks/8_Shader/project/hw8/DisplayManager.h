#pragma once
#include "Display.h"

// 这个类用来管理DIsplay类的使用，思想类似于工厂模式
class DisplayManager
{
private:
	Display mDisplay;

public:
	DisplayManager() {}

	void createDisplay();
	void updateDisplay();
	void closeDisplay();

	bool isRequestClosed();

	bool isKeyPressed(unsigned int keyCode);
};