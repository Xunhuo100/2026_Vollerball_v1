#pragma once

#define    RET_OK nullptr

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#include <io.h>
#endif

#include <chrono>
#include <string>
#include <vector>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include "onnxruntime_cxx_api.h"

#include <k4a/k4a.h>
#include <iostream>
#include <k4a/k4a.hpp>
#include <chrono>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <regex> 

#include <fcntl.h>      // 文件控制
#include <termios.h>    // 终端IO
#include <unistd.h>     // UNIX标准函数
#include <cstring>      // 字符串操作

#ifdef USE_CUDA
#include <cuda_fp16.h>
#endif


enum MODEL_TYPE {
    //FLOAT32 MODEL
    YOLO_ORIGIN_V5 = 0,
    YOLO_ORIGIN_V8 = 1,//only support v8 detector currently// FP32推理
    YOLO_POSE_V8 = 2,
    YOLO_CLS_V8 = 3,
    YOLO_ORIGIN_V8_HALF = 4,
    YOLO_POSE_V8_HALF = 5,
    YOLO_CLS_V8_HALF = 6
};


typedef struct _DCSP_INIT_PARAM {
    std::string ModelPath;// ONNX模型路径
    MODEL_TYPE ModelType = YOLO_ORIGIN_V8;// 模型类型
    std::vector<int> imgSize = {640, 640};// 输入图像尺寸
    float RectConfidenceThreshold = 0.6;// 置信度阈值
    float iouThreshold = 0.5;// NMS IoU阈值
    bool CudaEnable = false;// 是否启用CUDA
    int LogSeverityLevel = 3;
    int IntraOpNumThreads = 1;
} DCSP_INIT_PARAM;


typedef struct _DCSP_RESULT {
    int classId;   // 类 别ID
    float confidence;// 置信度
    cv::Rect box;  // 边界框
} DCSP_RESULT;


class DCSP_CORE {
public:
    DCSP_CORE();

    ~DCSP_CORE();

public:
    char *CreateSession(DCSP_INIT_PARAM &iParams);

    char *RunSession(cv::Mat &iImg, std::vector<DCSP_RESULT> &oResult);

    template<typename N>
    char *TensorProcess(std::chrono::high_resolution_clock::time_point &starttime_1, cv::Mat &iImg, N &blob, std::vector<int64_t> &inputNodeDims,
                        std::vector<DCSP_RESULT> &oResult);

    std::vector<std::string> classes{};

private:
    Ort::Env env;
    Ort::Session *session;
    bool cudaEnable;
    Ort::RunOptions options;
    std::vector<const char *> inputNodeNames;
    std::vector<const char *> outputNodeNames;

    MODEL_TYPE modelType;
    std::vector<int> imgSize;
    float rectConfidenceThreshold;
    float iouThreshold;
};
