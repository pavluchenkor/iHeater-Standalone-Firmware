<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# 獨立 iHeater 韌體

iHeater 是一個小巧且經濟高效的解決方案，適用於沒有主動恆溫室或主機板連接功能有限的 3D 印表機。它特別適用於具有專有主機板的型號，這些主機板沒有風扇、加熱器或溫度計的可用連接器。iHeater 以簡單而通用的方式解決了這個問題。


可以作為獨立設備使用自己的韌體獨立運行，或在 Klipper 管理下通過 USB 連接到印表機

![iHeater](../img/iHeater_200.png)

---

## 關於韌體

**iHeater** 是用於 3D 印表機恆溫室溫度自主控制設備的韌體。

---

## 用途

該設備提供加熱和維持 3D 印表機恆溫室內穩定溫度，這對於列印 ABS、ASA 和其他對溫度敏感、收縮率高、層粘著性低的塑膠至關重要。

---

## 如何配置韌體

### `config.h` 檔案

此檔案設定設備的運行參數：
檔案位於 Core/Inc/config.h 資料夾中

| 參數                                    | 用途                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | 選擇溫度計類型 (0-5，見下文)                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | 每個模式的溫度。可以設定任何值，包括小數，例如 `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | 加熱器允許溫度範圍                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | 空氣溫度範圍                                                                         |
| `HEATER_MIN_PWM`                            | 最小 PWM，系統開始檢查加熱器回應時使用                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | 等待加熱器溫度上升的時間 (預設 5 秒)                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | 達到目標空氣溫度的最大時間 (預設 10 分鐘)                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | PWM 頻率和品質設置                                                                    |

### 溫度計

在 `SELECTED_THERMISTOR_TYPE` 中選擇。支持的類型：

| ID | 類型                          | 評論                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | E3D 標準                       |
| 1  | ATC Semitec 104NT-4-R025H42G | 104GT 的模擬                          |
| 2  | EPCOS 100K B57560G104F       | 流行的                             |
| 3  | Generic 3950                 | **預設**，廣泛使用 |
| 4  | SliceEngineering 450         | 高溫支持          |
| 5  | TDK NTCG104LH104JT1          | 醫療級，精確                   |

### 溫度模式和 LED

模式由溫度值設定，並以二進位形式由三個 LED 顯示：

進入模式時 LED 閃爍，達到模式時持續發光

| Mode   | Temperature | LED3 | LED2 | LED1 |
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
### 自動開啟和關閉 r1.1

巨集
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

負責加熱器的開啟和關閉溫度，以及開啟時的模式。

假設觸發溫度計位於加熱台附近。
使用目前的設定，當台面溫度達到 45°C 時恆溫室加熱會開啟，當台面溫度下降到 80°C 以下時會關閉。這些參數適合大多數需要主動恆溫室的塑膠，並且在正常運行時不會造成邏輯衝突。

您也可以選擇在觸發時啟動的模式。例如，目前選擇的是 TRIGGER_MODE MODE_2，這意味著將啟動 MODE_TEMP_2 - 恆溫室溫度為 60.0°C

---
### 與 iHeater-link 協作 (r1.1 pulse 組件)

`iheater_rev1_1_pulse` 組件設計用於與 ESP 上的 iHeater-link 模組協作。該模組從印表機、門戶網站或行動應用程式接收命令，並通過單個 GPIO 線將恆溫室設定點傳達給 iHeater。iHeater 韌體不依賴於印表機端，也不知道傳輸方式 - 它只接收一個數字：所需的恆溫室空氣溫度，單位為 °C。

#### 連接

1. 使用 `iheater_rev1_1_pulse` 組件對 iHeater 進行刷新 (見「如何刷新」部分)。
2. 將來自 iHeater-link 的信號線連接到 iHeater 主機板上的 **TH2** 連接器 (引腳 PB1)。
3. ESP 和 iHeater 之間的公共 GND 是必需的 - 使用單獨的接地線。
4. 將加熱器溫度計保留在其標準連接器 **TH0** 上。空氣溫度計位於 **TH1**。它們的工作方式與任何其他組件中的一樣。
5. 觸發連接器在此組件中未使用 (其線路被數位輸入佔用)。

!!! warning "切勿在有電源的情況下連接或斷開信號線。"

#### 接收的命令

| iHeater-link 代碼 | 含義 |
|---------------------|--------|
| `0`                 | 關閉加熱 |
| `10`                | 關閉加熱 (協商的「off-code」) |
| `45..90`            | 開啟加熱，空氣設定點代碼為 °C |
| 其他值  | 被忽略，保持上一個狀態 |

最小值 `45 °C` 在 `config.h` 中由巨集 `PULSE_MIN_TARGET` 設定。最大值受 `MAX_AIR_TEMP` 限制 (預設 90 °C)。

#### LED 指示

| 狀態 | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| 等待開機後的第一個數據包 (最多 20 秒) | 流動燈 LED1 → LED2 → LED3，間隔 1000 毫秒 | | |
| 通信正常，加熱已關閉 | 持續發光 (心跳「通信正常」) | 熄滅 | 每收到一個數據包短閃約 80 毫秒 |
| 通信正常，加熱已開啟 | 流動燈 LED1 → LED2 → LED3，間隔 500 毫秒 | | |
| 通信丟失 (第一次連接後超過 1.5 秒沒有數據包) | 三個 LED 同步閃爍；設備重新啟動並顯示錯誤代碼 `0x08` (`ERROR_LINK_LOST`)。加熱器已關閉。 | | |

標準錯誤 (過熱、溫度計斷路、加熱器無回應) 的工作方式與類比組件相同 - 見「發生錯誤時如何處理」部分。

#### 連接遺失時的行為

開啟 iHeater 和 iHeater-link 的電源後，它們可能同時啟動。iHeater-link 模組需要時間初始化 Wi-Fi 並連接到命令來源，因此在啟動後的前 **20 秒**，iHeater 韌體 **不會將數據包遺失視為錯誤**，並顯示緩慢流動燈 (1000 毫秒間隔) - 「等待第一個數據包」。

如果在這 20 秒內仍未收到單個正確的數據包 - 韌體將進入 `ERROR_LINK_LOST` 狀態，就像在正常模式下連接遺失一樣。

收到第一個數據包後，適用通常規則：超過 1.5 秒未收到數據包 → 錯誤。

錯誤觸發時的演算法：

1. 韌體在閃存中保存錯誤代碼 `0x08` (`ERROR_LINK_LOST`)。
2. 重新啟動控制器。
3. 重新啟動後，加熱器已關閉；在新的 20 秒窗口內再次顯示等待流動燈，之後 - 所有 LED 緩慢閃爍。
4. 要重設錯誤，在加電時按住 MODE 按鈕，然後鬆開。設備將恢復到正常運行。

#### MODE 按鈕

在 pulse 組件中，MODE 按鈕 **不會切換模式，也不會啟動校準**。它的唯一功能是在設備啟動時重設已保存的錯誤。所有加熱命令僅來自 iHeater-link。

---

## 如何刷新

### 下載

[從發佈頁面下載韌體](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### 使用預先配置的韌體

韌體已使用上述 MODE_TEMP_0 至 MODE_TEMP_7 溫度模式進行預先配置，並設計用於與 Generic 3950 類型的溫度計 (根據 `config.h` 列表中的類型 3) 協作。

### 使用 DFU 模式 (USB)
=== "r1"

    1. 設定 BOOT0 跳線
    2. 連接 USB，設備將被識別為 DFU
    3.1 使用 STM32CubeProgrammer 載入韌體：

    3.1.1 指定 `.bin` 韌體檔案的路徑 (例如：`Firmware/iHeater_v1.2.3.bin`)
    3.1.2 設定載入位址：`0x08000000`

    3.1.3 按一下「Start Programming」

    或

    3.2 透過命令列載入韌體：
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. 移除跳線並重新啟動電源

=== "r1.1"

    1. 按住 BOOT 按鈕
    2. 連接 USB，設備將被識別為 DFU
    3. 使用 STM32CubeProgrammer 載入韌體：

        3.1.1 指定 `.bin` 韌體檔案的路徑 (例如：`Firmware/iHeater_v1.2.3.bin`)

        3.1.2 設定載入位址：`0x08000000`

        3.1.3 按一下「Start Programming」

        或

        3.2 透過命令列載入韌體：
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. 重新啟動電源

---

## 如何使用
=== "r1"

    - **短按** MODE 按鈕 - 切換模式 (0-7)
    - **長按 (2+ 秒)** - 將模式重設為 0 (OFF)
    - **LED** 根據上表顯示目前模式
    - **LED 閃爍** - 模式已啟用，但尚未達到溫度
    - **持續發光** - 已達到目標溫度

=== "r1.1"

    !!! note "修訂版本的差異"
   
        修訂版本 r1.1 有一個觸發埠，用於自動開啟和關閉恆溫室加熱。將溫度計連接到觸發埠時，加熱將根據 config.h 中指定的溫度自動開啟和關閉，預設值為開啟時 45 和關閉時 80。

    - **短按** MODE 按鈕 - 切換模式 (0-7)
    - **長按 (2+ 秒)** - 將模式重設為 0 (OFF)
    - **LED** 根據上表顯示目前模式
    - **LED 閃爍** - 模式已啟用，但尚未達到溫度
    - **持續發光** - 已達到目標溫度

---

## 發生錯誤時如何處理

設備自動重新啟動並使用 LED 顯示錯誤代碼，此時冷卻風扇將打開。


| Code  | Description                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | 空氣未在規定時間內達到目標        |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | 加熱器無回應                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | 手動關閉 (未使用)                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | 空氣溫度計已斷開                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | 加熱器溫度計已斷開             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | 加熱器過溫                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | 空氣過溫                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | iHeater-link 連接遺失 (僅 pulse 組件) | 0.25 Hz | 0.25 Hz | 0.25 Hz |
| 0xFF  | 未知錯誤                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### 重設錯誤

在啟動時按住 MODE 按鈕，直到 LED 指示改變。錯誤將從閃存中清除，設備將繼續正常運行。

## 微調

如果已確認溫度計讀數與實際溫度不同，可以使用微調功能。為此，需要測量相應電阻器的電阻，並記錄它們以替換預設值


```C++ title="Actual resistance of the pull-up resistors"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

此軟體根據 GNU General Public License v3.0 僅許可用於非商業目的。
