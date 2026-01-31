# from onnxruntime.quantization import quantize_dynamic, QuantType

# # 输入输出路径
# fp32_onnx = "/home/banjiu/Desktop/progect/YOLO/vball/runs/detect/volley_detect4/weights/best.onnx"      # 或 best.onnx
# int8_onnx = "best_640_int8.onnx"  # 或 best_int8.onnx

# quantize_dynamic(
#     model_input=fp32_onnx,
#     model_output=int8_onnx,
#     weight_type=QuantType.QInt8,   # 权重 INT8
#           # 顺带图优化
# )
# print("INT8 量化完成 →", int8_onnx)


import onnx
from onnx import numpy_helper, helper
import numpy as np

FP32_MODEL = "/home/banjiu/Desktop/progect/YOLO/vball/runs/detect/volley_detect4/weights/best.onnx"           # 输入
FP16_MODEL = "best_640_fp16.onnx"  # 输出

print(f"加载: {FP32_MODEL}")
model = onnx.load(FP32_MODEL)

converted = 0

# 只转权重（initializer），不转输入输出和中间特征
for init in model.graph.initializer:
    if init.data_type == onnx.TensorProto.FLOAT:
        arr = numpy_helper.to_array(init)
        # 跳过太小的权重（避免精度损失）
        if arr.size < 32:  # 小于32个元素的保持FP32
            continue
        arr_fp16 = arr.astype(np.float16)
        new_init = numpy_helper.from_array(arr_fp16, init.name)
        init.CopyFrom(new_init)
        converted += 1

print(f"转换权重: {converted} 个")

# 关键：输入输出保持 FP32，不改动 value_info
# 这样 Resize 等算子收到的是 FP32，但计算时用 FP16 权重

onnx.save(model, FP16_MODEL)
print(f"✓ 保存: {FP16_MODEL}")

# 验证
import os
f32 = os.path.getsize(FP32_MODEL) / 1024 / 1024
f16 = os.path.getsize(FP16_MODEL) / 1024 / 1024
print(f"大小: {f32:.2f}MB → {f16:.2f}MB ({f32/f16:.2f}x)")