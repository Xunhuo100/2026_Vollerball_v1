#include"COM.h"


Inference::Inference() : serial_fd_(-1) 
{
    // ... 现有代码 ...
}

// 析构函数关闭串口
Inference::~Inference() 
{
    closeSerial();
}


// ==================== 串口类实现 ====================



// 打开串口
bool Inference::openSerial(const std::string& port, int baudrate) {
    closeSerial();  // 先关闭已打开的串口
    
    serial_fd_ = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd_ == -1) {
        std::cerr << "无法打开串口: " << port << std::endl;
        return false;
    }
    
    // 恢复阻塞模式
    fcntl(serial_fd_, F_SETFL, 0);
    
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    
    if (tcgetattr(serial_fd_, &tty) != 0) {
        std::cerr << "tcgetattr 失败" << std::endl;
        closeSerial();
        return false;
    }
    
    // 设置波特率
    speed_t baud = B115200;  // 默认
    switch (baudrate) {
        case 9600:   baud = B9600;   break;
        case 19200:  baud = B19200;  break;
        case 38400:  baud = B38400;  break;
        case 57600:  baud = B57600;  break;
        case 115200: baud = B115200; break;
        case 230400: baud = B230400; break;
        default:     baud = B115200; break;
    }
    
    cfsetospeed(&tty, baud);
    cfsetispeed(&tty, baud);
    
    // 8N1 配置
    tty.c_cflag &= ~PARENB;        // 无校验
    tty.c_cflag &= ~CSTOPB;        // 1位停止位
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;            // 8位数据
    tty.c_cflag |= CREAD | CLOCAL; // 启用接收，忽略控制线
    
    // 原始模式
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;
    
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;  // 1秒超时
    
    if (tcsetattr(serial_fd_, TCSANOW, &tty) != 0) {
        std::cerr << "tcsetattr 失败" << std::endl;
        closeSerial();
        return false;
    }
    
    std::cout << "串口打开成功: " << port << " @ " << baudrate << std::endl;
    return true;
}

// 关闭串口
void Inference::closeSerial() {
    if (serial_fd_ != -1) {
        close(serial_fd_);
        serial_fd_ = -1;
    }
}

// 发送数据
bool Inference::sendData(const std::uint8_t* data,size_t len) {
    if (serial_fd_ == -1) {
        std::cerr << "串口未打开" << std::endl;
        return false;
    }
    
    ssize_t written = write(serial_fd_, data, len);
    return written == static_cast<ssize_t>(len);  
}

// 检查串口状态
bool Inference::isSerialOpen() const {
    return serial_fd_ != -1;
}







