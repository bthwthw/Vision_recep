#include "imu.hpp"

using namespace sl;

MyIMUData IMU::getAllData(Camera& zed) {
    MyIMUData result;
    SensorsData sensors_data;

    // Lấy dữ liệu sensor mới nhất
    if (zed.getSensorsData(sensors_data, TIME_REFERENCE::CURRENT) == ERROR_CODE::SUCCESS) {
        // Kiểm tra xem IMU có sống không [cite: 208, 221]
        if (sensors_data.imu.is_available) {
            
            result.is_valid = true;

            // --- 1. LẤY ORIENTATION (Góc nghiêng) ---
            // Từ Quaternion đổi ra Euler Angles cho dễ hiểu
            float3 euler = sensors_data.imu.pose.getRotationMatrix().getEulerAngles();
            result.roll  = euler[0];
            result.pitch = euler[1];
            result.yaw   = euler[2];

            // --- 2. LẤY LINEAR ACCELERATION (Gia tốc) ---
            // Đã được bù trừ Bias (linear_acceleration vs uncalibrated) [cite: 235, 236]
            result.ax = sensors_data.imu.linear_acceleration.x;
            result.ay = sensors_data.imu.linear_acceleration.y;
            result.az = sensors_data.imu.linear_acceleration.z;

            // --- 3. LẤY ANGULAR VELOCITY (Vận tốc góc) ---
            // Đơn vị là độ/giây [cite: 229]
            result.gx = sensors_data.imu.angular_velocity.x;
            result.gy = sensors_data.imu.angular_velocity.y;
            result.gz = sensors_data.imu.angular_velocity.z;
        }
    }
    return result;
}