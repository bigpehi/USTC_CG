#include "Rect.h"



Rect::Rect(int upperLeft_point_x, int upperLeft_point_y, int lowerRight_point_x, int lowRight_point_y, QColor color, int width)
{
	upperLeft_point_x_ = upperLeft_point_x;
	upperLeft_point_y_ = upperLeft_point_y;
	lowerRight_point_x_ = lowerRight_point_x;
	lowRight_point_y_ = lowRight_point_y;
	color_ = color;
	width_ = width;
}


void Rect::Draw(QPainter& painter)
{
	painter.setPen(QPen(color_, width_));
	painter.drawRect(upperLeft_point_x_, upperLeft_point_y_,
		lowerRight_point_x_ - upperLeft_point_x_, lowRight_point_y_ - upperLeft_point_y_);
	qDebug() << "A rect has been drawn!" << endl;

}
