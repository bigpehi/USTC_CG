#pragma once

#include "Shape.h"

class Rect : public Shape {
public:
	Rect() {};
	Rect(int upperLeft_point_x_, int upperLeft_point_y_, int lowerRight_point_x_, int lowRight_point_y_);
	~Rect() {};

	void Draw(QPainter& painter);

private:
	int upperLeft_point_x_, upperLeft_point_y_, lowerRight_point_x_, lowRight_point_y_;

};

