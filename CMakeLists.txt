cmake_minimum_required(VERSION 3.5)
project(robot_vision)

set(CMAKE_BUILD_TYPE Release)

# 1. Tìm gói CUDA (Bắt buộc)
find_package(CUDA REQUIRED)

# 2. Tìm gói OpenCV (QUAN TRỌNG: Mới thêm cái này)
find_package(OpenCV REQUIRED)

# 3. Cấu hình ZED
set(ZED_INCLUDE_DIRS "/usr/local/zed/include")
set(ZED_LIBRARY_DIR "/usr/local/zed/lib")

# 4. Khai báo thư mục Header (.h)
include_directories(${ZED_INCLUDE_DIRS})
include_directories(${CUDA_INCLUDE_DIRS})
include_directories("/usr/local/cuda/include")

# --- QUAN TRỌNG: Chỉ định đường dẫn OpenCV cho Jetson ---
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories("/usr/include/opencv4") 
# --------------------------------------------------------

add_executable(robot_vision main.cpp)

# 5. Tìm thư viện ZED
find_library(ZED_LIBRARY NAMES sl_zed HINTS ${ZED_LIBRARY_DIR})

# 6. Liên kết thư viện (Link Libraries)
target_link_libraries(robot_vision ${ZED_LIBRARY})
target_link_libraries(robot_vision ${CUDA_LIBRARIES})
target_link_libraries(robot_vision ${CUDA_CUDA_LIBRARY})
target_link_libraries(robot_vision ${CUDA_CUDART_LIBRARY})
target_link_libraries(robot_vision -lpthread)

# --- QUAN TRỌNG: Liên kết OpenCV vào file chạy ---
target_link_libraries(robot_vision ${OpenCV_LIBS})
# -------------------------------------------------

set_target_properties(robot_vision PROPERTIES CXX_STANDARD 14)