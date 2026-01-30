#pragma once
#include <sl/Camera.hpp>

// 1. Tạo cái giỏ chứa dữ liệu để gửi về Main
struct MyIMUData {
    bool is_valid = false;

    // A. Orientation (Góc nghiêng - Độ)
    float roll, pitch, yaw;

    // B. Linear Acceleration (Gia tốc tuyến tính - m/s2)
    float ax, ay, az;

    // C. Angular Velocity (Vận tốc xoay - độ/s)
    float gx, gy, gz;
};

// 2. Class xử lý
class IMU {
public:
    // Hàm này sẽ trả về cái giỏ dữ liệu trên
    MyIMUData getAllData(sl::Camera& zed);
};