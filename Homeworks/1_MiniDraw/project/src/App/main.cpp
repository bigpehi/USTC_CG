#include "minidraw.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	QApplication a(argc, argv); // a应用程序对象，在QT中，应用程序对象有且仅有一个	
	MiniDraw w; //窗口对象，myWidget父类->QWidget
	w.show(); // 窗口对象，默认不会显示窗口，必须调用show方法才显示
	return a.exec(); //让应用程序对象进入到消息循环，代码阻塞到这行，除非触发退出机制，比如点叉
}
