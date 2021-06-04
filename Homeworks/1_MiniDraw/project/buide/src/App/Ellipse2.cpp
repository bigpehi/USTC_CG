#include "Ellipse2.h"



Ellipse2::Ellipse2(int upperLeft_point_x, int upperLeft_point_y, int lowerRight_point_x, int lowRight_point_y)
{
	upperLeft_point_x_ = upperLeft_point_x;
	upperLeft_point_y_ = upperLeft_point_y;
	lowerRight_point_x_ = lowerRight_point_x;
	lowRight_point_y_ = lowRight_point_y;
}


void Ellipse2::Draw(QPainter& painter)
{
	painter.drawEllipse(upperLeft_point_x_, upperLeft_point_y_,
		lowerRight_point_x_ - upperLeft_point_x_, lowRight_point_y_ - upperLeft_point_y_);
	qDebug() << "A Ellipse has been drawn!" << endl;
}
