训练YOLO流程
（几乎每一步都需要修改py代码里面的路径）

date：是YOLO的预训练集.yaml文件，需要指明需要训练的train、val地址，物品类别

dateset：存放训练数据
        img——图片，
        json——labelme标注文件，
        labels——YOLO可识别txt文件

infer：存放识别结果
        img——图片，
        json——labelme标注文件，
        labels——YOLO可识别txt文件

runs：存放训练的模型本体

train.py:训练模型代码

predict.py：对图片批量处理代码
cam_demo.py：电脑摄像头演示代码


训练流程：
1.配置.yaml文件
2.标注train和val的图片
3.开train.py代码训练模型
4.验证结果