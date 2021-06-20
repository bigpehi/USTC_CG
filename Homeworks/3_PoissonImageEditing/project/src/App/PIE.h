#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <Eigen/Dense>
#include "Eigen/Sparse"
#include<vector>
#include "Eigen/IterativeLinearSolvers"
using namespace Eigen;



class PIE {
public:
	PIE();
	PIE(cv::Mat src_bg, cv::Mat src_fg, cv::Rect* roi_bg, cv::Rect* roi_fg);
	cv::Mat get_output_image_();

private:
	void compute_gradient_x_(); // compute the gradient of the image
	void compute_gradient_y_(); // compute the gradient of the image
	void blend_gradient_(); // blend the gradient of the gradient_f
	void compute_divergence_(); // compute the divergence of the blended gradient
	void compute_matrix_equation_();
	void compute_laplacian_test_();

	cv::Mat gradient_bg_x_; //background
	cv::Mat gradient_bg_y_;
	cv::Mat gradient_fg_x_; // foreground
	cv::Mat gradient_fg_y_;
	cv::Mat gradient_patch_x_; // patch
	cv::Mat gradient_patch_y_;

	cv::Mat laplacian_;
	cv::Mat laplacian_bg_;
	cv::Mat laplacian_fg_;

	cv::Mat coefficient_matrix_;

	cv::Mat src_bg_;
	cv::Mat src_fg_;
	cv::Mat patch_;
	cv::Mat dst_;

	cv::Rect* roi_bg_;
	cv::Rect* roi_fg_;
};