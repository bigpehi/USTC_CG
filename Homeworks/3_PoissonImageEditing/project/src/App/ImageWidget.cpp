#include "ImageWidget.h"
#include <QImage>
#include <QPainter>
#include <QtWidgets> 
#include <iostream>
#include "ChildWindow.h"

using std::cout;
using std::endl;

ImageWidget::ImageWidget(ChildWindow* relatewindow)
{
	image_ = new QImage();
	image_backup_ = new QImage();

	draw_status_ = kNone;
	is_choosing_ = false;
	is_pasting_ = false;

	point_start_ = QPoint(0, 0);
	point_end_ = QPoint(0, 0);

	source_window_ = NULL;
}

ImageWidget::~ImageWidget(void)
{
}

int ImageWidget::ImageWidth()
{
	return image_->width();
}

int ImageWidget::ImageHeight()
{
	return image_->height();
}

void ImageWidget::set_draw_status_to_choose()
{
	draw_status_ = kChoose;	
}

void ImageWidget::set_draw_status_to_paste()
{
	draw_status_ = kPaste;
}

void ImageWidget::set_draw_status_to_paste_mix()
{
	draw_status_ = kPaste_mix;
}

QImage* ImageWidget::image()
{
	return image_;
}

void ImageWidget::set_source_window(ChildWindow* childwindow)
{
	source_window_ = childwindow;
}

void ImageWidget::paintEvent(QPaintEvent* paintevent)
{
	QPainter painter;
	painter.begin(this);

	// Draw background
	painter.setBrush(Qt::lightGray);
	QRect back_rect(0, 0, width(), height());
	painter.drawRect(back_rect);

	// Draw image
	QRect rect = QRect(0, 0, image_->width(), image_->height());
	painter.drawImage(rect, *image_);

	// Draw choose region
	painter.setBrush(Qt::NoBrush);
	painter.setPen(Qt::red);
	painter.drawRect(point_start_.x(), point_start_.y(),
		point_end_.x() - point_start_.x(), point_end_.y() - point_start_.y());

	painter.end();
}

void ImageWidget::mousePressEvent(QMouseEvent* mouseevent)
{
	if (Qt::LeftButton == mouseevent->button())
	{
		switch (draw_status_)
		{
		case kChoose:
			is_choosing_ = true;
			point_start_ = point_end_ = mouseevent->pos();
			break;

		case kPaste:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image
			//	*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}
		case kPaste_mix:
		{
			is_pasting_ = true;

			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image
			//	*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}

		update();
		break;

		default:
			break;
		}
	}
}

void ImageWidget::mouseMoveEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		// Store point position for rectangle region
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
		}
		break;

	case kPaste:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image 
				*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}
		break;

	case kPaste_mix:
		// Paste rectangle region to object image
		if (is_pasting_)
		{
			// Start point in object image
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			// Start point in source image
			int xsourcepos = source_window_->imagewidget_->point_start_.rx();
			int ysourcepos = source_window_->imagewidget_->point_start_.ry();

			// Width and Height of rectangle region
			int w = source_window_->imagewidget_->point_end_.rx()
				- source_window_->imagewidget_->point_start_.rx() + 1;
			int h = source_window_->imagewidget_->point_end_.ry()
				- source_window_->imagewidget_->point_start_.ry() + 1;

			// Paste
			if ((xpos > 0) && (ypos > 0) && (xpos + w < image_->width()) && (ypos + h < image_->height()))
			{
				// Restore image 
				*(image_) = *(image_backup_);

				// Paste
				for (int i = 0; i < w; i++)
				{
					for (int j = 0; j < h; j++)
					{
						image_->setPixel(xpos + i, ypos + j, source_window_->imagewidget_->image()->pixel(xsourcepos + i, ysourcepos + j));
					}
				}
			}
		}
		break;

	default:
		break;
	}
	//qDebug() << point_start_.x() << "  " << point_start_.y() << "  " << point_end_.x() << "  " << point_end_.y() << endl;
	update();
}

void ImageWidget::mouseReleaseEvent(QMouseEvent* mouseevent)
{
	switch (draw_status_)
	{
	case kChoose:
		if (is_choosing_)
		{
			point_end_ = mouseevent->pos();
			is_choosing_ = false;
			draw_status_ = kNone;

			roi_fg_.x = point_start_.x();
			roi_fg_.y = point_start_.y();
			roi_fg_.width = point_end_.x() - point_start_.x();
			roi_fg_.height = point_end_.y() - point_start_.y();
			qDebug() << roi_fg_.x << "  " << roi_fg_.y << "  " << roi_fg_.width << "  " << roi_fg_.height << endl;
		}
		break;
	case kPaste:
		if (is_pasting_)
		{
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			roi_bg_.x = xpos;
			roi_bg_.y = ypos;
			roi_bg_.width = source_window_->imagewidget_->roi_fg_.width;
			roi_bg_.height = source_window_->imagewidget_->roi_fg_.height;

			//qDebug() << source_window_->imagewidget_->roi_.x << "  " << source_window_->imagewidget_->roi_.y << "  " << source_window_->imagewidget_->roi_.width << "  " << source_window_->imagewidget_->roi_.height << endl;
			qDebug() << "paste" << endl;
			// QImage -> Mat
			QImage image_bg = image_backup_->copy();
			cv::Mat src_bg = QImage2cvMat(image_bg); //image_是拼接后的图片
			cv::Mat src_fg = QImage2cvMat(*source_window_->imagewidget_->image());
			//qDebug() << source_window_->imagewidget_->roi_.x << "  " << source_window_->imagewidget_->roi_.y << "  " << source_window_->imagewidget_->roi_.width << "  " << source_window_->imagewidget_->roi_.height << endl;
			PIE pie(src_bg, src_fg, &roi_bg_, &source_window_->imagewidget_->roi_fg_);
			cv::Mat blend_result = pie.get_output_image_();
			// Mat -> QImage
			image_ = &Mat2QImage(blend_result);


			is_pasting_ = false;
			draw_status_ = kNone;
		}
		break;
	case kPaste_mix:
		qDebug() << "PIE_mixed!" << endl;
		if (is_pasting_)
		{
			int xpos = mouseevent->pos().rx();
			int ypos = mouseevent->pos().ry();

			roi_bg_.x = xpos;
			roi_bg_.y = ypos;
			roi_bg_.width = source_window_->imagewidget_->roi_fg_.width;
			roi_bg_.height = source_window_->imagewidget_->roi_fg_.height;

			//qDebug() << source_window_->imagewidget_->roi_.x << "  " << source_window_->imagewidget_->roi_.y << "  " << source_window_->imagewidget_->roi_.width << "  " << source_window_->imagewidget_->roi_.height << endl;
			qDebug() << "paste" << endl;
			// QImage -> Mat
			QImage image_bg = image_backup_->copy();
			cv::Mat src_bg = QImage2cvMat(image_bg) ; //image_是拼接后的图片
			cv::Mat src_fg = QImage2cvMat(*source_window_->imagewidget_->image());
			//qDebug() << source_window_->imagewidget_->roi_.x << "  " << source_window_->imagewidget_->roi_.y << "  " << source_window_->imagewidget_->roi_.width << "  " << source_window_->imagewidget_->roi_.height << endl;
			PIEMixed pie(src_bg, src_fg, &roi_bg_, &source_window_->imagewidget_->roi_fg_);
			cv::Mat blend_result = pie.get_output_image_();
			// Mat -> QImage
			image_ = &Mat2QImage(blend_result);


			is_pasting_ = false;
			draw_status_ = kNone;
		}
		break;

	default:
		break;
	}

	update();
}

void ImageWidget::Open(QString filename)
{
	// Load file
	if (!filename.isEmpty())
	{
		image_->load(filename);
		*(image_backup_) = *(image_);
	}


	//	setFixedSize(image_->width(), image_->height());
	//	relate_window_->setWindowFlags(Qt::Dialog);
	//	relate_window_->setFixedSize(QSize(image_->width(), image_->height()));
	//	relate_window_->setWindowFlags(Qt::SubWindow);

		//image_->invertPixels(QImage::InvertRgb);
		//*(image_) = image_->mirrored(true, true);
		//*(image_) = image_->rgbSwapped();
	cout << "image size: " << image_->width() << ' ' << image_->height() << endl;
	update();
}

void ImageWidget::Save()
{
	SaveAs();
}

void ImageWidget::SaveAs()
{
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Image"), ".", tr("Images(*.bmp *.png *.jpg)"));
	if (filename.isNull())
	{
		return;
	}

	image_->save(filename);
}

void ImageWidget::Invert()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			image_->setPixel(i, j, qRgb(255 - qRed(color), 255 - qGreen(color), 255 - qBlue(color)));
		}
	}

	// equivalent member function of class QImage
	// image_->invertPixels(QImage::InvertRgb);
	update();
}

void ImageWidget::Mirror(bool ishorizontal, bool isvertical)
{
	QImage image_tmp(*(image_));
	int width = image_->width();
	int height = image_->height();

	if (ishorizontal)
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, height - 1 - j));
				}
			}
		}
		else
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(i, height - 1 - j));
				}
			}
		}

	}
	else
	{
		if (isvertical)
		{
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					image_->setPixel(i, j, image_tmp.pixel(width - 1 - i, j));
				}
			}
		}
	}

	// equivalent member function of class QImage
	//*(image_) = image_->mirrored(true, true);
	update();
}

void ImageWidget::TurnGray()
{
	for (int i = 0; i < image_->width(); i++)
	{
		for (int j = 0; j < image_->height(); j++)
		{
			QRgb color = image_->pixel(i, j);
			int gray_value = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
			image_->setPixel(i, j, qRgb(gray_value, gray_value, gray_value));
		}
	}

	update();
}

void ImageWidget::Restore()
{
	*(image_) = *(image_backup_);
	point_start_ = point_end_ = QPoint(0, 0);
	update();
}


cv::Mat ImageWidget::QImage2cvMat(QImage image)
{
	cv::Mat mat;
	//qDebug() << image.format();
	switch (image.format())
	{
	case QImage::Format_ARGB32:
	case QImage::Format_RGB32:
	case QImage::Format_ARGB32_Premultiplied:
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
		break;
	case QImage::Format_RGB888:
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
		cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
		break;
	case QImage::Format_Indexed8:
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
		break;
	}
	return mat;
}


QImage ImageWidget::Mat2QImage(const cv::Mat& InputMat)

{

	cv::Mat TmpMat;

	// convert the color space to RGB
	if (InputMat.channels() == 1)

	{
		cv::cvtColor(InputMat, TmpMat, cv::COLOR_GRAY2RGB);
	}

	else

	{
		cv::cvtColor(InputMat, TmpMat, cv::COLOR_BGR2RGB);
	}


	// construct the QImage using the data of the mat, while do not copy the data

	QImage Result = QImage((const uchar*)(TmpMat.data), TmpMat.cols, TmpMat.rows,

		QImage::Format_RGB888);

	// deep copy the data from mat to QImage

	Result.bits();

	return Result;
}