#include "Line.h"

Line::Line(int start_point_x, int start_point_y, int end_point_x, int end_point_y, QColor color, int width)
{
    qDebug() << "	r=" << color.red() << "	g=" << color.green() << "	b=" << color.blue() << endl;
    start_point_x_ = start_point_x;
    start_point_y_ = start_point_y;
    end_point_x_ = end_point_x;
    end_point_y_ = end_point_y;
    color_ = color;
    width_ = width;
}

void Line::Draw(QPainter& paint)
{
    paint.setPen(QPen(color_, width_));
    paint.drawLine(start_point_x_, start_point_y_, end_point_x_, end_point_y_);
}
