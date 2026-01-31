#include <fcntl.h>      // 文件控制
#include <termios.h>    // 终端IO
#include <unistd.h>     // UNIX标准函数
#include <cstring>      // 字符串操作
#include <regex> 
#include <iostream>  



// ==================== 串口通信类 ====================
class Inference {
public:
    Inference();
    ~Inference();
    
    // ... 现有方法 ...
    
    // ========== 添加串口相关方法 ==========
    bool openSerial(const std::string& port, int baudrate);
    void closeSerial();
    bool sendData(const std::uint8_t* data,size_t len);
    bool isSerialOpen() const;
    
private:
    // ========== 添加串口文件描述符 ==========
    int serial_fd_;  // 替换原来的 serial:
};

