#include "minidraw.h"
#include <QToolButton> 
#include <QToolBar>
#include <QPushButton>

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


	Action_Undo = new QAction(tr("&Undo"), this);
	connect(Action_Undo, SIGNAL(triggered()), view_widget_, SLOT(undo()));

	Action_Color = new QAction(tr("&Color"), this);
	connect(Action_Color, SIGNAL(triggered()), view_widget_, SLOT(setColor()));

	Action_Pen_Width = new QAction(tr("&Width"), this);
	connect(Action_Pen_Width, &QAction::triggered, this, &MiniDraw::SetWidthComboBox);

	//Action_Pen_Style= new QAction(tr("&Style"), this);
	//connect(Action_Pen_Style, SIGNAL(triggered()), view_widget_, SLOT(setPenStyle()));
}

void MiniDraw::Creat_ToolBar() {
	pToolBar = addToolBar(tr("&Main")); // 创建工具栏，标签为 Main
	pToolBar->addAction(Action_About);  // 将动作加载到 Main 菜单中
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
	pToolBar->addAction(Action_Brush);
	pToolBar->addAction(Action_Undo);
	pToolBar->addAction(Action_Color);
	pToolBar->addAction(Action_Pen_Width);
	QIcon icon_Line(":/line.png");
	Action_Line->setIcon(icon_Line);
	QIcon icon_Rect(":/rect.png");
	Action_Rect->setIcon(icon_Rect);
	QIcon icon_Ellipse(":/ellipse.png");
	Action_Ellipse->setIcon(icon_Ellipse);
	QIcon icon_Brush(":/brush.png");
	Action_Brush->setIcon(icon_Brush);
	QIcon icon_Undo(":/undo.png");
	Action_Undo->setIcon(icon_Undo);
	QIcon icon_Color(":/Color.png");
	Action_Color->setIcon(icon_Color);	
	QIcon icon_Pen_Width(":/width.png");
	Action_Pen_Width->setIcon(icon_Pen_Width);
}

void MiniDraw::Creat_Menu() {
	pMenu = menuBar()->addMenu(tr("&Figure Tool")); // 创建菜单，标签为Figure Tool
	pMenu->addAction(Action_About); // 将动作加载到 Main 菜单中
	pMenu->addAction(Action_Line);
	pMenu->addAction(Action_Rect);
	pMenu->addAction(Action_Ellipse);
	pMenu->addAction(Action_Undo);
	pMenu->addAction(Action_Color);
	pMenu->addAction(Action_Pen_Width);
}

void MiniDraw::AboutBox() {
	QMessageBox::about(this, tr("About"), tr("Designed by Chenbin Li"));
}

int MiniDraw::SetWidthComboBox() {
	QToolButton* toolBtn = new QToolButton(this);
	toolBtn->setText("Width");
	QMenu* menu = new QMenu(this);
	QAction* Action_width_1 = new QAction(QString::asprintf("%d", 1), this);
	QAction* Action_width_2 = new QAction(QString::asprintf("%d", 2), this);
	QAction* Action_width_3 = new QAction(QString::asprintf("%d", 3), this);
	QAction* Action_width_4 = new QAction(QString::asprintf("%d", 4), this);
	QAction *Action_width_5 = new QAction(QString::asprintf("%d", 5), this);
	connect(Action_width_1, SIGNAL(triggered()), view_widget_, SLOT(setPenWidth1()));
	connect(Action_width_2, SIGNAL(triggered()), view_widget_, SLOT(setPenWidth2()));
	connect(Action_width_3, SIGNAL(triggered()), view_widget_, SLOT(setPenWidth3()));
	connect(Action_width_4, SIGNAL(triggered()), view_widget_, SLOT(setPenWidth4()));
	connect(Action_width_5, SIGNAL(triggered()), view_widget_, SLOT(setPenWidth5()));
	menu->addAction(Action_width_1); // 无图标
	menu->addAction(Action_width_2); // 无图标
	menu->addAction(Action_width_3); // 无图标
	menu->addAction(Action_width_4); // 无图标
	menu->addAction(Action_width_5); // 无图标
	
	toolBtn->setMenu(menu);
	toolBtn->setPopupMode(QToolButton::MenuButtonPopup);

	pToolBar->addWidget(toolBtn);
	return 0;
}


MiniDraw::~MiniDraw() {}


void MiniDraw::Init() {
	//...
	view_widget_ = new ViewWidget(); // 实例化 ViewWidget 控件窗口
	//...
	setCentralWidget(view_widget_); // 将 ViewWidget 控件设置为主窗口的中心位置
	//...
}
