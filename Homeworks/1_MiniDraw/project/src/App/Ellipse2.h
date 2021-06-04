#pragma once

#include "Shape.h"

class Ellipse2 : public Shape {
public:
	Ellipse2() {};
	Ellipse2(int upperLeft_point_x_, int upperLeft_point_y_, int lowerRight_point_x_, int lowRight_point_y_);
	~Ellipse2() {};

	void Draw(QPainter& painter);

private:
	int upperLeft_point_x_, upperLeft_point_y_, lowerRight_point_x_, lowRight_point_y_;

};

