#验证数据集

from ultralytics import YOLO

model = YOLO('runs/volley_detect/weights/best.pt')
metrics = model.val(
    data='data.yaml', 
    imgsz=640, 
    device=0)  # GPU-0
print('mAP50=', metrics.box.map50)
print('mAP50-95=', metrics.box.map)