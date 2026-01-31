import numpy
import onnxruntime as rt
import numpy as np


input_data = np.random.randn(1, 3, 640, 640).astype(np.float32)
sess = rt.InferenceSession(
    "/home/banjiu/Desktop/progect/YOLO/vball/runs/detect/volley_detect4/weights/best.onnx", providers=rt.get_available_providers())
input_name = sess.get_inputs()[0].name
label_name = sess.get_outputs()[0].name
pred_onx = sess.run(
    [label_name], {input_name: input_data.astype(numpy.float32)})[0]
print(pred_onx)