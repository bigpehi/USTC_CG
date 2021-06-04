#pragma once

#include <ui_viewwidget.h>

#include "Shape.h"
#include "Line.h"
#include "Rect.h"
#include "Ellipse2.h"
#include "Brush.h"


#include <qevent.h>
#include <qpainter.h>
#include <QWidget>

#include <vector>

class ViewWidget : public QWidget
{
	Q_OBJECT

public:
	ViewWidget(QWidget* parent = 0);
	~ViewWidget();

private:
	Ui::ViewWidget ui;

private:
	bool draw_status_; // ��ǰ����״̬��true Ϊ���Ƶ�ǰ����϶���ͼԪ��false Ϊ������
	QPoint start_point_; // ��ǰͼԪ����ʼ��
	QPoint end_point_; // ��ǰͼԪ����ֹ��
	Shape::Type type_;
	Shape* shape_;
	std::vector<Shape*> shape_list_;
	int  shape_cnt_ = 0; // ���ڼ�¼��ǰ��Ҫdraw������Ԫ�ص����һ����������Ϊ��ʱͼԪ


public:
	void mousePressEvent(QMouseEvent* event); // ��������Ӧ���������Ҽ�����˫����
	void mouseMoveEvent(QMouseEvent* event); // ����ƶ���Ӧ��������һ����Ҫ�������ĵ������������
	void mouseReleaseEvent(QMouseEvent* event); // ����ͷ���Ӧ���������Ҽ�����˫����

public:
	void paintEvent(QPaintEvent*); // Qt ���еĻ��ƶ�ֻ���ڴ˺��������
signals:
public slots:
	void setLine();
	void setRect();
	void setEllipse();
	void setBrush();
	void undo();

};
