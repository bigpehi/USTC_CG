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

void MiniDraw::Creat_Action() { // �˵������������еĶ�������ť��ѡ�����������źŲ�
	Action_About = new QAction(tr("&About"), this);
	connect(Action_About, &QAction::triggered, this, &MiniDraw::AboutBox);//& �����ã����ô˶����Ŀ�ݼ�Ϊ��ǩ�ַ���������ĸ������Ϊ 'M'
	// connect(�źŷ��䷽���źţ��źŽ��շ����ۺ���);
	Action_Line = new QAction(tr("&Line"), this);
	connect(Action_Line, SIGNAL(triggered()), view_widget_, SLOT(setLine()));

	Action_Rect = new QAction(tr("&Rect"), this);
	connect(Action_Rect, SIGNAL(triggered()), view_widget_, SLOT(setRect()));

	Action_Ellipse = new QAction(tr("&Ellipse"), this);
	connect(Action_Ellipse, SIGNAL(triggered()), view_widget_, SLOT(setEllipse()));
}

void MiniDraw::Creat_ToolBar() {
	pToolBar = addToolBar(tr("&Main")); // ��������������ǩΪ Main
	pToolBar->addAction(Action_About);  // ���������ص� Main �˵���
	pToolBar->addAction(Action_Line);
	pToolBar->addAction(Action_Rect);
	pToolBar->addAction(Action_Ellipse);
}

void MiniDraw::Creat_Menu() {
	pMenu = menuBar()->addMenu(tr("&Figure Tool")); // �����˵�����ǩΪFigure Tool
	pMenu->addAction(Action_About); // ���������ص� Main �˵���
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
	view_widget_ = new ViewWidget(); // ʵ���� ViewWidget �ؼ�����
	//...
	setCentralWidget(view_widget_); // �� ViewWidget �ؼ�����Ϊ�����ڵ�����λ��
	//...
}
