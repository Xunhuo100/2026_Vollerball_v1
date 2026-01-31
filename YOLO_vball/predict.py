

from ultralytics import YOLO
from pathlib import Path

model   = YOLO('runs/detect/volley_detect_p2/weights/best.pt')
in_path = Path('infer/images')          # 图片文件夹
out_path= Path('infer')                 # 输出根目录

results = model.predict(
        source=in_path,
        save_txt=True,   # 关键开关
        project=out_path,
        )   # 子目录名
