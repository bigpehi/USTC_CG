#pragma once
#include "Display.h"

// �������������DIsplay���ʹ�ã�˼�������ڹ���ģʽ
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