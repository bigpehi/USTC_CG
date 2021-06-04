#pragma once

#include <ui_minidraw.h>
#include <viewwidget.h>

#include <QtWidgets/QMainWindow>
#include <qmessagebox.h> 

class MiniDraw : public QMainWindow {
	Q_OBJECT // Q_OBJECT�꣬��������ʹ���źźͲ۵Ļ���

public:
	MiniDraw(QWidget* parent = 0); // ���캯��
	~MiniDraw(); // ��������

	QMenu* pMenu; //�����˵���
	QToolBar* pToolBar; // ����������
	QAction* Action_About; // ��������
	QAction* Action_Line;
	QAction* Action_Rect;
	QAction* Action_Ellipse;

	void Creat_Menu();
	void Creat_ToolBar();
	void Creat_Action();
	void Init();

	void AboutBox(); //message box

private:
	Ui::MiniDrawClass ui;
	ViewWidget* view_widget_;
};
