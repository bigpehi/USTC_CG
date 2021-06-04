#include "Brush.h"



Brush::Brush(QPointF* point_set,  QColor color)
{
	point_set_ = point_set;
	color_ = color;
}


void Brush::Draw(QPainter& painter)
{
	painter.setPen(QPen(color_, 5));
	//painter.drawPoints(point_set_, point_num); // 不连续
	for (int i = 0; i < point_num-1; i++) {
		painter.drawLine(point_set_[i], point_set_[i + 1]);
	}
	//qDebug() << "drawing points" << "points num	" << point_num <<endl;
}

void Brush::Add_point(QPointF* point) {
	QPointF *new_point_set = new QPointF[point_num+1];
	for (int i = 0; i < point_num; i++) {
		new_point_set[i] = point_set_[i];
	}
	if (point_set_ == nullptr) {
		new_point_set[0] = *point;
	}
	else {
		new_point_set[point_num] = *point;
		delete[] point_set_; //释放临时申请的堆内存
		
	}
	point_set_ = new_point_set;
	point_num++;
}