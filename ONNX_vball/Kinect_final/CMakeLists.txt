
 cmake_minimum_required(VERSION 3.10)
project(Kinect_final LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")


# ----------- OpenCV -----------
find_package(OpenCV REQUIRED)


# ----------- Azure Kinect -----------
find_library(K4A_LIB k4a PATHS /usr/lib/x86_64-linux-gnu)
find_path(K4A_INCLUDE_DIR k4a/k4a.h PATHS /usr/include)
if(NOT K4A_LIB OR NOT K4A_INCLUDE_DIR)
    message(FATAL_ERROR "Azure Kinect SDK not found! sudo apt install libk4a1.4 libk4a1.4-dev")
endif()



# ----------- ONNX Runtime -----------
set(ONNXRUNTIME_ROOT "/opt/onnxruntime" CACHE PATH "ORT path")
find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
          PATHS ${ONNXRUNTIME_ROOT}/include)
find_library(ONNXRUNTIME_LIB onnxruntime
             PATHS ${ONNXRUNTIME_ROOT}/lib)
if(NOT ONNXRUNTIME_INCLUDE_DIR OR NOT ONNXRUNTIME_LIB)
    message(FATAL_ERROR "ONNX Runtime not found in ${ONNXRUNTIME_ROOT}")
endif()


# ----------- 可执行文件 -----------
add_executable(main 
                src/main.cpp
                src/inference.cpp
                src/COM.cpp
                    )
target_include_directories(main PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${K4A_INCLUDE_DIR}
    ${ONNXRUNTIME_INCLUDE_DIR})
target_link_libraries(main PRIVATE
    ${OpenCV_LIBS}
    ${K4A_LIB}
    ${ONNXRUNTIME_LIB} 
    pthread)

