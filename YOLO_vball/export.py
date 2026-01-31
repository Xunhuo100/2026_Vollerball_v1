#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
YOLOv8 导出 TensorRT FP16 脚本
"""

from ultralytics import YOLO

# 1. 加载训练好的权重
pt_path = "runs/detect/volley_detect4/weights/best.pt"
model = YOLO(pt_path)

from ultralytics import YOLO

model.export(format="onnx", imgsz=640)
print("导出完成！ONNX文件已保存。")

# onnx_path = model.export(
#     format="onnx",
#     imgsz=640,          # 与训练分辨率一致
#     half=True,          # FP16 提速主体
#     simplify=True,      # 节点精简
#     dynamic=False,      # 静态尺寸，推理最快
#     opset=16            # 兼容主流硬件/Runtime
# )

# 2. 导出 TensorRT FP16

# engine_path = model.export(
#     format="engine",   # 固定写法
#     imgsz=640,         # 与训练分辨率保持一致
#     half=True,         # 关键：开启 FP16
#     device=0,          # GPU 卡号
#     workspace=4,       # TensorRT 编译最大工作空间(GB)
#     batch=1,           # 预设 batch，推理时≤此值即可
#     dynamic=False      # 如需动态尺寸改成 True
# )
# print("✅ TensorRT FP16 引擎已生成：", engine_path)