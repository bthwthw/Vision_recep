// main.cpp - PHIÊN BẢN CÓ GIAO DIỆN OPENCV
#include <sl/Camera.hpp>
#include <iostream>
#include <cmath>
// Thêm thư viện OpenCV
#include <opencv2/opencv.hpp>

using namespace sl;
using namespace std;

// Hàm chuyển đổi từ sl::Mat (ZED) sang cv::Mat (OpenCV)
// Vì ZED dùng GPU/CPU memory riêng, ta cần ánh xạ dữ liệu qua
cv::Mat slMat2cvMat(Mat& input) {
    // Lưu ý: ZED SDK trả về định dạng BGRA (4 kênh màu)
    int cv_type = CV_8UC4; 
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM::CPU));
}

int main(int argc, char **argv) {
    // 1. KHỞI TẠO CAMERA
    Camera zed;
    InitParameters init_parameters;
    init_parameters.camera_resolution = RESOLUTION::VGA; // Giữ VGA cho nhẹ
    init_parameters.camera_fps = 30; // 30 FPS nhìn cho mượt
    init_parameters.depth_mode = DEPTH_MODE::PERFORMANCE;
    init_parameters.coordinate_units = UNIT::METER;
    init_parameters.coordinate_system = COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;

    ERROR_CODE err = zed.open(init_parameters);
    if (err != ERROR_CODE::SUCCESS) {
        cout << "Loi: Khong mo duoc Camera (" << err << ")" << endl;
        return 1;
    }

    // 2. BẬT TRACKING & DETECTION
    PositionalTrackingParameters tracking_parameters;
    zed.enablePositionalTracking(tracking_parameters);

    ObjectDetectionParameters obj_param;
    obj_param.enable_tracking = true;
    obj_param.detection_model = DETECTION_MODEL::MULTI_CLASS_BOX;

    zed.enableObjectDetection(obj_param);

    ObjectDetectionRuntimeParameters obj_runtime_param;
    obj_runtime_param.detection_confidence_threshold = 70;
    obj_runtime_param.object_class_filter.push_back(OBJECT_CLASS::PERSON);

    Objects objects;
    
    // Tạo biến ảnh ZED và ảnh OpenCV
    sl::Mat image_zed; 
    cv::Mat image_ocv;
    
    // Tên cửa sổ
    string window_name = "ZED Robot Vision";
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

    cout << ">>> ROBOT VISION GUI STARTED - Nhan 'q' de thoat <<<" << endl;

    char key = ' ';
    while (key != 'q') {
        if (zed.grab() == ERROR_CODE::SUCCESS) {
            // 1. Lấy dữ liệu vật thể
            zed.retrieveObjects(objects, obj_runtime_param);
            
            // 2. Lấy hình ảnh từ Camera (Mắt trái)
            zed.retrieveImage(image_zed, VIEW::LEFT, MEM::CPU); // Lấy về RAM CPU
            
            // 3. Chuyển sang định dạng OpenCV để vẽ
            image_ocv = slMat2cvMat(image_zed);

            // 4. Duyệt qua danh sách người và vẽ
            if (objects.is_new) {
                for (auto& obj : objects.object_list) {
                    // Lấy tọa độ hộp 2D (Bounding Box)
                    // ZED trả về 4 điểm: [0] Top-Left, [1] Top-Right, [2] Bottom-Right, [3] Bottom-Left
                    auto top_left = obj.bounding_box_2d[0];
                    auto bottom_right = obj.bounding_box_2d[2];
                    
                    // Chuyển sang Point của OpenCV
                    cv::Point p1((int)top_left.x, (int)top_left.y);
                    cv::Point p2((int)bottom_right.x, (int)bottom_right.y);

                    // Tính khoảng cách
                    float dist = sqrt(obj.position.x * obj.position.x + 
                                      obj.position.y * obj.position.y + 
                                      obj.position.z * obj.position.z);

                    // --- VẼ LÊN HÌNH ---
                    // Màu sắc: Xanh lá (BGR: 0, 255, 0)
                    cv::Scalar color(0, 255, 0);
                    
                    // Nếu gần quá (< 1.5m) thì đổi màu Đỏ cảnh báo
                    if (dist < 1.5) color = cv::Scalar(0, 0, 255);

                    // Vẽ hình chữ nhật
                    cv::rectangle(image_ocv, p1, p2, color, 2);

                    // Vẽ chữ (ID + Distance)
                    string text = "ID:" + to_string(obj.id) + " | " + to_string(dist).substr(0, 3) + "m";
                    
                    // Vẽ nền đen cho chữ dễ đọc
                    cv::rectangle(image_ocv, cv::Point(p1.x, p1.y - 20), cv::Point(p1.x + 150, p1.y), color, cv::FILLED);
                    cv::putText(image_ocv, text, cv::Point(p1.x, p1.y - 5), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
                }
            }

            // 5. Hiển thị lên màn hình
            cv::imshow(window_name, image_ocv);
            
            // Chờ 10ms để bắt phím bấm (quan trọng để cửa sổ không bị treo)
            key = cv::waitKey(10);
        }
    }

    // Dọn dẹp
    zed.disableObjectDetection();
    zed.disablePositionalTracking();
    zed.close();
    cv::destroyAllWindows(); // Đóng cửa sổ
    return 0;
}