#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>

using std::cout;
using std::endl;

ImageWidget::ImageWidget(void)
{
	ptr_image_ = new QImage();
	ptr_image_backup_ = new QImage();
}


ImageWidget::~ImageWidget(void)
{
}

void ImageWidget::paintEvent(QPaintEvent *paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect( (width()-ptr_image_->width())/2, (height()-ptr_image_->height())/2, ptr_image_->width(), ptr_image_->height());
	painter.drawImage(rect, *ptr_image_); 

	// Draw line
	if (warp_bool_) {
		for (size_t i = 0; i < line_list_.size(); i++)
		{
			line_list_[i]->Draw(painter);
		}
	}
	painter.end();


}



void ImageWidget::mousePressEvent(QMouseEvent* event) {
	if (warp_bool_) {
		line_ = new Line();
		draw_status_ = true; // 设置绘制状态为 – 绘制
		start_point_ = end_point_ = event->pos(); // 将图元初始点设置为当前鼠标击发点
		line_->set_start(start_point_);
		line_->set_end(end_point_);
	}

}



void ImageWidget::mouseMoveEvent(QMouseEvent* event) {
	if (draw_status_ && warp_bool_) // 判断当前绘制状态
	{
		// 删除临时图元
		std::vector<Line*>::iterator it = line_list_.begin();
		//qDebug() << shape_cnt_ << endl;
		for (int i = 0; i < line_cnt_; i++) {
			it++;
		}
		//qDebug() << shape_cnt_ << endl;
		line_list_.erase(it, line_list_.end());

		end_point_ = event->pos(); // 若为真，则设置图元终止点位鼠标当前位置

		Line* current_line_ = NULL;
		current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		line_list_.push_back(current_line_);
		update();
		qDebug() << "updata" << endl;
	}

}


void ImageWidget::mouseReleaseEvent(QMouseEvent* event) {
	if (warp_bool_) {
		// 删除临时图元
		std::vector<Line*>::iterator it = line_list_.begin();
		//qDebug() << shape_cnt_ << endl;
		for (int i = 0; i < line_cnt_; i++) {
			it++;
		}
		//qDebug() << shape_cnt_ << endl;
		line_list_.erase(it, line_list_.end());

		Line* current_line_ = NULL;
		current_line_ = new Line(start_point_.rx(), start_point_.ry(), end_point_.rx(), end_point_.ry());
		line_list_.push_back(current_line_);
		line_cnt_++;//设置图元数量

		draw_status_ = false;
		qDebug() << "release	" << draw_status_ << endl;
		update();
	}

}


void ImageWidget::undo() {
	qDebug() << "undo" << endl;
}


void ImageWidget::Warp_IDW() {
	if (line_cnt_ == 0)
		return;
	QVector<QPoint> p_points, q_points;
	for (size_t i = 0; i < line_list_.size(); i++)
	{
		double image_top_left_x = (width() - ptr_image_->width()) / 2;
		double image_top_left_y = (height() - ptr_image_->height()) / 2;
		QPoint p(line_list_[i]->start_point_x_- image_top_left_x, line_list_[i]->start_point_y_ - image_top_left_y);
		QPoint q(line_list_[i]->end_point_x_- image_top_left_x, line_list_[i]->end_point_y_- image_top_left_y);
		p_points.append(p);
		q_points.append(q);
	}

	IDW  *curent_warp_IDW = new IDW(p_points,q_points);
	QImage* image_output = curent_warp_IDW->warp_image_by_IDW(*ptr_image_);
	for (int i = 0; i < ptr_image_->width(); i++)
	{
		for (int j = 0; j < ptr_image_->height(); j++)
		{
			QRgb color = image_output->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(qRed(color), qGreen(color), qBlue(color)));
		}
	}

	// clear lines
	warp_bool_ = false;
	line_cnt_ = 0;
	line_list_.clear();

	update();

}


void ImageWidget::Warp_RBF() {
	warp_bool_ = true;
	line_list_.clear();
	line_cnt_ = 0;
	update();
	qDebug() << "warp_RBF" << endl;
}

void ImageWidget::DrawLine()
{
	warp_bool_ = true;
	line_list_.clear();
	line_cnt_ = 0;
	update();
}



void ImageWidget::Open()
{
	warp_bool_ = false;
	line_cnt_ = 0;
	line_list_.clear();

	// Open file
	QString fileName = QFileDialog::getOpenFileName(this, tr("Read Image"), ".", tr("Images(*.bmp *.png *.jpg)"));

	// Load file
	if (!fileName.isEmpty())
	{
		ptr_image_->load(fileName);
		*(ptr_image_backup_) = *(ptr_image_);
	}

	//ptr_image_->invertPixels(QImage::InvertRgb);
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	//*(ptr_image_) = ptr_image_->rgbSwapped();
	//cout<<"image size: "<<ptr_image_->width()<<' '<<ptr_image_->height()<<endl;
	update();
}

void ImageWidget::Save()
{
	warp_bool_ = false;
	SaveAs();
}

void ImageWidget::SaveAs()
{
	warp_bool_ = false;
	line_list_.clear();
	line_cnt_ = 0;

	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}	

	ptr_image_->save(filename);
}

void ImageWidget::Invert()
{
	warp_bool_ = false;
	line_list_.clear();
	line_cnt_ = 0;

	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			ptr_image_->setPixel(i, j, qRgb(255-qRed(color), 255-qGreen(color), 255-qBlue(color)) );
		}
	}

	// equivalent member function of class QImage
	// ptr_image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	warp_bool_ = false;
	line_list_.clear();
	line_cnt_ = 0;

	QImage image_tmp(*(ptr_image_));
	int width = ptr_image_->width();
	int height = ptr_image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, height-1-j));
				}
			}
		} 
		else
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(i, height-1-j));
				}
			}
		}
		
	}
	else
	{
		if (isvertical)
		{
			for (int i=0; i<width; i++)
			{
				for (int j=0; j<height; j++)
				{
					ptr_image_->setPixel(i, j, image_tmp.pixel(width-1-i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(ptr_image_) = ptr_image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	warp_bool_ = false;
	line_list_.clear();
	line_cnt_ = 0;

	for (int i=0; i<ptr_image_->width(); i++)
	{
		for (int j=0; j<ptr_image_->height(); j++)
		{
			QRgb color = ptr_image_->pixel(i, j);
			int gray_value = (qRed(color)+qGreen(color)+qBlue(color))/3;
			ptr_image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value) );
		}
	}

	update();
}

void ImageWidget::Restore()
{
	warp_bool_ = false;
	line_list_.clear();
	line_cnt_ = 0;
	*(ptr_image_) = *(ptr_image_backup_);
	update();
}