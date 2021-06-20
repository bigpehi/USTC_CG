#include "PIE.h"


PIE::PIE()
{
}

PIE::PIE(cv::Mat src_bg, cv::Mat src_fg, cv::Rect* roi_bg, cv::Rect* roi_fg, SparseLU<SparseMatrix<double>>* solver)
{
    cv::imwrite("./src_bg.jpg", src_bg); // 修改图片格式
    cv::imwrite("./src_fg.jpg", src_fg); // 修改图片格式
    src_bg_ = cv::imread("./src_bg.jpg", -1);
    src_fg_ = cv::imread("./src_fg.jpg", -1);
    roi_bg_ = roi_bg;
    roi_fg_ = roi_fg;
    patch_ = src_fg(*roi_fg_).clone(); //将src_fg中对应的ROI进行提取并克隆（单独对克隆区域操作而不改变原src中对应的ROI图像）

    solver_ = solver;
    if (solver->info() != Success) {
        qDebug() << "decomposition failed" << endl;
    }

    gradient_bg_x_ = cv::Mat::zeros(src_bg_.rows, src_bg_.cols, CV_8UC4);
    gradient_bg_y_ = cv::Mat::zeros(src_bg_.rows, src_bg_.cols, CV_8UC4);
    gradient_fg_x_ = cv::Mat::zeros(src_fg_.rows, src_fg_.cols, CV_8UC4); // foreground
    gradient_fg_y_ = cv::Mat::zeros(src_fg_.rows, src_fg_.cols, CV_8UC4);

}

cv::Mat PIE::get_output_image_()
{
    compute_gradient_x_();
    compute_gradient_y_();
    //blend_gradient_();
    //compute_divergence_();
    compute_laplacian_test_();
    compute_matrix_equation_();
    return src_bg_;
}

void PIE::compute_gradient_x_()
{
    // 初始化kernel
    cv::Mat kernel = cv::Mat::zeros(1, 3, CV_8S);
    kernel.at<char>(0, 2) = 1;
    kernel.at<char>(0, 1) = -1;

    filter2D(src_bg_, gradient_bg_x_, CV_8U, kernel); // only color image
    filter2D(src_fg_, gradient_fg_x_, CV_8U, kernel); // only color image

    // 将背景图片的roi部分梯度替换成patch的梯度
    gradient_patch_x_ = gradient_bg_x_.clone();
    for (int i = 0; i < roi_bg_->height; i++) { // i在y轴上迭代
        for (int j = 0; j < roi_bg_->width; j++) { // j在x轴上迭代
            int x_in_fg = j + roi_fg_->x;
            int y_in_fg = i + roi_fg_->y;
            int x_in_bg = j + roi_bg_->x;
            int y_in_bg = i + roi_bg_->y;
            gradient_patch_x_.at<cv::Vec3b>(y_in_bg, x_in_bg)[0] = gradient_fg_x_.at<cv::Vec3b>(y_in_fg, x_in_fg)[0]; // B
            gradient_patch_x_.at<cv::Vec3b>(y_in_bg, x_in_bg)[1] = gradient_fg_x_.at<cv::Vec3b>(y_in_fg, x_in_fg)[1]; // G
            gradient_patch_x_.at<cv::Vec3b>(y_in_bg, x_in_bg)[2] = gradient_fg_x_.at<cv::Vec3b>(y_in_fg, x_in_fg)[2]; // R
        }
    }

    //cv::imshow("gradient_patch_x_", gradient_patch_x_);
    //cv::imshow("gradient_fg_x_", gradient_fg_x_);
    //cv::imshow("gradient_bg_x_", gradient_bg_x_);
    //cv::waitKey(0);
    //qDebug() << "show" << endl;
    //cv::destroyAllWindows();
}

void PIE::compute_gradient_y_()
{
    cv::Mat kernel = cv::Mat::zeros(3, 1, CV_8S);
    kernel.at<char>(2, 0) = 1;
    kernel.at<char>(1, 0) = -1;

    filter2D(src_bg_, gradient_bg_y_, CV_8U, kernel); // only color image
    filter2D(src_fg_, gradient_fg_y_, CV_8U, kernel); // only color image


    // 将背景图片的roi部分梯度替换成patch的梯度
    gradient_patch_y_ = gradient_bg_y_.clone();
    for (int i = 0; i < roi_bg_->height; i++) { // i在y轴上迭代
        for (int j = 0; j < roi_bg_->width; j++) { // j在x轴上迭代
            int x_in_fg = j + roi_fg_->x;
            int y_in_fg = i + roi_fg_->y;
            int x_in_bg = j + roi_bg_->x;
            int y_in_bg = i + roi_bg_->y;
            gradient_patch_y_.at<cv::Vec3b>(y_in_bg, x_in_bg)[0] = gradient_fg_y_.at<cv::Vec3b>(y_in_fg, x_in_fg)[0]; // B
            gradient_patch_y_.at<cv::Vec3b>(y_in_bg, x_in_bg)[1] = gradient_fg_y_.at<cv::Vec3b>(y_in_fg, x_in_fg)[1]; // G
            gradient_patch_y_.at<cv::Vec3b>(y_in_bg, x_in_bg)[2] = gradient_fg_y_.at<cv::Vec3b>(y_in_fg, x_in_fg)[2]; // R
        }

    }

    //cv::imshow("gradient_patch_y_", gradient_patch_y_);
    //cv::imshow("gradient_fg_y_", gradient_fg_y_);
    //cv::imshow("gradient_bg_y_", gradient_bg_y_);
    //cv::waitKey(0);
    //qDebug() << "show" << endl;
    //cv::destroyAllWindows();
}

void PIE::blend_gradient_()
{
    cv::Mat laplacianX = cv::Mat(gradient_bg_x_.size(), CV_8UC4);
    cv::Mat laplacianY = cv::Mat(gradient_bg_y_.size(), CV_8UC4);

    laplacianX = gradient_patch_x_;
    laplacianY = gradient_patch_y_;
}

void PIE::compute_divergence_()
{
    cv::Mat laplacianX = cv::Mat(gradient_bg_x_.size(), CV_8UC4);
    cv::Mat laplacianY = cv::Mat(gradient_bg_y_.size(), CV_8UC4);

    laplacianX = gradient_patch_x_;
    laplacianY = gradient_patch_y_;

    cv::Mat kernel_x = cv::Mat::zeros(1, 3, CV_8S); // 这里为什么加个这个就能算拉普拉斯
    kernel_x.at<char>(0, 0) = -1;
    kernel_x.at<char>(0, 1) = 1;
    filter2D(laplacianX, laplacianX, CV_8U, kernel_x);

    cv::Mat kernel_y = cv::Mat::zeros(3, 1, CV_8S);
    kernel_y.at<char>(0, 0) = -1;
    kernel_y.at<char>(1, 0) = 1;
    filter2D(laplacianY, laplacianY, CV_8U, kernel_y);

    laplacian_ = laplacianX + laplacianY; // 散度
    //cv::imshow("laplacian_", laplacian_);
    //cv::waitKey(0);
    //qDebug() << "show" << endl;
    //cv::destroyAllWindows();
}

void PIE::compute_matrix_equation_()
{
    //// 声明动态大小的系数矩阵，大小为N*N, N为roi中的点数
    //SparseMatrix<double> A_sparse(roi_bg_->width * roi_bg_->height, roi_bg_->width * roi_bg_->height);

    VectorXd X_r(roi_bg_->width * roi_bg_->height);
    VectorXd X_g(roi_bg_->width * roi_bg_->height);
    VectorXd X_b(roi_bg_->width * roi_bg_->height);
    VectorXd B_r(roi_bg_->width * roi_bg_->height);
    VectorXd B_g(roi_bg_->width * roi_bg_->height);
    VectorXd B_b(roi_bg_->width * roi_bg_->height);

    int roi_start_x = roi_bg_->x;   int roi_end_x = roi_bg_->x + roi_bg_->width;
    int roi_start_y = roi_bg_->y;   int roi_end_y = roi_bg_->y + roi_bg_->height;


    for (int i = 0; i < roi_bg_->height; i++) { // 先行后列
        for (int j = 0; j < roi_bg_->width; j++) {
            int certain_row_in_A = i * roi_bg_->width + j; //代表了当前像素的序号
            if (i == 0 || j == 0 || i == roi_bg_->height - 1 || j == roi_bg_->width - 1) { // 四周
                //A_sparse.insert(certain_row_in_A, certain_row_in_A) = 1;

                // B向量该行元素的值应该为patch在该点的像素值，而不是散度，不然不符合约束条件
                B_b(certain_row_in_A) = src_bg_.at<cv::Vec3b>(i + roi_bg_->y, j + roi_bg_->x)[0]; //B
                B_g(certain_row_in_A) = src_bg_.at<cv::Vec3b>(i + roi_bg_->y, j + roi_bg_->x)[1]; //G
                B_r(certain_row_in_A) = src_bg_.at<cv::Vec3b>(i + roi_bg_->y, j + roi_bg_->x)[2]; //R

            }
            else // 内部
            {
                //A_sparse.insert(certain_row_in_A, certain_row_in_A) = -4;
                //A_sparse.insert(certain_row_in_A, certain_row_in_A - roi_bg_->width) = 1;
                //A_sparse.insert(certain_row_in_A, certain_row_in_A + roi_bg_->width) = 1;
                //A_sparse.insert(certain_row_in_A, certain_row_in_A - 1) = 1;
                //A_sparse.insert(certain_row_in_A, certain_row_in_A + 1) = 1;


                B_b(certain_row_in_A) = laplacian_fg_.at<cv::Vec3f>(i + roi_fg_->y, j + roi_fg_->x)[0]; //B
 
                B_g(certain_row_in_A) = laplacian_fg_.at<cv::Vec3f>(i + roi_fg_->y, j + roi_fg_->x)[1]; //G
                   
                B_r(certain_row_in_A) = laplacian_fg_.at<cv::Vec3f>(i + roi_fg_->y, j + roi_fg_->x)[2]; //R

            }
        }
    }



    // 稀疏求解器
    //SparseLU<SparseMatrix<double> > solver;
    //solver.compute(A_sparse);
    if (solver_->info() != Success) {
        qDebug() << "decomposition failed" << endl;
    }
    X_b = solver_->solve(B_b);
    X_g = solver_->solve(B_g);
    X_r = solver_->solve(B_r);
    

    for (int i = 0; i < roi_bg_->height; i++) { // i在y轴上迭代
        for (int j = 0; j < roi_bg_->width; j++) { // j在x轴上迭代
            int x_in_fg = j + roi_fg_->x;
            int y_in_fg = i + roi_fg_->y;
            int x_in_bg = j + roi_bg_->x;
            int y_in_bg = i + roi_bg_->y;
            int certain_row_in_X = i * roi_bg_->width + j;

            // 防止像素越界
            //qDebug() << "X_b " << X_b(certain_row_in_X) << endl;
            //qDebug() << "X_g " << X_b(certain_row_in_X) << endl;
            //qDebug() << "X_h "<< X_b(certain_row_in_X) <<endl;
            if (X_b(certain_row_in_X)>=0 && X_b(certain_row_in_X)<255)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[0] = X_b(certain_row_in_X);// +src_fg_.at<cv::Vec3b>(y_in_fg, x_in_fg)[0];
            else if(X_b(certain_row_in_X) < 0)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[0] = 0;
            else
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[0] = 255;

            if (X_g(certain_row_in_X) >= 0 && X_g(certain_row_in_X) < 255)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[1] = X_g(certain_row_in_X);
            else if (X_g(certain_row_in_X) < 0)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[1] = 0;
            else
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[1] = 255;

            if (X_r(certain_row_in_X) >= 0 && X_r(certain_row_in_X) < 255)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[2] = X_r(certain_row_in_X);
            else if (X_r(certain_row_in_X) < 0)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[2] = 0;
            else
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[2] = 255;


        }
    }

    //cv::imshow("src_bg_", src_bg_);
    //cv::waitKey(0);
    //qDebug() << "show" << endl;
    //cv::destroyAllWindows();

}

void PIE::compute_laplacian_test_()
{
    cv::Mat kernel = (cv::Mat_<float>(3, 3) << 0, 1, 0, 1, -4, 1, 0, 1, 0); //Laplace的整体方向算子

    //先转为32F
    cv::Mat src_fg_32F;
    src_fg_.convertTo(src_fg_32F, CV_32F);
    filter2D(src_fg_32F, laplacian_fg_, -1, kernel, cv::Point(-1, -1), 0, 0);

    cv::Mat src_bg_32F;
    src_bg_.convertTo(src_bg_32F, CV_32F);
    filter2D(src_bg_32F, laplacian_bg_, -1, kernel, cv::Point(-1, -1), 0, 0);


}
