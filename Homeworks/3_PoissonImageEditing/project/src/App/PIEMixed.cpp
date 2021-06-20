#include "PIEMixed.h"


PIEMixed::PIEMixed()
{
}

PIEMixed::PIEMixed(cv::Mat src_bg, cv::Mat src_fg, cv::Rect* roi_bg, cv::Rect* roi_fg)
{
    cv::imwrite("./src_bg.jpg", src_bg); // �޸�ͼƬ��ʽ
    cv::imwrite("./src_fg.jpg", src_fg); // �޸�ͼƬ��ʽ
    src_bg_ = cv::imread("./src_bg.jpg", -1);
    src_fg_ = cv::imread("./src_fg.jpg", -1);
    roi_bg_ = roi_bg;
    roi_fg_ = roi_fg;
    patch_ = src_fg(*roi_fg_).clone(); //��src_fg�ж�Ӧ��ROI������ȡ����¡�������Կ�¡������������ı�ԭsrc�ж�Ӧ��ROIͼ��

    // show
    qDebug() << roi_bg_->x << "  " << roi_bg_->y << "  " << roi_bg_->width << "  " << roi_bg_->height << endl;

    gradient_bg_x_ = cv::Mat::zeros(src_bg_.rows, src_bg_.cols, CV_32FC4);
    gradient_bg_y_ = cv::Mat::zeros(src_bg_.rows, src_bg_.cols, CV_32FC4);
    gradient_fg_x_ = cv::Mat::zeros(src_fg_.rows, src_fg_.cols, CV_32FC4); // foreground
    gradient_fg_y_ = cv::Mat::zeros(src_fg_.rows, src_fg_.cols, CV_32FC4);
    laplacianX_ = cv::Mat(gradient_bg_x_.size(), CV_32FC4);
    laplacianY_ = cv::Mat(gradient_bg_y_.size(), CV_32FC4);

}

cv::Mat PIEMixed::get_output_image_()
{
    compute_gradient_x_();
    compute_gradient_y_();
    blend_gradient_();
    compute_divergence_(); // �����ں�ͼ���ݶȵ�ɢ��
    //compute_laplacian_test_();
    compute_matrix_equation_();
    return src_bg_;
}

void PIEMixed::compute_gradient_x_()
{
    // ��ʼ��kernel
    cv::Mat kernel = cv::Mat::zeros(1, 3, CV_8S);
    kernel.at<char>(0, 2) = 1;
    kernel.at<char>(0, 1) = -1;

    filter2D(src_bg_, gradient_bg_x_, CV_32F, kernel); // only color image
    filter2D(src_fg_, gradient_fg_x_, CV_32F, kernel); // only color image

}

void PIEMixed::compute_gradient_y_()
{
    cv::Mat kernel = cv::Mat::zeros(3, 1, CV_8S);
    kernel.at<char>(2, 0) = 1;
    kernel.at<char>(1, 0) = -1;

    filter2D(src_bg_, gradient_bg_y_, CV_32F, kernel); // only color image
    filter2D(src_fg_, gradient_fg_y_, CV_32F, kernel); // only color image

}

void PIEMixed::blend_gradient_()
{


    // ��bg��roi��ȡ����ͼ���ݶȵĸ����
    laplacianX_ = gradient_bg_x_;
    laplacianY_ = gradient_bg_y_;
    for (int i = 0; i < roi_bg_->height; i++) { // i��y���ϵ���
        for (int j = 0; j < roi_bg_->width; j++) { // j��x���ϵ���
            int x_in_fg = j + roi_fg_->x;
            int y_in_fg = i + roi_fg_->y;
            int x_in_bg = j + roi_bg_->x;
            int y_in_bg = i + roi_bg_->y;
            int gradient_bg_b = abs(gradient_bg_x_.at<cv::Vec3f>(y_in_bg, x_in_bg)[0]) + abs(gradient_bg_y_.at<cv::Vec3f>(y_in_bg, x_in_bg)[0]);
            int gradient_bg_g = abs(gradient_bg_x_.at<cv::Vec3f>(y_in_bg, x_in_bg)[1]) + abs(gradient_bg_y_.at<cv::Vec3f>(y_in_bg, x_in_bg)[1]);
            int gradient_bg_r = abs(gradient_bg_x_.at<cv::Vec3f>(y_in_bg, x_in_bg)[2]) + abs(gradient_bg_y_.at<cv::Vec3f>(y_in_bg, x_in_bg)[2]);

            int gradient_fg_b = abs(gradient_fg_x_.at<cv::Vec3f>(y_in_fg, x_in_fg)[0]) + abs(gradient_fg_y_.at<cv::Vec3f>(y_in_fg, x_in_fg)[0]);
            int gradient_fg_g = abs(gradient_fg_x_.at<cv::Vec3f>(y_in_fg, x_in_fg)[1]) + abs(gradient_fg_y_.at<cv::Vec3f>(y_in_fg, x_in_fg)[1]);
            int gradient_fg_r = abs(gradient_fg_x_.at<cv::Vec3f>(y_in_fg, x_in_fg)[2]) + abs(gradient_fg_y_.at<cv::Vec3f>(y_in_fg, x_in_fg)[2]);

            // ���ǰ�����ݶȴ��ڱ������ݶȣ�����ǰ�����ݶ�����������ݶ�
            if (gradient_fg_b > gradient_bg_b) {
                laplacianX_.at<cv::Vec3f>(y_in_bg, x_in_bg)[0] = gradient_fg_x_.at<cv::Vec3f>(y_in_fg, x_in_fg)[0];
                laplacianY_.at<cv::Vec3f>(y_in_bg, x_in_bg)[0] = gradient_fg_y_.at<cv::Vec3f>(y_in_fg, x_in_fg)[0];
            }
            if (gradient_fg_g > gradient_bg_g) {
                laplacianX_.at<cv::Vec3f>(y_in_bg, x_in_bg)[1] = gradient_fg_x_.at<cv::Vec3f>(y_in_fg, x_in_fg)[1];
                laplacianY_.at<cv::Vec3f>(y_in_bg, x_in_bg)[1] = gradient_fg_y_.at<cv::Vec3f>(y_in_fg, x_in_fg)[1];
            }
            if (gradient_fg_r > gradient_bg_r) {
                laplacianX_.at<cv::Vec3f>(y_in_bg, x_in_bg)[2] = gradient_fg_x_.at<cv::Vec3f>(y_in_fg, x_in_fg)[2];
                laplacianY_.at<cv::Vec3f>(y_in_bg, x_in_bg)[2] = gradient_fg_y_.at<cv::Vec3f>(y_in_fg, x_in_fg)[2];
            }

        }
    }
    //cv::imshow("laplacianX_", laplacianX_);
    //cv::imshow("laplacianY_", laplacianY_);
    //cv::waitKey(0);
    //qDebug() << "show" << endl;
    //cv::destroyAllWindows();
}

void PIEMixed::compute_divergence_()
{

    cv::Mat kernel_x = cv::Mat::zeros(1, 3, CV_8S); // ����Ϊʲô�Ӹ����������������˹
    kernel_x.at<char>(0, 0) = -1;
    kernel_x.at<char>(0, 1) = 1;
    filter2D(laplacianX_, laplacianX_, CV_32F, kernel_x);

    cv::Mat kernel_y = cv::Mat::zeros(3, 1, CV_8S);
    kernel_y.at<char>(0, 0) = -1;
    kernel_y.at<char>(1, 0) = 1;
    filter2D(laplacianY_, laplacianY_, CV_32F, kernel_y);

    laplacian_ = laplacianX_ + laplacianY_; // ɢ��

    //cv::imshow("laplacian_", laplacian_);
    //cv::waitKey(0);
    //qDebug() << "show" << endl;
    //cv::destroyAllWindows();
}

void PIEMixed::compute_matrix_equation_()
{
    // ������̬��С��ϵ�����󣬴�СΪN*N, NΪroi�еĵ���
    SparseMatrix<double> A_sparse(roi_bg_->width * roi_bg_->height, roi_bg_->width * roi_bg_->height);

    VectorXd X_r(roi_bg_->width * roi_bg_->height);
    VectorXd X_g(roi_bg_->width * roi_bg_->height);
    VectorXd X_b(roi_bg_->width * roi_bg_->height);
    VectorXd B_r(roi_bg_->width * roi_bg_->height);
    VectorXd B_g(roi_bg_->width * roi_bg_->height);
    VectorXd B_b(roi_bg_->width * roi_bg_->height);

    for (int i = 0; i < roi_bg_->height; i++) { // ���к���
        for (int j = 0; j < roi_bg_->width; j++) {
            int certain_row_in_A = i * roi_bg_->width + j; //�����˵�ǰ���ص����
            if (i == 0 || j == 0 || i == roi_bg_->height - 1 || j == roi_bg_->width - 1) { // ����
                A_sparse.insert(certain_row_in_A, certain_row_in_A) = 1;

                // B��������Ԫ�ص�ֵӦ��Ϊpatch�ڸõ������ֵ��������ɢ�ȣ���Ȼ������Լ������
                B_b(certain_row_in_A) = src_bg_.at<cv::Vec3b>(i + roi_bg_->y, j + roi_bg_->x)[0]; //B
                B_g(certain_row_in_A) = src_bg_.at<cv::Vec3b>(i + roi_bg_->y, j + roi_bg_->x)[1]; //G
                B_r(certain_row_in_A) = src_bg_.at<cv::Vec3b>(i + roi_bg_->y, j + roi_bg_->x)[2]; //R

            }
            else // �ڲ�
            {
                A_sparse.insert(certain_row_in_A, certain_row_in_A) = -4;
                A_sparse.insert(certain_row_in_A, certain_row_in_A - roi_bg_->width) = 1;
                A_sparse.insert(certain_row_in_A, certain_row_in_A + roi_bg_->width) = 1;
                A_sparse.insert(certain_row_in_A, certain_row_in_A - 1) = 1;
                A_sparse.insert(certain_row_in_A, certain_row_in_A + 1) = 1;

                // �ݶ��ںϣ�ȡͬһλ��bg��fg�ĸ�����ݶ�
                B_b(certain_row_in_A) = laplacian_.at<cv::Vec3f>(i + roi_bg_->y, j + roi_bg_->x)[0]; //B

                B_g(certain_row_in_A) = laplacian_.at<cv::Vec3f>(i + roi_bg_->y, j + roi_bg_->x)[1]; //G

                B_r(certain_row_in_A) = laplacian_.at<cv::Vec3f>(i + roi_bg_->y, j + roi_bg_->x)[2]; //R

            }
        }
    }



    // ϡ�������
    SparseLU<SparseMatrix<double> > solver;
    solver.compute(A_sparse);
    if (solver.info() != Success) {
        qDebug() << "decomposition failed" << endl;
    }
    if (solver.info() != Success) {
        qDebug() << "solving failed" << endl;
    }

    X_b = solver.solve(B_b);
    X_g = solver.solve(B_g);
    X_r = solver.solve(B_r);


    for (int i = 0; i < roi_bg_->height; i++) { // i��y���ϵ���
        for (int j = 0; j < roi_bg_->width; j++) { // j��x���ϵ���
            int x_in_fg = j + roi_fg_->x;
            int y_in_fg = i + roi_fg_->y;
            int x_in_bg = j + roi_bg_->x;
            int y_in_bg = i + roi_bg_->y;
            int certain_row_in_X = i * roi_bg_->width + j;

            // ��ֹ����Խ��
            if (X_b(certain_row_in_X) >= 0 && X_b(certain_row_in_X) < 255)
                src_bg_.at<cv::Vec3b>(y_in_bg, x_in_bg)[0] = X_b(certain_row_in_X);// +src_fg_.at<cv::Vec3b>(y_in_fg, x_in_fg)[0];
            else if (X_b(certain_row_in_X) < 0)
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

    cv::imshow("src_bg_", src_bg_);
    cv::waitKey(0);
    qDebug() << "show" << endl;
    cv::destroyAllWindows();

}
