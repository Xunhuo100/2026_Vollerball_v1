
#读取摄像头验证

import cv2
from ultralytics import YOLO
from pathlib import Path

# ---------------- 可改区域 ----------------
SAVE_VIDEO = False                       # 是否保存录像
SAVE_PATH  = Path(__file__).parent / 'runs' / 'camera' / 'cam_demo.mp4'
CAM_INDEX  = 0                          # 0=内置，1=外接
CONF_THRES = 0.25                       # 置信度阈值
# ------------------------------------------

BEST_PT = Path(__file__).with_name('runs') / 'detect' / 'volley_detect' / 'weights' / 'best.pt'

model = YOLO(str(BEST_PT))

cap = cv2.VideoCapture(CAM_INDEX)
if not cap.isOpened():
    raise RuntimeError('无法打开摄像头')

fps = int(cap.get(cv2.CAP_PROP_FPS)) or 30
w   = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
h   = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))

# 视频写入器（仅当 SAVE_VIDEO=True 时生效）
if SAVE_VIDEO:
    SAVE_PATH.parent.mkdir(parents=True, exist_ok=True)
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    out    = cv2.VideoWriter(str(SAVE_PATH), fourcc, fps, (w, h))
    print(f'[Info] 录制中 → {SAVE_PATH}')

print('按 q 退出')
while True:
    ret, frame = cap.read()
    if not ret:
        break

    # 推理
    results = model(frame, conf=CONF_THRES, stream=True)
    for r in results:
        annotated = r.plot()

    # 实时显示
    cv2.imshow('YOLOv8 - volleyball  (press q to quit)', annotated)

    # 保存帧
    if SAVE_VIDEO:
        out.write(annotated)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 清理
cap.release()
if SAVE_VIDEO:
    out.release()
    print(f'[Info] 已保存到 {SAVE_PATH}')
cv2.destroyAllWindows()