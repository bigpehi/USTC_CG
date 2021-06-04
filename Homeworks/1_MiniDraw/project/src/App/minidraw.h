#pragma once

#include <ui_minidraw.h>
#include <viewwidget.h>

#include <QtWidgets/QMainWindow>
#include <qmessagebox.h> 

class MiniDraw : public QMainWindow {
	Q_OBJECT // Q_OBJECT宏，允许类中使用信号和槽的机制

public:
	MiniDraw(QWidget* parent = 0); // 构造函数
	~MiniDraw(); // 析构函数

	QMenu* pMenu; //声明菜单栏
	QToolBar* pToolBar; // 声明工具栏
	QAction* Action_About; // 声明动作
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
