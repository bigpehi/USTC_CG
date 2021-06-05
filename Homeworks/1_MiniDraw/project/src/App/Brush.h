#pragma once

#include "Shape.h"

class Brush : public Shape {
public:
	Brush() {};
	Brush(QPointF* point_set, QColor color, int width);
	~Brush() {};

	void Draw(QPainter& painter);
	void Add_point(QPointF* point);
	QColor color_;
	int width_=1;

private:
	QPointF* point_set_= new QPointF();
	int point_num = 0;

};

