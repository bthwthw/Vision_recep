#include "person.hpp"
#include <iostream>

using namespace sl;
using namespace std;

void Person::init(Camera& zed) {
    // 1. Bật Positional Tracking (Bắt buộc cho Object Detection)
    PositionalTrackingParameters tracking_parameters;
    zed.enablePositionalTracking(tracking_parameters);

    // 2. Bật Object Detection
    ObjectDetectionParameters obj_param;
    obj_param.enable_tracking = true;
    obj_param.detection_model = DETECTION_MODEL::MULTI_CLASS_BOX;
    zed.enableObjectDetection(obj_param);

    // 3. Cấu hình Runtime (Độ nhạy, lọc người...)
    obj_runtime_param.detection_confidence_threshold = 70;
    obj_runtime_param.object_class_filter = {OBJECT_CLASS::PERSON};
    
    cout << "[Module Vision] Da khoi tao xong!" << endl;
}

cv::Mat Person::updateAndDraw(Camera& zed) {
    // Lấy dữ liệu vật thể
    zed.retrieveObjects(objects, obj_runtime_param);
    
    // Lấy hình ảnh (Mắt trái)
    zed.retrieveImage(image_zed, VIEW::LEFT, MEM::CPU);
    
    // Chuyển sang OpenCV
    cv::Mat image_ocv = slMat2cvMat(image_zed);

    // Vẽ vời
    if (objects.is_new) {
        for (auto& obj : objects.object_list) {
            // Lấy tọa độ
            auto top_left = obj.bounding_box_2d[0];
            auto bottom_right = obj.bounding_box_2d[2];
            cv::Point p1((int)top_left.x, (int)top_left.y);
            cv::Point p2((int)bottom_right.x, (int)bottom_right.y);

            // Tính khoảng cách
            float dist = sqrt(obj.position.x * obj.position.x + 
                              obj.position.y * obj.position.y + 
                              obj.position.z * obj.position.z);

            // Logic màu sắc
            cv::Scalar color(0, 255, 0); // Xanh
            if (dist < 1.5) color = cv::Scalar(0, 0, 255); // Đỏ

            // Vẽ khung & Chữ
            cv::rectangle(image_ocv, p1, p2, color, 2);
            string text = "ID:" + to_string(obj.id) + " | " + to_string(dist).substr(0, 3) + "m";
            cv::putText(image_ocv, text, cv::Point(p1.x, p1.y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
        }
    }
    return image_ocv; // Trả về ảnh đã vẽ để main hiển thị
}

void Person::close(Camera& zed) {
    zed.disableObjectDetection();
    zed.disablePositionalTracking();
}

// Hàm phụ trợ (Copy từ code cũ của bạn vào đây cho gọn)
cv::Mat Person::slMat2cvMat(Mat& input) {
    int cv_type = CV_8UC4; 
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM::CPU));
}