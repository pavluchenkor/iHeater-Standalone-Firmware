<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# Standalone iHeater 固件

iHeater 是一种紧凑且可靠的解决方案，适用于没有主动恒温室或主板连接能力有限的 3D 打印机。它特别适合配备专有主板的机型，这些主板没有额外的风扇、加热器或温度传感器接口。iHeater 以简单通用的方式解决了这一问题。


可以作为独立设备独立运行，或在 Klipper 管理下通过 USB 连接到打印机

![iHeater](../img/iHeater_200.png)

---

## 关于固件

**iHeater** 是用于 3D 打印机恒温室温度自主控制的固件。

---

## 功能

该设备可以加热并维持 3D 打印机恒温室内的稳定温度，这对于打印具有高收缩率和层间低粘附力的 ABS、ASA 和其他温度敏感型塑料至关重要。

---

## 如何配置固件

### `config.h` 文件

此文件设置设备的工作参数：
文件位置：Core/Inc/config.h

| 参数                                    | 功能                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | 选择温度传感器类型（0-5，见下文）                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | 每个模式的温度。可以设置任何值，包括小数，例如 `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | 加热器允许的温度范围                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | 空气温度范围                                                                         |
| `HEATER_MIN_PWM`                            | 最小 PWM，低于此值系统开始检查加热器响应                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | 等待加热器温度上升的时间（默认 5 秒）                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | 达到目标空气温度的最大时间（默认 10 分钟）                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | PWM 频率和品质调整                                                                    |

### 温度传感器

在 `SELECTED_THERMISTOR_TYPE` 中选择。支持的类型：

| ID | 类型                          | 备注                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | E3D 标准                       |
| 1  | ATC Semitec 104NT-4-R025H42G | 104GT 的替代品                          |
| 2  | EPCOS 100K B57560G104F       | 常见使用                             |
| 3  | Generic 3950                 | **默认**，广泛使用 |
| 4  | SliceEngineering 450         | 高温支持          |
| 5  | TDK NTCG104LH104JT1          | 医用级，精度高                   |

### 温度模式和 LED

模式由温度值设置，并以二进制形式通过三个 LED 显示：

当进入模式时 LED 闪烁，到达模式时常亮

| 模式   | 温度 | LED3 | LED2 | LED1 |
|--------|-------------|------|------|------|
| MODE_TEMP_0  | 0.0°C       |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_1  | 55.0°C      |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |
| MODE_TEMP_2  | 60.0°C      |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_3  | 65.0°C      |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |
| MODE_TEMP_4  | 70.0°C      |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_5  | 75.0°C      |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |
| MODE_TEMP_6  | 80.0°C      |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_7  | 85.0°C      |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](../img/ball_gifs/blinking_ball.gif)   |

---
### 自动启用和关闭 r1.1

宏定义
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

控制加热器的启用和关闭温度，以及启用时的模式。

假设触发器温度传感器位于热床附近。
使用当前设置，当热床温度达到 45℃ 时恒温室加热将启用，当热床温度低于 80℃ 时将关闭。这些参数对大多数需要主动恒温室的塑料都是合适的，并且在运行时不会引起逻辑冲突。

还可以选择触发触发器时将运行的模式。例如，当前选择 TRIGGER_MODE MODE_2，这意味着将运行 MODE_TEMP_2 —— 恒温室温度为 60.0°C

---
### 与 iHeater-link 配合工作（r1.1 pulse 版本）

`iheater_rev1_1_pulse` 版本设计用于与 ESP 上的 iHeater-link 模块配合工作。该模块从打印机、门户网站或移动应用程序接收命令，并通过一条 GPIO 线将恒温室设定点转换为 iHeater。iHeater 固件不依赖于打印机端，也不知道传输方式 —— 它只接收一个数字：所需的恒温室空气温度（°C）。

#### 连接

1. 用 `iheater_rev1_1_pulse` 版本刷入 iHeater（见"如何刷入"部分）。
2. 将来自 iHeater-link 的信号线连接到 iHeater 板的 **TH2** 接口（引脚 PB1）。
3. ESP 和 iHeater 之间必须有公共 GND —— 使用单独的接地线。
4. 将加热器温度传感器保留在其标准接口 **TH0** 上。空气温度传感器保留在 **TH1** 上。它们的工作方式与任何其他版本相同。
5. 此版本不使用触发器接口（其线路被数字输入占用）。

!!! warning "无论如何都不要在通电时连接或断开信号线。"

#### 接收的命令

| 来自 iHeater-link 的代码 | 含义 |
|---------------------|--------|
| `0`                 | 关闭加热 |
| `10`                | 关闭加热（约定的"off-code"） |
| `45..90`            | 启用加热，空气设定点为代码值 °C |
| 其他值  | 被忽略，保持之前的状态 |

最小值 `45 °C` 在 `config.h` 中由宏 `PULSE_MIN_TARGET` 设定。最大值受 `MAX_AIR_TEMP` 限制（默认 90 °C）。

#### LED 指示

| 状态 | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| 等待启动后第一个数据包（最多 20 秒） | LED1 → LED2 → LED3 循环，步长 1000 ms | | |
| 连接正常，加热关闭 | 常亮（心跳"连接活动"） | 熄灭 | 每收到一个数据包闪烁 ~80 ms |
| 连接正常，加热启用 | LED1 → LED2 → LED3 循环，步长 500 ms | | |
| 连接丢失（首次连接后超过 1.5 秒无数据包） | 三个 LED 同步闪烁；设备使用错误代码 `0x08`（`ERROR_LINK_LOST`）重启。加热器已关闭。 | | |

标准错误（过热、温度传感器断开、加热器无响应）的工作方式与模拟版本相同 —— 见"发生错误时"部分。

#### 连接丢失时的行为

启动 iHeater 和 iHeater-link 电源后，两者可能同时启动。iHeater-link 模块需要时间初始化 Wi-Fi 并连接到命令源，因此在启动后的前 **20 秒内**，iHeater 固件 **不将缺失数据包视为错误**，并显示缓慢循环（步长 1000 ms）—— "等待第一个数据包"。

如果在这 20 秒内没有收到任何有效数据包 —— 固件进入 `ERROR_LINK_LOST`，就像在正常模式下连接丢失一样。

接收到第一个数据包后，适用常规规则：数据包丢失超过 1.5 秒 → 错误。

错误触发时的算法：

1. 固件在闪存中保存错误代码 `0x08`（`ERROR_LINK_LOST`）。
2. 重启控制器。
3. 重启后，加热器已关闭；在新的 20 秒窗口内再次出现等待循环，之后是所有 LED 的缓慢闪烁。
4. 要重置错误，请在通电时按住 MODE 按钮，然后释放。设备将恢复到正常操作。

#### MODE 按钮

在 pulse 版本中，MODE 按钮 **不切换模式，也不启动校准**。它的唯一功能是在设备启动时重置保存的错误。所有加热命令仅来自 iHeater-link。

---

## 如何刷入

### 下载

[从发布页面下载固件](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### 使用预编译固件

固件已预先配置为具有上述 MODE_TEMP_0 至 MODE_TEMP_7 的温度模式，旨在与 Generic 3950 型温度传感器（`config.h` 列表中的类型 3）一起使用。

### 使用 DFU 模式 (USB)
=== "r1"

    1. 安装 BOOT0 跳线
    2. 连接 USB，设备将识别为 DFU
    3.1 使用 STM32CubeProgrammer 加载固件：

    3.1.1 指定 `.bin` 文件路径（例如：`Firmware/iHeater_v1.2.3.bin`）
    3.1.2 设置加载地址：`0x08000000`

    3.1.3 点击"Start Programming"

    或

    3.2 通过命令行加载固件：
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. 移除跳线并重新启动电源

=== "r1.1"

    1. 按住 BOOT 按钮
    2. 连接 USB，设备将识别为 DFU
    3. 使用 STM32CubeProgrammer 加载固件：

        3.1.1 指定 `.bin` 文件路径（例如：`Firmware/iHeater_v1.2.3.bin`）

        3.1.2 设置加载地址：`0x08000000`

        3.1.3 点击"Start Programming"

        或

        3.2 通过命令行加载固件：
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. 重新启动电源

---

## 如何使用
=== "r1"

    - **短按** MODE 按钮 - 切换模式 (0-7)
    - **长按（2+ 秒）** - 重置模式为 0 (OFF)
    - **LED** 显示上表中的当前模式
    - **LED 闪烁** - 模式活动，但尚未达到温度
    - **LED 常亮** - 已达到目标温度

=== "r1.1"

    !!! note "版本差异"
   
        r1.1 版本有一个触发器端口，用于自动启用和禁用恒温室加热。将温度传感器连接到触发器端口时，加热将根据 config.h 中指定的温度自动启用和禁用，默认情况下分别为 45 和 80。

    - **短按** MODE 按钮 - 切换模式 (0-7)
    - **长按（2+ 秒）** - 重置模式为 0 (OFF)
    - **LED** 显示上表中的当前模式
    - **LED 闪烁** - 模式活动，但尚未达到温度
    - **LED 常亮** - 已达到目标温度

---

## 发生错误时

设备自动重启并通过 LED 显示错误代码，风扇此时将打开。


| 代码  | 描述                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | 空气未能及时达到目标温度        |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | 加热器无响应                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | 手动关闭（未使用）                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | 空气温度传感器断开                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | 加热器温度传感器断开             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | 加热器过热                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | 空气过热                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | 与 iHeater-link 连接丢失（仅限 pulse 版本） | 0.25 Hz | 0.25 Hz | 0.25 Hz |
| 0xFF  | 未知错误                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### 重置错误

在设备启动时按住 MODE 按钮，直到 LED 指示改变。错误将从闪存中擦除，设备将继续正常工作。

## 微调

如果有确认温度传感器读数与实际温度不同，可以使用微调功能。为此，需要测量相应电阻的阻值，并将其值替换为默认值


```C++ title="上拉电阻的实际阻值"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

此软件根据 GNU General Public License v3.0 获得许可，仅供非商业使用。
