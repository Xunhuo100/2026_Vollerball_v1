COM.cpp是串口函数文件
inference.cpp是推理函数文件



部署模型步骤：
一、拷贝文件
1.ssh连接：
例如：
scp -r /home/banjiu/Desktop/progect/vball_final/Kinect_final rc@192.168.43.60:/home/rc/progect/ 

2.修改CMakeList.txt：
project(Kinect_final LANGUAGES CXX)————>project(文件名 LANGUAGES CXX)
3.强制删除build里的CMake生成的缓存文件和目录：
rm -rf CMakeCache.txt CMakeFiles/
4.重新编译与生成可执行文件：
cmake .. 
make

二、安装依赖库
1.kinect Azure相机：
# 1. 清理可能存在的错误仓库配置（如果之前执行过失败的操作）
sudo rm -f /etc/apt/sources.list.d/microsoft-prod.list
sudo apt-get update
# 2. 安装基础依赖
sudo apt-get install -y curl apt-transport-https software-properties-common lsb-release gnupg
# 3. 添加 Microsoft GPG 密钥
curl -sSL https://packages.microsoft.com/keys/microsoft.asc | sudo tee /etc/apt/trusted.gpg.d/microsoft.asc > /dev/null
# 4. 【关键步骤】添加 Ubuntu 20.04 (focal) 的仓库到 22.04 系统
#    因为 Microsoft 没有为 22.04 (jammy) 单独发布，但 focal 的包在 jammy 上完全兼容
echo "deb [arch=amd64] https://packages.microsoft.com/ubuntu/20.04/prod focal main" | sudo tee /etc/apt/sources.list.d/microsoft-azure kinect.list
# 5. 更新 apt 缓存
sudo apt-get update
# 6. 安装 Azure Kinect SDK（C++ 核心库和工具）
sudo apt-get install -y \
    k4a-tools \
    libk4a1.4 \
    libk4a1.4-dev
# 7. 创建 udev 规则（允许非 root 访问硬件）
sudo mkdir -p /etc/udev/rules.d
sudo tee /etc/udev/rules.d/99-k4a.rules > /dev/null << 'EOF'
# Azure Kinect DK USB 设备权限规则
SUBSYSTEM=="usb", ATTRS{idVendor}=="045e", ATTRS{idProduct}=="097a", MODE="0666", GROUP="video"
SUBSYSTEM=="usb", ATTRS{idVendor}=="045e", ATTRS{idProduct}=="097b", MODE="0666", GROUP="video"
SUBSYSTEM=="usb", ATTRS{idVendor}=="045e", ATTRS{idProduct}=="097c", MODE="0666", GROUP="video"
SUBSYSTEM=="usb", ATTRS{idVendor}=="045e", ATTRS{idProduct}=="097d", MODE="0666", GROUP="video"
SUBSYSTEM=="usb", ATTRS{idVendor}=="045e", ATTRS{idProduct}=="097e", MODE="0666", GROUP="video"
EOF
# 8. 重新加载 udev 规则
sudo udevadm control --reload-rules && sudo udevadm trigger
# 9. 验证安装 - 查看版本号
echo "=== 验证 k4a-tools 安装 ==="
k4aviewer --version
k4arecorder --version
echo "=== 查看库文件位置 ==="
dpkg -L libk4a1.4 | grep ".so"

安装完成后：
# 1. 将当前用户添加到 video 组（避免每次用 sudo）
sudo usermod -a -G video $USER
# 需要重新登录或执行以下命令使权限生效：
newgrp video
# 2. 连接相机后，测试设备是否识别
k4aviewer
# 3. 手动指定库路径（如果下一步找不到库）
echo 'export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH' >> ~/.bashrc
source ~/.bashrc

2.onnxruntime：
# 1. 安装系统基础依赖
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    unzip \
    libgomp1
# 2. 下载并安装 ONNX Runtime C++ 库
mkdir -p ~/onnx_install && cd ~/onnx_install
# 下载最新稳定版 CPU 版本（v1.16.3）
wget https://github.com/microsoft/onnxruntime/releases/download/v1.20.1/onnxruntime-linux-x64-1.20.1.tgz
# 解压到系统目录
sudo tar -xzf onnxruntime-linux-x64-1.20.1.tgz --strip-components=1 -C /usr/local
# 创建符号链接（确保版本兼容性）
sudo ln -sf /usr/local/lib/libonnxruntime.so.1.20.1 / usr/local/lib/libonnxruntime.so.1
sudo ln -sf /usr/local/lib/libonnxruntime.so.1 /usr/local/lib/libonnxruntime.so
# 更新动态链接库缓存
sudo ldconfig
# 3. 验证安装
echo "=== 检查头文件 ==="
ls -la /usr/local/include/onnxruntime_cxx_api.h
echo "=== 检查库文件 ==="
ls -la /usr/local/lib/libonnxruntime.so*
echo "=== 检查版本 ==="
strings /usr/local/lib/libonnxruntime.so.1.20.1 | grep "1.20.1"


三、.检测串口名称
1.彻底禁用 brltty：
sudo systemctl stop brltty-udev.service
sudo systemctl disable brltty-udev.service
sudo systemctl mask brltty-udev.service
2.物理插拔确定串口名称：
sudo dmesg -w（看日志）
例如：
[ 1490.654899] usb 3-1: USB disconnect, device number 9
[ 1490.655195] ch341-uart ttyUSB0: ch341-uart converter now disconnected from ttyUSB0
[ 1490.655221] ch341 3-1:1.0: device disconnected
[ 1492.522352] usb 3-1: new full-speed USB device number 10 using xhci_hcd
[ 1492.662715] usb 3-1: New USB device found, idVendor=1a86, idProduct=7523, bcdDevice= 2.64
[ 1492.662719] usb 3-1: New USB device strings: Mfr=0, Product=2, SerialNumber=0
[ 1492.662722] usb 3-1: Product: USB Serial
[ 1492.667761] ch341 3-1:1.0: ch341-uart converter detected
[ 1492.681831] usb 3-1: ch341-uart converter now attached to ttyUSB0
初步确定串口名称为/dev/ttyUSB0
ls -l 串口名称
例如：
ls -l /dev/ttyUSB0
crw-rw---- 1 root dialout 188, 0  1月 29 14:23 /dev/ttyUSB0
则可确定串口名称为/dev/ttyUSB0，该如main函数里的串口初始化配置
3.把用户加进 dia

四、修改使用的模型
修改模型路径
修改模型输入数据大小
修改CPU线程数：
取线程数的0.25倍左右
echo "逻辑线程数: $(nproc 2>/dev/null || cat /proc/cpuinfo | grep -c processor)"
echo "CPU型号: $(cat /proc/cpuinfo | grep 'model name' | head -1 | cut -d':' -f2)"
