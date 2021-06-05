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
	QAction* Action_Brush;
	QAction* Action_Undo;
	QAction* Action_Color;
	QAction* Action_Pen_Width;
	QAction* Action_Pen_Style;


	void Creat_Menu();
	void Creat_ToolBar();
	void Creat_Action();
	void Init();

	void AboutBox(); //message box
	int SetWidthComboBox();

private:
	Ui::MiniDrawClass ui;
	ViewWidget* view_widget_;
};
