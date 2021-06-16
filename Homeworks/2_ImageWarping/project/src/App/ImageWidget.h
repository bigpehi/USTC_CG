#pragma once
#include <QWidget>
#include <Line.h>
#include <warp_IDW.h>
#include <warp_RBF.h>
#include <QDebug>

QT_BEGIN_NAMESPACE
class QImage;
class QPainter;
QT_END_NAMESPACE

class ImageWidget :
	public QWidget
{
	Q_OBJECT

public:
	ImageWidget(void);
	~ImageWidget(void);

protected:
	void paintEvent(QPaintEvent *paintevent);

public slots:
	// File IO
	void Open();												// Open an image file, support ".bmp, .png, .jpg" format
	void Save();												// Save image to current file
	void SaveAs();												// Save image to another file

	// Image processing
	void Invert();												// Invert pixel value in image
	void Mirror(bool horizontal=false, bool vertical=true);		// Mirror image vertically or horizontally
	void TurnGray();											// Turn image to gray-scale map
	void Restore();												// Restore image to origin
	void Warp_IDW();
	void Warp_RBF();
	void DrawLine();

	// Mouse event
	void mousePressEvent(QMouseEvent* event); // 鼠标击发响应函数（左右键，单双击）
	void mouseMoveEvent(QMouseEvent* event); // 鼠标移动响应函数（其一个重要性质在文档最后有详述）
	void mouseReleaseEvent(QMouseEvent* event); // 鼠标释放响应函数（左右键，单双击）

private:
	QImage		*ptr_image_;				// image 
	QImage		*ptr_image_backup_;

	// MOuse event
	bool draw_status_; // 当前绘制状态，true 为绘制当前鼠标拖动的图元，false 为不绘制
	QPoint start_point_; // 当前图元的起始点
	QPoint end_point_; // 当前图元的终止点
	std::vector<Line*> line_list_;
	Line* line_;
	int line_cnt_ = 0;
	bool warp_bool_ = false;

signals:
public slots:
	void undo();


};

