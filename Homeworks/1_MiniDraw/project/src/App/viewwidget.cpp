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
			qDebug() << "shape	" << "line" << endl;

			break;
		case Shape::kDefault:
			qDebug() << "shape	" << "default" << endl;
			break;

		case Shape::kRect:
			shape_ = new Rect();
			qDebug() << "shape	" << "rect" << endl;
			break;
		}
		if (shape_ != NULL)
		{
			draw_status_ = true; // ���û���״̬Ϊ �C ����
			start_point_ = end_point_ =  event->pos(); // ��ͼԪ��ʼ������Ϊ��ǰ��������
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
		// ɾ����ʱͼԪ
		std::vector<Shape*>::iterator it = shape_list_.begin();
		qDebug() << shape_cnt_ << endl;
		for (int i = 0; i < shape_cnt_; i++) {
			it++;
		}
		qDebug() << shape_cnt_ << endl;
		shape_list_.erase(it, shape_list_.end());

		end_point_ = event->pos(); // ��Ϊ�棬������ͼԪ��ֹ��λ��굱ǰλ��

		switch (type_){
		case Shape::kLine: {
			Line* current_line_ = NULL;
			current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
			shape_list_.push_back(current_line_);
			break; }
		case Shape::kDefault: {
			break; }
		case Shape::kRect: {
			Rect* current_rect_ = NULL;
			current_rect_ = new Rect(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
			//qDebug() << "rect_point		" << end_point_.rx() << "	" << end_point_.ry() << endl;
			shape_list_.push_back(current_rect_);
			break; }
		}

		update();
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{

	switch (type_) {
	case Shape::kLine: {
		Line* current_line_ = NULL;
		current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		shape_list_.push_back(current_line_);
		shape_cnt_++;//����ͼԪ����
		break; }
	case Shape::kDefault: {
		break; }
	case Shape::kRect: {
		Rect* current_rect_ = NULL;
		current_rect_ = new Rect(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		shape_list_.push_back(current_rect_);
		shape_cnt_++;//����ͼԪ����
		break; }
	}
	draw_status_ = false;
	qDebug() << "release	" << draw_status_ << endl;
	update();
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	//qDebug() << "paintevent" << endl;
	QPainter painter(this);
	// �ػ�����ͼԪ
	for (size_t i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}
	painter.end();
	//painter.drawLine(start_point_, end_point_);
}

ViewWidget::~ViewWidget()
{
	//��QT��Դ����Ҫ�û��Լ�����
	for (size_t i = 0; i < shape_list_.size(); i++)
	{
		if (shape_list_[i])
		{
			delete shape_list_[i];
			shape_list_[i] = NULL;
		}
	}
}