#pragma once // Chỉ nạp file này 1 lần
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

class Person {
public:
    // Hàm khởi tạo module (Cấu hình tham số AI)
    void init(sl::Camera& zed);

    // Hàm chạy chính (Lấy ảnh, vẽ bounding box và trả về ảnh OpenCV để hiển thị)
    cv::Mat updateAndDraw(sl::Camera& zed);

    // Hàm tắt module
    void close(sl::Camera& zed);

private:
    sl::ObjectDetectionRuntimeParameters obj_runtime_param;
    sl::Objects objects;
    sl::Mat image_zed;

    // Hàm phụ trợ chuyển đổi ảnh
    cv::Mat slMat2cvMat(sl::Mat& input);
};