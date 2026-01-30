#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

// Gọi 2 module vào
#include "modules/person.hpp"
#include "modules/IMU.hpp"

using namespace sl;
using namespace std;

int main(int argc, char **argv) {
    // 1. KHỞI TẠO CAMERA (CHỈ LÀM 1 LẦN Ở ĐÂY)
    Camera zed;
    InitParameters init_parameters;
    init_parameters.camera_resolution = RESOLUTION::VGA; 
    init_parameters.camera_fps = 30;
    init_parameters.depth_mode = DEPTH_MODE::PERFORMANCE;
    init_parameters.coordinate_units = UNIT::METER;
    init_parameters.coordinate_system = COORDINATE_SYSTEM::RIGHT_HANDED_Y_UP;

    ERROR_CODE err = zed.open(init_parameters);
    if (err != ERROR_CODE::SUCCESS) {
        cout << "Loi Camera: " << err << endl;
        return 1;
    }

    // 2. KHỞI TẠO CÁC MODULE CON
    person myPerson;
    IMU myIMU;

    // Cài đặt thông số cho Vision (truyền biến zed vào để nó setup)
    myPerson.init(zed);

    // 3. VÒNG LẶP CHÍNH
    string window_name = "Robot Control Center";
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    
    cout << "--- ROBOT STARTED ---" << endl;

    char key = ' ';
    while (key != 'q') {
        if (zed.grab() == ERROR_CODE::SUCCESS) {
            
            // --- GIAO VIỆC 1: Xử lý hình ảnh & Nhận diện ---
            cv::Mat display_img = myPerson.updateAndDraw(zed);

            // --- GIAO VIỆC 2: Đọc cảm biến IMU ---
            MyIMUData imuData = myIMU.getAllData(zed);

            // In ra màn hình console (hoặc gửi xuống mạch điều khiển robot)
            if (imuData.is_valid) {
                // Xóa màn hình cũ in đè lên cho đẹp (ANSI code)
                cout << "\033[2J\033[1;1H"; 
                cout << "=== ROBOT SENSOR STATUS ===" << endl;
                
                cout << "[GOC NGHIENG] Roll: " << imuData.roll 
                     << " | Pitch: " << imuData.pitch 
                     << " | Yaw: " << imuData.yaw << endl;

                cout << "[GIA TOC m/s2] X: " << imuData.ax 
                     << " | Y: " << imuData.ay 
                     << " | Z: " << imuData.az << endl;

                cout << "[VAN TOC XOAY d/s]  X: " << imuData.gx 
                     << " | Y: " << imuData.gy 
                     << " | Z: " << imuData.gz << endl;
            }

            // --- HIỂN THỊ KẾT QUẢ ---
            if (!display_img.empty()) {
                cv::imshow(window_name, display_img);
            }
            
            key = cv::waitKey(10);
        }
    }

    // 4. DỌN DẸP
    myPerson.close(zed);
    zed.close();
    return 0;
}