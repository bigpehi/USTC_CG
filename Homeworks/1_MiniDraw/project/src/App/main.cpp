#include "minidraw.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

	QApplication a(argc, argv); // aӦ�ó��������QT�У�Ӧ�ó���������ҽ���һ��	
	MiniDraw w; //���ڶ���myWidget����->QWidget
	w.show(); // ���ڶ���Ĭ�ϲ�����ʾ���ڣ��������show��������ʾ
	return a.exec(); //��Ӧ�ó��������뵽��Ϣѭ�����������������У����Ǵ����˳����ƣ�������
}
