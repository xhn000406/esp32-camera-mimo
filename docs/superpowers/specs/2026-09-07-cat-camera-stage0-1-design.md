# ESP32-S3 猫咪摄像头阶段 0–1 设计

## 范围

为 GOOJUU ESP32-S3-CAM V1.5（ESP32-S3 N16R8、OV5640、板载 TF）建立 ESP-IDF 5.5.1 最小启动固件。该阶段只验证开发环境、原生 USB 烧录和启动诊断；不初始化摄像头、Wi-Fi、TF、AI 或音频。

## 已确认硬件事实

- 原生 USB Serial/JTAG 端口：`/dev/cu.usbmodem101`
- 芯片：ESP32-S3 QFN56 rev 0.2，40 MHz 晶振
- Flash：16 MB，3.3 V，Quad 模式
- PSRAM：8 MB，3.3 V
- MAC：`28:84:85:4A:2F:E8`
- Camera 参考映射：GPIO4/5/6/7/8–13/15–18；本阶段不得使用。
- TF 参考映射：GPIO38 CMD、GPIO39 CLK、GPIO40 DAT0；本阶段不得使用。

## 设计

使用单一 `main` 组件。启动任务读取 ESP-IDF、芯片型号、Flash 容量、PSRAM 容量、复位原因和 MAC，全部通过 USB Serial/JTAG 控制台以 115200 输出。硬件映射集中在一个只读头文件中，标示为“参考值，尚未摄像头实测”，防止业务代码直接散落 GPIO 数字。

## 验收

1. `idf.py build` 在 macOS arm64 上通过，目标为 `esp32s3`。
2. `idf.py -p /dev/cu.usbmodem101 flash monitor` 能烧录并输出包含版本、Flash、PSRAM、MAC、复位原因的启动报告。
3. 连续复位 20 次后，每次均能观察到完整启动报告；摄像头和 TF 保持未初始化。

## 非目标

不在此阶段设定最终摄像头 RESET/PWDN、音频 GPIO、实际视频帧率或 AI 准确率。
