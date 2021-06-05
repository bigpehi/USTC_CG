#include "Brush.h"



Brush::Brush(QPointF* point_set,  QColor color, int width)
{
	point_set_ = point_set;
	color_ = color;
	width_ = width;
}


void Brush::Draw(QPainter& painter)
{
	painter.setPen(QPen(color_, width_));
	//painter.drawPoints(point_set_, point_num); // ������
	for (int i = 0; i < point_num-1; i++) {
		painter.drawLine(point_set_[i], point_set_[i + 1]);
	}
	qDebug() << "width_" << width_ << point_num <<endl;
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
		delete[] point_set_; //�ͷ���ʱ����Ķ��ڴ�
		
	}
	point_set_ = new_point_set;
	point_num++;
}