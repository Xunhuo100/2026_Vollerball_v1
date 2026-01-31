
#训练YOLO模型文件

from ultralytics import YOLO
import os

# 1. 加载预训练权重（带特征提取层 + 检测头）
#model = YOLO('yolov8n.pt')
model = YOLO('yolov8n-p2.yaml')   # 仅结构，无预训练头
model.load('yolov8n.pt')

# 2. 开始“迁移学习”：只把检测头 nc 改成 1，其余层冻结策略 YOLO 自动做
model.train(
    data='date/volleyball.yaml',
    epochs=300,              # 数据少可以 50~100
    patience=30,             # 早停
    imgsz=960,
    batch=-1,                # 显存小就 8
    #multi_scale=True,        #每 10 epoch 在 ±200 px 范围内随机切换输入尺寸
    optimizer='Adam',
    lr0=0.001,
    lrf=0.01,
    warmup_epochs=3,

    # ===== 数据增强配置 =====
    hsv_h=0.015,          # 色调增强幅度(0-1)
    hsv_s= 0.7,            # 饱和度增强幅度(0-1)
    hsv_v= 0.4,            # 亮度增强幅度(0-1)

    degrees= 10.0,         # 图像旋转角度范围(-degrees, +degrees)
    translate= 0.1,        # 图像平移比例(0-1)
    scale= 0.5,            # 图像缩放比例(0-1)
    shear= 0.0,            # 图像剪切角度 

    # 高级增强技术
    mosaic= 1.0,           # 马赛克增强概率(0-1)，小目标检测关键
    mixup= 0.5,            # Mixup增强概率(0-1)
    copy_paste= 0.0,       # 复制粘贴增强概率(0-1)
        
        # ===== 损失函数权重 =====
    box=7.5,              # 边界框损失权重
    cls= 0.5,              # 分类损失权重
    dfl= 1.5,              # 分布焦点损失权重


    device=0,
    workers=4,
    name='volley_detect_p2'
)