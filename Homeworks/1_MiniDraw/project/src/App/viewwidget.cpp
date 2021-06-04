#include "viewwidget.h"

ViewWidget::ViewWidget(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	draw_status_ = false; // 设置初始绘制状态为 – 不绘制
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
			draw_status_ = true; // 设置绘制状态为 – 绘制
			start_point_ = end_point_ =  event->pos(); // 将图元初始点设置为当前鼠标击发点
			shape_->set_start(start_point_);
			shape_->set_end(end_point_);

		}
	}
	
	qDebug() << "press	" << draw_status_ << endl;
}

void ViewWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (draw_status_) // 判断当前绘制状态
	{
		// 删除临时图元
		std::vector<Shape*>::iterator it = shape_list_.begin();
		//qDebug() << shape_cnt_ << endl;
		for (int i = 0; i < shape_cnt_; i++) {
			it++;
		}
		//qDebug() << shape_cnt_ << endl;
		shape_list_.erase(it, shape_list_.end());

		end_point_ = event->pos(); // 若为真，则设置图元终止点位鼠标当前位置

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
		case Shape::kEllipse: {
			Ellipse2* current_Ellipse_ = NULL;
			current_Ellipse_ = new Ellipse2(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
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
			shape_list_.push_back(brush);

			break; }
		}

		update();
		qDebug() << "updata" << endl;
	}
}

void ViewWidget::mouseReleaseEvent(QMouseEvent* event)
{

	switch (type_) {
	case Shape::kLine: {
		Line* current_line_ = NULL;
		current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		shape_list_.push_back(current_line_);
		shape_cnt_++;//设置图元数量
		break; }
	case Shape::kDefault: {
		break; }
	case Shape::kRect: {
		Rect* current_rect_ = NULL;
		current_rect_ = new Rect(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		shape_list_.push_back(current_rect_);
		shape_cnt_++;//设置图元数量
		break; }
	case Shape::kEllipse: {
		Ellipse2* current_Ellipse_ = NULL;
		current_Ellipse_ = new Ellipse2(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		shape_list_.push_back(current_Ellipse_);
		shape_cnt_++;//设置图元数量
		break; }
	case Shape::kBrush: {
		qDebug() << "release a brush" << endl;
		QPointF* current_point_ = new QPointF();
		current_point_->setX(end_point_.rx());
		current_point_->setY(end_point_.ry());
		Brush* brush = (Brush*)shape_;
		brush->Add_point(current_point_);
		shape_list_.push_back(brush);
		shape_cnt_++;//设置图元数量
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
	// 重画所有图元
	for (size_t i = 0; i < shape_list_.size(); i++)
	{
		shape_list_[i]->Draw(painter);
	}
	painter.end();
	//painter.drawLine(start_point_, end_point_);
}

ViewWidget::~ViewWidget()
{
	//非QT资源，需要用户自己回收
	for (size_t i = 0; i < shape_list_.size(); i++)
	{
		if (shape_list_[i])
		{
			delete shape_list_[i];
			shape_list_[i] = NULL;
		}
	}
}