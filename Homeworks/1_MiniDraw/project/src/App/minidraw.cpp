#include "minidraw.h"

#include <QToolBar>

MiniDraw::MiniDraw(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	view_widget_ = new ViewWidget();
	Creat_Action();
	Creat_ToolBar();
	Creat_Menu();

	setCentralWidget(view_widget_);
}

void MiniDraw::Creat_Action() { // 菜单栏，工具栏中的动作（按钮，选项），建立相关信号槽
	Action_About = new QAction(tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);//& 的作用：设置此动作的快捷键为标签字符串的首字母，此例为 'M'
	// connect(信号发射方，信号，信号接收方，槽函数);
	Action_Line = new QAction(tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(tr("&Rect"), this);
	connect(Action_Rect, SIGNAL(triggered()), view_widget_, SLOT(setRect()));

	Action_Ellipse = new QAction(tr("&Ellipse"), this);
	connect(Action_Ellipse, SIGNAL(triggered()), view_widget_, SLOT(setEllipse()));

	Action_Brush = new QAction(tr("&Brush"), this);
	connect(Action_Brush, SIGNAL(triggered()), view_widget_, SLOT(setBrush()));
}

void MiniDraw::Creat_ToolBar() {
	pToolBar = addToolBar(tr("&Main")); // 创建工具栏，标签为 Main
	pToolBar->addAction(Action_About);  // 将动作加载到 Main 菜单中
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
	pToolBar->addAction(Action_Brush);
	QIcon icon_Line(":/line.png");
	Action_Line->setIcon(icon_Line);
	QIcon icon_Rect(":/rect.png");
	Action_Rect->setIcon(icon_Rect);
	QIcon icon_Ellipse(":/ellipse.png");
	Action_Ellipse->setIcon(icon_Ellipse);
	QIcon icon_Brush(":/brush.png");
	Action_Brush->setIcon(icon_Brush);
}

void MiniDraw::Creat_Menu() {
	pMenu = menuBar()->addMenu(tr("&Figure Tool")); // 创建菜单，标签为Figure Tool
	pMenu->addAction(Action_About); // 将动作加载到 Main 菜单中
	pMenu->addAction(Action_Line);
	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
}

void MiniDraw::AboutBox() {
	QMessageBox::about(this, tr("About"), tr("MiniDraw"));
}

MiniDraw::~MiniDraw() {}


void MiniDraw::Init() {
	//...
	view_widget_ = new ViewWidget(); // 实例化 ViewWidget 控件窗口
	//...
	setCentralWidget(view_widget_); // 将 ViewWidget 控件设置为主窗口的中心位置
	//...
}
