cmake_minimum_required(VERSION 3.5)
project(robot_main_system) # Đặt tên project là robot_main_system cho đúng bản chất

# --- CẤU HÌNH ĐƯỜNG DẪN GỐC (Thay đổi nếu bạn di chuyển thư mục project) ---
# Lấy đường dẫn hiện tại làm gốc (nơi chứa file CMakeLists.txt này)
set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Định nghĩa đường dẫn tuyệt đối đến thư mục modules
set(MODULES_DIR "${PROJECT_ROOT_DIR}/modules")

# -------------------------------------------------------------------------

set(CMAKE_BUILD_TYPE Release)

# 1. Tìm gói CUDA (Bắt buộc)
find_package(CUDA REQUIRED)

# 2. Tìm gói OpenCV (Bắt buộc)
find_package(OpenCV REQUIRED)

# 3. Cấu hình ZED (Đường dẫn tuyệt đối cứng)
set(ZED_INCLUDE_DIRS "/usr/local/zed/include")
set(ZED_LIBRARY_DIR "/usr/local/zed/lib")

# 4. Khai báo các thư mục Header (.h/.hpp)
# Include từ ZED
include_directories(${ZED_INCLUDE_DIRS})

# Include từ CUDA
include_directories(${CUDA_INCLUDE_DIRS})
include_directories("/usr/local/cuda/include")

# Include từ OpenCV (Thêm đường dẫn cứng cho Jetson)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("/usr/include/opencv4")

# --- QUAN TRỌNG: Include đường dẫn tuyệt đối của thư mục modules ---
# Giúp trong main.cpp chỉ cần viết #include "ZedVision.hpp" thay vì "modules/ZedVision.hpp"
include_directories(${MODULES_DIR})
# ------------------------------------------------------------------

# 5. Khai báo file chạy (Executable) và các file nguồn (Source files)
# Sử dụng đường dẫn tuyệt đối để trỏ tới các file .cpp
add_executable(robot_main
    "${PROJECT_ROOT_DIR}/main.cpp"
    "${MODULES_DIR}/person.cpp"
    "${MODULES_DIR}/imu.cpp"
)

# 6. Tìm thư viện ZED (.so)
find_library(ZED_LIBRARY NAMES sl_zed HINTS ${ZED_LIBRARY_DIR})

# 7. Liên kết thư viện (Link Libraries)
# Liên kết thư viện ZED
target_link_libraries(robot_main ${ZED_LIBRARY})

# Liên kết các thư viện CUDA
target_link_libraries(robot_main ${CUDA_LIBRARIES})
target_link_libraries(robot_main ${CUDA_CUDA_LIBRARY})
target_link_libraries(robot_main ${CUDA_CUDART_LIBRARY})

# Liên kết OpenCV
target_link_libraries(robot_main ${OpenCV_LIBS})

# Liên kết luồng (Thread)
target_link_libraries(robot_main -lpthread)

# 8. Cấu hình chuẩn C++ 14
set_target_properties(robot_main PROPERTIES CXX_STANDARD 14)