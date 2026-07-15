# Micro Serial OLED Terminal (MSOT)  
> 嵌入式串口数据实时终端显示器 – 基于 Atmega328p + 0.96" OLED  

[![Arduino](https://img.shields.io/badge/Platform-Arduino%20IDE-00979D?logo=arduino)](https://arduino.cc)  [![Display](https://img.shields.io/badge/Display-0.96%22%20SSD1306%20(I2C)-3C7B3C)](https://www.adafruit.com/product/326)  [![Library](https://img.shields.io/badge/Library-U8g2%20v2.28+-orange)](https://github.com/olikraus/U8g2_Arduino)  [![License](https://img.shields.io/badge/License-MIT-blue)](LICENSE)  

**Micro Serial OLED Terminal** 基于 Atmega328p 打造的轻量级串口数据显示终端，在 0.96 英寸屏幕上实时滚动显示串口ASCII 文本。支持一键 **暂停/恢复** 屏幕刷新，便于仔细查看历史数据，同时后台继续接收串口信息，恢复时即时补全。适用于嵌入式调试、传感器监控，**快速 & 便捷** 查看串口信息

---
# ⚙️ 特色功能
1. 配备有typec接口, **支持C to C数据线供电**, 支持由终端设备为调试设备供电,免去供电烦恼
2. 同时支持从调试设备取电, 即插即用
3. 支持**一键暂停**, 快速锁定内容
4. **跨平台换行兼容** – 支持 Windows (`\r\n`)、Linux (`\n`)、Mac (`\r`) 的换行风格。
5. **实时滚动显示** – 自动处理换行符（`\n` / `\r` / `\r\n`），实现平滑滚动，无撕裂或闪烁。


## 快速开始

>有关制作及其注意事项请详见此[文档](build.md)

### 外设
| 接口 | 功能 |
| :--- | :--- |
| USB-C | 仅供电, 兼容 C to C 数据线* |
| 4pin 排针 | 供电, 串口信号输入 |
| 拨轮 | 上拨向上翻页, 中键翻转刷新状态, 下拨向下翻页 |
| 蜂鸣器** | 按键提示 |

> *注: 须焊接R5 R6设为下拉以兼容 C to C数据线
  **注: 对应电路元件封装较小(0402封装), 不便于焊接, 此项为可选项

### 接线
| 4pin 排针(屏幕面从左到右) | 对外调试设备 |
| :--- | :--- |
| GND | GND |
| TXD | RXD |
| RXD | TXD |
| VCC | VCC / 5V |

## 🕹️ 使用说明  

### 基本操作  

| 操作 | 效果 |
| :--- | :--- |
| 串口发送文本（如 `Hello World\n`） | OLED 上滚动显示该行 |
| 发送超长文本（>21 字符） | 自动在 21 字符处换行 |
| 发送空行（单独 `\n`） | 产生一个空白行，用于间隔 |
| 按下中键 | 屏幕冻结，但串口继续接收 |
| 再次按下中键 | 屏幕立即补全所有新内容 |


### 软件环境
- **Arduino IDE** (≥1.8.13) 或 **PlatformIO**  
- 安装依赖库（通过 Arduino 库管理器）：  
  - `U8g2` by olikraus (≥2.28.10)  
  - `Adafruit BusIO`（U8g2 的依赖）

---

## 📦编译上传 
### 一. BootLoader烧录
1. 对于需 Arduino UNO / Nano 拆板的MCU可跳过此步骤, 移步至 **程序烧录** 步骤
2. 对于全新 Atmega328p 芯片: 
    1. 安装到编程座上, 或直接焊接至开发板上
    2. 使用AVRISP, 烧录BootLoader
       或使用另一块Arduino, 先上传Arduino ISP程序: *文件 --> 示例 --> ArduinoISP*, 然后按以下表格进行接线
    3. 点击 *工具 --> 编程器 --> Arduino as ISP (此项根据对应编程器选择, 此处以 Arduino as ISP 为例)*
    4. 点击 *烧录引导程序*

| 作为编程器的Arduino | 目标对象 |
| :--- | :--- |
| 13 | 13 |
| 12 | 12 |
| 11 | 11 |
| 10 | REST |
| GND | GND |
| VCC | VCC |

 

### 二. 程序烧录
>虽然PCB板已引出REST引脚, 但仍然建议先烧录程序, 再安装芯片到PCB板上
1. 下载位于src目录下的源码 `main.ino`。  
2. 在 Arduino IDE 中选择开发板 **Arduino Nano** 和对应串口。  
3. 点击 **上传**。  
4. 打开串口监视器（默认波特率 115200），发送任意文本，OLED 将立即滚动显示。

```shell
# 示例：发送 "Hello World\n"
# OLED 屏幕将显示该行，并自动换行滚动
```

---

## 开发指南

若你想在此基础上进行二次开发或定制，请遵循以下步骤：

```shell
git clone https://github.com/yourname/serial-oled-terminal.git
cd serial-oled-terminal/
# 使用 Arduino IDE 打开 main.ino，或使用 PlatformIO 打开项目文件夹
```

### 项目结构
```
├──PCB                       # PCB原始文件
├──BOM                       # 元件清单
├──Image                     # 示意图
├──Gerber                    # 制板文件
├──src
    └──  main.ino            # 主程序
├── README.md                # 本文件
└── LICENSE                  # MIT 许可证
```

### 核心代码模块
- **`processSerialData()`** – 非阻塞读取串口，解析 `\r`/`\n`，更新行缓冲区。  
- **`scrollUp()`** – 滚动显存数组（丢弃最旧行，为新行腾出空间）。  
- **`renderDisplay()`** – 使用 U8g2 全帧缓冲绘制并发送到 OLED。  
- **`handleButton()`** – 检测按钮下降沿，软件消抖，切换暂停状态。

## 🔧自定义配置
你可以在文件顶部修改以下宏定义以适应不同屏幕尺寸或硬件：

| 宏定义 | 默认值 | 说明 |
| :--- | :--- | :--- |
| `SCREEN_ROWS` | 6 | 显示行数（根据 OLED 高度和字体调整） |
| `SCREEN_COLS` / `MAX_LINE_LEN` | 21 | 每行字符数（根据 OLED 宽度和字体调整） |
| `BTN_PIN` | 2 | 按钮引脚号 |
| `DEBOUNCE_DELAY` | 50 | 消抖时间（毫秒） |

### 构建与测试
- 若更改了屏幕分辨率或字体，请同步调整 `SCREEN_ROWS` 和 `SCREEN_COLS`。  
- 编译时注意 RAM 占用（由于 ~硬件资源有限~ 个人能力不足, 当前已达94%）  
- 如需提升刷新速度，可在 `setup()` 中添加 `Wire.setClock(400000L);` 将 I2C 频率提至 400kHz。

### 配置参数

本项目提供了若干可调节的配置项，以适应不同硬件或偏好。

#### `SCREEN_ROWS`
- **类型**：`int`  
- **默认值**：`6`  
- **说明**：OLED 屏幕上显示的文字行数。根据屏幕高度和所选字体（当前为 6×10 像素）计算，例如 64 像素高度 / 10 像素行高 = 6 行。

#### `SCREEN_COLS`（同 `MAX_LINE_LEN`）
- **类型**：`int`  
- **默认值**：`21`  
- **说明**：每行最多容纳的字符数。根据屏幕宽度和字体宽度（6 像素）计算，128 / 6 ≈ 21。

#### `DEBOUNCE_DELAY`
- **类型**：`unsigned long`（毫秒）  
- **默认值**：`50`  
- **说明**：按钮消抖延时窗口。若按钮抖动较严重，可适当增大（如 100ms）。

#### 串口波特率
- **类型**：`long`  
- **默认值**：`115200`  
- **说明**：在 `Serial.begin()` 中修改。可与你的串口设备匹配（如 9600、57600 等）。

#### 示例：修改为 128×32 屏幕
若使用 128×32 分辨率 OLED，建议：
```cpp
#define SCREEN_ROWS     3
#define SCREEN_COLS     21
// 并在 setup() 中改用较小字体：u8g2.setFont(u8g2_font_5x7_tf);
```

---

## 贡献指南

由于此项目为我个人第一个正式开源项目, 且我个人能力十分有限, BUG在所难免. 所以欢迎任何形式的贡献，无论是问题报告、功能建议还是代码提交。请遵循以下流程：

1. **Fork** 本仓库到你的 GitHub 账户。  
2. 创建一个新的 **功能分支**（`git checkout -b feature/amazing-feature`）。  
3. 提交你的修改（`git commit -m 'Add some amazing feature'`）。  
4. 推送到分支（`git push origin feature/amazing-feature`）。  
5. 打开一个 **Pull Request**，清晰描述你的更改内容

---

## 链接与资源

- **项目主页**： [https://github.com/yourname/serial-oled-terminal](https://github.com/yourname/serial-oled-terminal)  
- **源代码仓库**： [https://github.com/yourname/serial-oled-terminal](https://github.com/yourname/serial-oled-terminal)  
- **问题跟踪**： [https://github.com/yourname/serial-oled-terminal/issues](https://github.com/yourname/serial-oled-terminal/issues)  
- **相关项目**：  
  - [U8g2 图形库](https://github.com/olikraus/U8g2_Arduino) – 为本项目提供 OLED 驱动。  
  - [Arduino 官方文档](https://www.arduino.cc/reference/en/) – 参考 Arduino API。

**联系**：如有疑问，可以发送邮件至 `bilibili.dragon.boy@gmail.com`。

---

## 许可证

本项目采用 **MIT 许可证**，详情请参阅 [LICENSE](LICENSE) 文件。  

---
