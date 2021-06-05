#include "viewwidget.h"
#include <QColorDialog>
#include <QtGui>

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

void ViewWidget::setEllipse()
{
	type_ = Shape::kEllipse;
}

void ViewWidget::setBrush()
{
	type_ = Shape::kBrush;
}

void ViewWidget::undo()
{
	qDebug() << "undo" << endl;
	if (!shape_list_.empty()) {
		shape_list_.pop_back();
		shape_cnt_--;

	}
	else
	{
		qDebug() << "The canvas is empty!" << endl;
	}
	update();
}

void ViewWidget::setColor()
{
	color = QColorDialog::getColor(QColor(0, 0, 0));
	//qDebug() << "	r=" <<color.red() << "	g=" << color.green() << "	b=" << color.blue() << endl;
}

void ViewWidget::setPenStyle()
{
	
}

void ViewWidget::setPenWidth1()
{
	width_ = 1;
}

void ViewWidget::setPenWidth2()
{
	width_ = 2;
}

void ViewWidget::setPenWidth3()
{
	width_ = 3;
}

void ViewWidget::setPenWidth4()
{
	width_ = 4;
}

void ViewWidget::setPenWidth5()
{
	width_ = 5;
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
		case Shape::kEllipse:
			shape_ = new Ellipse2();
			qDebug() << "shape	" << "Ellipse" << endl;
			break;
		case Shape::kBrush:
			shape_ = new Brush();
			qDebug() << "shape	" << "Brush" << endl;
			qDebug() << "create a brush" << endl;
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
		//qDebug() << shape_cnt_ << endl;
		for (int i = 0; i < shape_cnt_; i++) {
			it++;
		}
		//qDebug() << shape_cnt_ << endl;
		shape_list_.erase(it, shape_list_.end());

		end_point_ = event->pos(); // ��Ϊ�棬������ͼԪ��ֹ��λ��굱ǰλ��

		switch (type_){
		case Shape::kLine: {
			Line* current_line_ = NULL;
			current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry(),color,width_);
			shape_list_.push_back(current_line_);
			break; }
		case Shape::kDefault: {
			break; }
		case Shape::kRect: {
			Rect* current_rect_ = NULL;
			current_rect_ = new Rect(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry(), color, width_);
			//qDebug() << "rect_point		" << end_point_.rx() << "	" << end_point_.ry() << endl;
			shape_list_.push_back(current_rect_);
			break; }
		case Shape::kEllipse: {
			Ellipse2* current_Ellipse_ = NULL;
			current_Ellipse_ = new Ellipse2(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry(),color, width_);
			//qDebug() << "rect_point		" << end_point_.rx() << "	" << end_point_.ry() << endl;
			shape_list_.push_back(current_Ellipse_);
			break;}
		case Shape::kBrush: {
			qDebug() << "brush is moving" << endl;
			QPointF *current_point_ = new QPointF();
			current_point_->setX(end_point_.rx());
			current_point_->setY(end_point_.ry());
			Brush* brush = (Brush*)shape_;
			brush->Add_point(current_point_);
			brush->color_ = color;
			brush->width_ = width_;
			shape_list_.push_back(brush);

			break; }
		}

		update();
		qDebug() << "updata" << endl;
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// ɾ����ʱͼԪ
	std::vector<Shape*>::iterator it = shape_list_.begin();
	//qDebug() << shape_cnt_ << endl;
	for (int i = 0; i < shape_cnt_; i++) {
		it++;
	}
	//qDebug() << shape_cnt_ << endl;
	shape_list_.erase(it, shape_list_.end());

	switch (type_) {
	case Shape::kLine: {
		Line* current_line_ = NULL;
		current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry(), color, width_);
		shape_list_.push_back(current_line_);
		shape_cnt_++;//����ͼԪ����
		break; }
	case Shape::kDefault: {
		break; }
	case Shape::kRect: {
		Rect* current_rect_ = NULL;
		current_rect_ = new Rect(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry(), color, width_);
		shape_list_.push_back(current_rect_);
		shape_cnt_++;//����ͼԪ����
		break; }
	case Shape::kEllipse: {
		Ellipse2* current_Ellipse_ = NULL;
		current_Ellipse_ = new Ellipse2(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry(), color, width_);
		shape_list_.push_back(current_Ellipse_);
		shape_cnt_++;//����ͼԪ����
		break; }
	case Shape::kBrush: {
		qDebug() << "release a brush" << endl;
		QPointF* current_point_ = new QPointF();
		current_point_->setX(end_point_.rx());
		current_point_->setY(end_point_.ry());
		Brush* brush = (Brush*)shape_;
		brush->Add_point(current_point_);
		shape_list_.push_back(brush);
		shape_cnt_++;//����ͼԪ����
		break; }
	}
	draw_status_ = false;
	qDebug() << "release	" << draw_status_ << endl;
	update();
}

void ViewWidget::paintEvent(QPaintEvent*)
{
	qDebug() << "paintevent" << endl;
	qDebug() << shape_list_.size() << endl;
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