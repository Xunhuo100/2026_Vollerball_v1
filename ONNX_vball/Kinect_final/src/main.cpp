#include "inference.h"
#include "COM.h"
#include <regex>

int main() 
{
    int send_flag =0;
    //int time=0;//跳帧
    float Confidence=0.6;//置信度
    float confidence=0;
    cv::Rect  re_final;//最终目标球
    uint8_t date_vall[]={0xEE,0x0C,0xB0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEE};//数据帧格式


    //配置ONNX模型参数
    std::cout << "ONNXRuntime CPU version: " << Ort::GetVersionString() << std::endl;
    DCSP_CORE *yoloDetector = new DCSP_CORE;
    cv::Mat bgr;
    yoloDetector->classes = {"volleyball"}; 
    std::string model_path = "../best_640.onnx"; 
    int img_size=640; 
#ifdef USE_CUDA
    // GPU FP32推理
    DCSP_INIT_PARAM params{ 
        model_path,           // 模型路径
        YOLO_ORIGIN_V8,       // FP32模型
        {640, 640},           // 输入尺寸
        0.1,                  // 置信度阈值
        0.5,                  // NMS IoU阈值
        true                  // 启用CUDA
    };
    // GPU FP16推理（更快，需GPU支持）
    // DCSP_INIT_PARAM params{ model_path, YOLO_ORIGIN_V8_HALF, {640,640}, 0.1, 0.5, true };
#else
    // CPU推理
    DCSP_INIT_PARAM params{ 
        model_path, 
        YOLO_ORIGIN_V8, // FP32模型
        {img_size, img_size}, 
        Confidence,            // 置信度阈值
        0.5,            // NMS IoU阈值
        false ,
        3,
        8};
#endif
    yoloDetector->CreateSession(params);

    //配置串口
    const char* serialPortName = "/dev/ttyUSB0";  
    speed_t baudRate = B115200;  
    Inference inference;  // 或你的对象名
    if (!inference.openSerial(serialPortName, baudRate))
    {
        std::cerr<<"Failed open COM\n"; 
        return 1;
    }


    //配置相机
    k4a_device_t device = nullptr;
        if (k4a_device_open(K4A_DEVICE_DEFAULT, &device) != K4A_RESULT_SUCCEEDED) 
    {
        std::cerr << "Failed to open device\n";
        return 1;
    }

    k4a_device_configuration_t config = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    config.color_format             = K4A_IMAGE_FORMAT_COLOR_BGRA32;
    config.color_resolution         = K4A_COLOR_RESOLUTION_1080P;
    config.camera_fps               = K4A_FRAMES_PER_SECOND_30;
    config.depth_mode               = K4A_DEPTH_MODE_OFF;
    config.synchronized_images_only = false;

    if (k4a_device_start_cameras(device, &config) != K4A_RESULT_SUCCEEDED) 
    {
        std::cerr << "Failed to start cameras\n";
        k4a_device_close(device);
        return 1;
    }
    cv::namedWindow("Azure Kinect RGB", cv::WINDOW_AUTOSIZE);

    while(true)
    {
        //time++;
        confidence=Confidence;

        //跳帧
        // if(time==2)
        // {
        //     time=0;
        //     continue;
        // }

        //相机获取图像并预处理成BGR图像
        k4a_capture_t capture = nullptr;
        k4a_device_get_capture(device, &capture, K4A_WAIT_INFINITE);

        k4a_image_t img = k4a_capture_get_color_image(capture);
        int w  = k4a_image_get_width_pixels(img);
        int h  = k4a_image_get_height_pixels(img);
        uint8_t* data_buffer = k4a_image_get_buffer(img);   // BGRA32

        cv::Mat bgra(h, w, CV_8UC4, data_buffer);

        cv::cvtColor(bgra, bgr, cv::COLOR_BGRA2BGR);      // 去掉 alpha

        std::vector<DCSP_RESULT> res;
        yoloDetector->RunSession(bgr, res);

        for (auto &re : res) 
        {
            send_flag=1;
            std::cout << "检测到: " << yoloDetector->classes[re.classId] 
                    << " 置信度: " << re.confidence 
                    <<"  id: "<<re.classId
                    << " h: " << re.box.height 
                    << " w: " << re.box.width 
                    << " x: " << re.box.x 
                    << " y: " << re.box.y<<"\n"  ;
            
            uint16_t centre_x=re.box.x+re.box.width/2;
            uint16_t centre_y=re.box.y+re.box.height/2; 
            
            if(re.confidence>=confidence)
            {
                re_final=re.box;
                confidence=re.confidence;
                date_vall[3]=uint8_t((centre_x>>8)&0xFF);
                date_vall[4]=uint8_t((centre_x)&0xFF);
                date_vall[5]=uint8_t((centre_y>>8)&0xFF);
                date_vall[6]=uint8_t((centre_y)&0xFF);
                date_vall[7]=uint8_t((re.box.width>>8)&0xFF);
                date_vall[8]=uint8_t((re.box.width)&0xFF);
                date_vall[9]=uint8_t((re.box.height>>8)&0xFF);
                date_vall[10]=uint8_t((re.box.height)&0xFF);
            }
        
        }
        if(send_flag)
        {
            //可视化显示
            cv::rectangle(bgr,re_final,cv::Scalar(0, 0, 255), 1);
            //发送数据
            inference.sendData(date_vall,sizeof(date_vall));
            send_flag=0;
        }
        cv::imshow("Azure Kinect RGB", bgr);  // 窗口标题，图像
        
        //释放内存
        k4a_capture_release(capture);
        k4a_image_release(img); 
        if (cv::waitKey(1) == 27) break;   // ESC 退出
    }
    k4a_device_stop_cameras(device);
    k4a_device_close(device);
    cv::destroyAllWindows();  // 关闭所有窗口
    delete yoloDetector;
    return 0;
}