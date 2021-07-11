#include "DisplayManager.h"

void DisplayManager::createDisplay()
{
	mDisplay.setTitle("first window");
	mDisplay.setDisplayMode(DisplayMode(640, 480));
	ContextAttri attr(3, 3);
	mDisplay.create(attr);
}

void DisplayManager::updateDisplay()
{
	mDisplay.update();
}

void DisplayManager::closeDisplay()
{
	mDisplay.destroy();
}

bool DisplayManager::isRequestClosed()
{
	return mDisplay.isRequestClosed();
}

bool DisplayManager::isKeyPressed(unsigned int keyCode)
{
	return mDisplay.isKeyPressed(keyCode);
}
