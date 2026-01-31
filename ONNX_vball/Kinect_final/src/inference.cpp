#include"inference.h"

#define benchmark

DCSP_CORE::DCSP_CORE() 
{

}


DCSP_CORE::~DCSP_CORE() 
{
    delete session;
}


#ifdef USE_CUDA
namespace Ort
{
    template<>
    struct TypeToTensorType<half> { static constexpr ONNXTensorElementDataType type = ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16; };
}
#endif

//预处理函数：归一化
template<typename T>
char *BlobFromImage(cv::Mat &iImg, T &iBlob) 
{
    int channels = iImg.channels();
    int imgHeight = iImg.rows;
    int imgWidth = iImg.cols;

    for (int c = 0; c < channels; c++) 
    {
        for (int h = 0; h < imgHeight; h++) 
        {
            for (int w = 0; w < imgWidth; w++) 
            {
                iBlob[c * imgWidth * imgHeight + h * imgWidth + w] = typename std::remove_pointer<T>::type(
                        (iImg.at<cv::Vec3b>(h, w)[c]) / 255.0f);
            }
        }
    }
    return RET_OK;
}

//预处理函数：格式转换
char *PostProcess(cv::Mat &iImg, std::vector<int> iImgSize, cv::Mat &oImg) 
{
    cv::Mat img = iImg.clone();
    cv::resize(iImg, oImg, cv::Size(iImgSize.at(0), iImgSize.at(1)));
    if (img.channels() == 1) 
    {
        cv::cvtColor(oImg, oImg, cv::COLOR_GRAY2BGR);
    }
    cv::cvtColor(oImg, oImg, cv::COLOR_BGR2RGB);
    return RET_OK;
}

//创建模型
char *DCSP_CORE::CreateSession(DCSP_INIT_PARAM &iParams) 
{
    char *Ret = RET_OK;
    std::regex pattern("[\u4e00-\u9fa5]");
    bool result = std::regex_search(iParams.ModelPath, pattern);
    if (result) 
    {
        Ret = "[DCSP_ONNX]:Model path error.Change your model path without chinese characters.";
        std::cout << Ret << std::endl;
        return Ret;
    }
    try 
    {
        rectConfidenceThreshold = iParams.RectConfidenceThreshold;
        iouThreshold = iParams.iouThreshold;
        imgSize = iParams.imgSize;
        modelType = iParams.ModelType;
        env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "Yolo");
        Ort::SessionOptions sessionOption;
        if (iParams.CudaEnable) 
        {
            cudaEnable = iParams.CudaEnable;
            OrtCUDAProviderOptions cudaOption;
            cudaOption.device_id = 0;
            sessionOption.AppendExecutionProvider_CUDA(cudaOption);
        }
        sessionOption.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
        sessionOption.SetIntraOpNumThreads(iParams.IntraOpNumThreads);
        sessionOption.SetLogSeverityLevel(iParams.LogSeverityLevel);


        const char *modelPath = iParams.ModelPath.c_str();

        session = new Ort::Session
        (
            env, 
            modelPath, 
            sessionOption
        );//创建会话

        Ort::AllocatorWithDefaultOptions allocator;
        size_t inputNodesNum = session->GetInputCount();
        for (size_t i = 0; i < inputNodesNum; i++) 
        {
            Ort::AllocatedStringPtr input_node_name = session->GetInputNameAllocated(i, allocator);
            char *temp_buf = new char[50];
            strcpy(temp_buf, input_node_name.get());
            inputNodeNames.push_back(temp_buf);
        }
        size_t OutputNodesNum = session->GetOutputCount();
        for (size_t i = 0; i < OutputNodesNum; i++) 
        {
            Ort::AllocatedStringPtr output_node_name = session->GetOutputNameAllocated(i, allocator);
            char *temp_buf = new char[10];
            strcpy(temp_buf, output_node_name.get());
            outputNodeNames.push_back(temp_buf);
        }//获取保存输出输入点名称
        options = Ort::RunOptions{nullptr};

        return RET_OK;
    }
    catch (const std::exception &e) 
    {
        const char *str1 = "[DCSP_ONNX]:";
        const char *str2 = e.what();
        std::string result = std::string(str1) + std::string(str2);
        char *merged = new char[result.length() + 1];
        std::strcpy(merged, result.c_str());
        std::cout << merged << std::endl;
        delete[] merged;
        return "[DCSP_ONNX]:Create session failed.";
    }

}


//关键启动函数
char *DCSP_CORE::RunSession(cv::Mat &iImg, std::vector<DCSP_RESULT> &oResult) 
{
#ifdef benchmark
    auto starttime_1 = std::chrono::high_resolution_clock::now();
#endif // benchmark

    char *Ret = RET_OK;
    cv::Mat processedImg;
    PostProcess(iImg, imgSize, processedImg);
    if (modelType < 4) 
    {
        float *blob = new float[processedImg.total() * 3];
        BlobFromImage(processedImg, blob);
        std::vector<int64_t> inputNodeDims = {1, 3, imgSize.at(0), imgSize.at(1)};
        TensorProcess(starttime_1, iImg, blob, inputNodeDims, oResult);
    } 
    else 
    {
#ifdef USE_CUDA
        half* blob = new half[processedImg.total() * 3];
        BlobFromImage(processedImg, blob);
        std::vector<int64_t> inputNodeDims = { 1,3,imgSize.at(0),imgSize.at(1) };
        TensorProcess(starttime_1, iImg, blob, inputNodeDims, oResult);
#endif
    }

    return Ret;
}

//推理函数
template<typename N>
char *DCSP_CORE::TensorProcess(std::chrono::high_resolution_clock::time_point &starttime_1, cv::Mat &iImg, N &blob, std::vector<int64_t> &inputNodeDims,
                               std::vector<DCSP_RESULT> &oResult) 
{
    Ort::Value inputTensor = Ort::Value::CreateTensor<typename std::remove_pointer<N>::type>
    (
            Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU), 
            blob, 
            3 * imgSize.at(0) * imgSize.at(1),
            inputNodeDims.data(), 
            inputNodeDims.size()
    );
#ifdef benchmark
    auto starttime_2 = std::chrono::high_resolution_clock::now();
#endif // benchmark
    auto outputTensor = session->Run
    (
        options, 
        inputNodeNames.data(), 
        &inputTensor, 
        1, 
        outputNodeNames.data(),
        outputNodeNames.size()
    );
#ifdef benchmark
    auto starttime_3 = std::chrono::high_resolution_clock::now();
#endif // benchmark

    Ort::TypeInfo typeInfo = outputTensor.front().GetTypeInfo();
    auto tensor_info = typeInfo.GetTensorTypeAndShapeInfo();
    std::vector<int64_t> outputNodeDims = tensor_info.GetShape();
    auto output = outputTensor.front().GetTensorMutableData<typename std::remove_pointer<N>::type>();
    delete blob;
    switch (modelType) 
    {
        case 1://V8_ORIGIN_FP32
        case 4://V8_ORIGIN_FP16
        {
            int strideNum = outputNodeDims[2];//8400
            int signalResultNum= outputNodeDims[1];//5
            std::vector<int> class_ids;
            std::vector<float> confidences;
            std::vector<cv::Rect> boxes;
            cv::Mat rowData
            (
                signalResultNum, 
                strideNum, 
                CV_32F, 
                output
            );
            rowData = rowData.t();

            float *data = (float *) rowData.data;

            float x_factor = iImg.cols / (float)imgSize.at(0);
            float y_factor = iImg.rows / (float)imgSize.at(0);
            //遍历所有Anchor点（核心循环）
            for (int i = 0; i < strideNum; ++i) 
            {
                float *classesScores = data + 4;
                cv::Mat scores// 创建1×80的Mat，直接引用类别分数内存
                (
                    1, 
                    this->classes.size(), 
                    CV_32FC1, 
                    classesScores
                );
                cv::Point class_id;
                double maxClassScore;
                cv::minMaxLoc // 找最大值位置和数值
                (
                    scores, 
                    0, 
                    &maxClassScore, 
                    0, 
                    &class_id
                );
                if (maxClassScore > rectConfidenceThreshold) 
                {
                    confidences.push_back(maxClassScore);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);

                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.emplace_back(left, top, width, height);
                }
                data += signalResultNum;
            }

            std::vector<int> nmsResult;
            cv::dnn::NMSBoxes
            (
                boxes, 
                confidences, 
                rectConfidenceThreshold, 
                iouThreshold, 
                nmsResult
            );

            for (int i = 0; i < nmsResult.size(); ++i) 
            {
                int idx = nmsResult[i];
                DCSP_RESULT result;
                result.classId = class_ids[idx];
                result.confidence = confidences[idx];
                result.box = boxes[idx];
                oResult.push_back(result);
            }


#ifdef benchmark
            auto starttime_4 = std::chrono::high_resolution_clock::now();
            // double pre_process_time = (double) (starttime_2 - starttime_1) / CLOCKS_PER_SEC * 1000;
            // double process_time = (double) (starttime_3 - starttime_2) / CLOCKS_PER_SEC * 1000;
            // double post_process_time = (double) (starttime_4 - starttime_3) / CLOCKS_PER_SEC * 1000;
            double pre_process_time = std::chrono::duration<double, std::milli>(starttime_2 - starttime_1).count();
            double process_time = std::chrono::duration<double, std::milli>(starttime_3 - starttime_2).count();
            double post_process_time = std::chrono::duration<double, std::milli>(starttime_4 - starttime_3).count();
            if (cudaEnable) 
            {
                std::cout << "[DCSP_ONNX(CUDA)]: " << pre_process_time << "ms pre, " << process_time
                          << "ms inf, " << post_process_time << "ms post." << std::endl;
            } else 
            {
                std::cout << "[DCSP_ONNX(CPU)]: " << pre_process_time << "ms pre, " << process_time
                          << "ms infe, " << post_process_time << "ms post." << std::endl;
            }
#endif // benchmark

            break;
        }
    }
    return RET_OK;
}
