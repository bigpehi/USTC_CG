#include "viewwidget.h"
//#include <Line.h>

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false; // ���ó�ʼ����״̬Ϊ �C ������
	shape_ = NULL;
	type_ = Shape::kDefault;
}

ViewWidget::~ViewWidget()
{
}

void ViewWidget::setLine()
{
	type_ = Shape::kLine;
}

void ViewWidget::setRect()
{
	type_ = Shape::kRect;
}

void ViewWidget::mousePressEvent(QMouseEvent* event)
{
	if (Qt::LeftButton == event->button())
	{
		switch (type_)
		{
		case Shape::kLine:
			shape_ = new Line();

			break;
		case Shape::kDefault:
			break;

		case Shape::kRect:
			shape_ = new Rect();
			break;
		}
		if (shape_ != NULL)
		{
			draw_status_ = true; // ���û���״̬Ϊ �C ����
			start_point_ =  event->pos(); // ��ͼԪ��ʼ������Ϊ��ǰ��������
			shape_->set_start(start_point_);
			shape_->set_end(end_point_);

		}
	}
	
	qDebug() << "press	" << draw_status_ << endl;
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_) // �жϵ�ǰ����״̬
	{
		std::vector<Line*>::iterator it = line_array_.begin(); 
		qDebug() << line_cnt_ << endl;
		for (int i = 0; i < line_cnt_; i++) {
			it++;
		}
		qDebug() << line_cnt_ << endl;
		line_array_.erase(it, line_array_.end());
		end_point_ = event->pos(); // ��Ϊ�棬������ͼԪ��ֹ��λ��굱ǰλ��
		Line* current_line_ = NULL;
		current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		line_array_.push_back(current_line_);
		update();
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	//for (std::vector<Line*>::iterator it = tail; it != line_array_.end(); it++) {
	//	line_array_.erase(it);
	//}
	Line* current_line_ = NULL;
	current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
	line_array_.push_back(current_line_);
	line_cnt_++;//����ͼԪ����
	draw_status_ = false;
	qDebug() << "release	" << draw_status_ << endl;
	update();
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	qDebug() << "paintevent" << endl;
	QPainter painter(this);
	// �ػ������߶�
	for (size_t i = 0; i < line_array_.size(); i++)
	{
		line_array_[i]->Draw(painter);
	}
	painter.end();
	painter.drawLine(start_point_, end_point_);
}

//ViewWidget::~ViewWidget()
//{
//	for (size_t i = 0; i < line_array_.size(); i++)
//	{
//		if (line_array_[i])
//		{
//			delete line_array_[i];
//			line_array_[i] = NULL;
//		}
//	}
//}