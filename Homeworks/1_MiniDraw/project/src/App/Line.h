#pragma once

#include "Shape.h"

class Line : public Shape {
public:
	Line(void) {};
	Line(int start_point_x, int start_point_y, int end_point_x, int end_point_y, QColor color, int width);
	~Line() {};
	
	void Draw(QPainter& painter);

private:
	int start_point_x_, start_point_y_, end_point_x_, end_point_y_;
	QColor color_;
	int width_;

};
