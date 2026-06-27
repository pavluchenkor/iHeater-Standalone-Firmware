<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# Standalone iHeater ファームウェア

iHeaterは、アクティブな温度制御チャンバーがない、またはマザーボードへの接続オプションが限られている3Dプリンター向けのコンパクトで手頃なソリューションです。特に専有基板を持つモデルに有用で、ファン、ヒーター、または温度センサー用の空きコネクタがありません。iHeaterはシンプルで汎用的な方法でこの問題を解決します。


独自のファームウェアを備えた独立したデバイスとして機能することも、USB経由でプリンターに接続してKlipperの制御下で動作することも可能です。

![iHeater](../img/iHeater_200.png)

---

## ファームウェアについて

**iHeater** - 3Dプリンターのチャンバー温度を自動的に制御するデバイス用のファームウェアです。

---

## 用途

このデバイスは、3Dプリンターのチャンバー内の温度を加熱および維持します。これはABS、ASA、および寸法縮小が大きく層間接着性が低い他の温度依存プラスチックの印刷に不可欠です。

---

## ファームウェアの設定方法

### ファイル `config.h`

このファイルでデバイスの動作パラメータを設定します。
ファイルはCore/Inc/config.hフォルダにあります

| パラメータ                                    | 説明                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | サーミスタの種類を選択（0-5、以下を参照）                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | 各モードの温度。分数値を含む任意の値を設定できます（例：`MODE_TEMP_4 67.5`） |
| `MAX_TEMP`, `MIN_TEMP`                      | ヒーターの許容温度範囲                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | 空気温度の範囲                                                                         |
| `HEATER_MIN_PWM`                            | システムがヒーター応答チェックを開始するときの最小PWM                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | ヒーター温度上昇を待つ時間（デフォルト 5秒）                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | 空気目標温度に達する最大時間（デフォルト 10分）                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | PWM周波数と品質の設定                                                                    |

### サーミスタ

`SELECTED_THERMISTOR_TYPE`で選択します。サポートされているもの：

| ID | タイプ                          | コメント                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | E3D標準                       |
| 1  | ATC Semitec 104NT-4-R025H42G | 104GTアナログ                          |
| 2  | EPCOS 100K B57560G104F       | 人気                             |
| 3  | Generic 3950                 | **デフォルト**、広く使用されている |
| 4  | SliceEngineering 450         | 高温対応          |
| 5  | TDK NTCG104LH104JT1          | 医療用、精密                   |

### 温度モードとLED

モードは温度値によって設定され、3つのLEDによってバイナリ形式で表示されます。

モードに到達するとLEDが点滅し、モード達成後は継続して点灯します

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
### 自動オン・オフ r1.1

マクロ
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

ヒーターのオンオフ温度とオン時のモードを担当します。

トリガーサーミスタがヒーティングプレート近くに配置されていると仮定します。
現在の設定では、プレート温度が45に達するとチャンバー加熱がオンになり、プレート温度が80を下回るとオフになります。これらのパラメータはアクティブなチャンバーを必要とするほとんどのプラスチックに適切で、動作時に論理的な矛盾を引き起こしません。

トリガーが起動したときに実行されるモードを選択することもできます。例えば、TRIGGER_MODE MODE_2が選択されている場合、MODE_TEMP_2が実行されます - チャンバー温度60.0°C

---
### iHeater-linkでの動作（r1.1 pulseビルド）

`iheater_rev1_1_pulse`ビルドは、ESP上のiHeater-linkモジュールとの連携動作用に設計されています。モジュールはプリンター、ポータル、またはモバイルアプリからコマンドを受け取り、チャンバー設定値をiHeaterの単一GPIO行を介して変換します。iHeaterファームウェアはプリンター側から独立し、トランスポートについて知りません - それは単に値を受け取ります：チャンバー内の空気の望ましい温度（°C）。

#### 接続

1. `iheater_rev1_1_pulse`ビルドでiHeaterをフラッシュします（「プログラミング方法」セクションを参照）。
2. iHeater-linkからの信号ワイヤをiHeater基板の**TH2**コネクタ（ピンPB1）に接続します。
3. ESPとiHeater間の共通GND接続は必須です - 別の接地ワイヤを使用します。
4. ヒーターサーミスタを標準コネクタ**TH0**に保ちます。空気サーミスタ - **TH1**に。他のビルドと同様に機能します。
5. このビルドではトリガーコネクタは使用されません（その行はデジタル入力に使用されます）。

!!! warning "電源が供給されている間、信号ワイヤを接続または切断しないでください。"

#### 受信コマンド

| iHeater-linkからのコード | 意味 |
|---------------------|--------|
| `0`                 | 加熱をオフ |
| `10`                | 加熱をオフ（調整済み「off-code」） |
| `45..90`            | 加熱をオン、空気設定値は°Cのコード値 |
| その他の値  | 無視、前の状態を保持 |

最小値`45 °C`は`config.h`マクロ`PULSE_MIN_TARGET`で設定されます。最大値は`MAX_AIR_TEMP`（デフォルト90 °C）で制限されます。

#### LED表示

| 状態 | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| 起動後の最初のパケット待機（最大20秒） | LED1 → LED2 → LED3ランニングライト、1000msステップ | | |
| リンクOK、加熱オフ | 継続点灯（ハートビート「リンク生存」） | 消灯 | 各受信パケット時に短いフラッシュ約80ms |
| リンクOK、加熱オン | LED1 → LED2 → LED3ランニングライト、500msステップ | | |
| リンク喪失（最初の接続後1.5秒以上パケットなし） | 3つすべてが同期して点滅。デバイスはエラーコード`0x08`（`ERROR_LINK_LOST`）で再起動します。ヒーターはオフです。 | | |

標準エラー（過熱、サーミスタの断線、ヒーター無応答）は、アナログビルドと同じように機能します - 「エラーが発生した場合」セクションを参照してください。

#### リンク喪失時の動作

iHeaterとiHeater-linkが同時に起動した後、iHeater-linkモジュールはWi-Fiを初期化し、コマンドソースに接続するための時間が必要です。起動後最初の**20秒間**、iHeaterファームウェアはパケットの欠落をエラーと見なさず、遅いランニングライト（1000msステップ）を表示します - 「最初のパケット待機中」。

これらの20秒以内に有効なパケットが1つも到着しない場合、ファームウェアは通常モードでリンクが失われたときと同じく`ERROR_LINK_LOST`に入ります。

最初のパケットが受信されたら、通常のルールが適用されます：1.5秒以上のパケット喪失→エラー。

エラーがトリガーされたときのアルゴリズム：

1. ファームウェアはflashにエラーコード`0x08`（`ERROR_LINK_LOST`）を保存します。
2. コントローラーを再起動します。
3. 再起動後、ヒーターはオフです。新しい20秒のウィンドウ内でもう一度待機ランニングライト、その後すべてのLEDが遅く点滅します。
4. エラーをクリアするには、電源投入時にMODEボタンを押し続け、その後離します。デバイスは通常動作に戻ります。

#### MODEボタン

pulse-ビルドでは、MODEボタン**はモードを切り替えず、キャリブレーションを開始しません**。その唯一の機能はデバイス起動時の保存されたエラーのクリアです。すべての加熱コマンドはiHeater-linkからのみ来ます。

---

## プログラミング方法

### ダウンロード

[リリースページからファームウェアをダウンロードします](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### 準備ファームウェアの使用

ファームウェアは既に上記で説明されたMODE_TEMP_0からMODE_TEMP_7の温度モードで設定されており、Generic 3950型サーミスタ（`config.h`リストの型3）での動作を目的としています。

### DFUモード（USB）の使用
=== "r1"

    1. BOOT0ジャンパーを設定します
    2. USBを接続します。デバイスはDFUとして認識されます
    3.1 STM32CubeProgrammerを使用してファームウェアをロードします：

    3.1.1 ファームウェアの`.bin`ファイルへのパスを指定します（例：`Firmware/iHeater_v1.2.3.bin`）
    3.1.2 ロードアドレスを設定します：`0x08000000`

    3.1.3 「Start Programming」をクリック

    または

    3.2 コマンドラインを通じてファームウェアをロードします：
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. ジャンパーを外し、電源を再起動します

=== "r1.1"

    1. BOOTボタンを長押しします
    2. USBを接続します。デバイスはDFUとして認識されます
    3. STM32CubeProgrammerを使用してファームウェアをロードします：

        3.1.1 ファームウェアの`.bin`ファイルへのパスを指定します（例：`Firmware/iHeater_v1.2.3.bin`）

        3.1.2 ロードアドレスを設定します：`0x08000000`

        3.1.3 「Start Programming」をクリック

        または

        3.2 コマンドラインを通じてファームウェアをロードします：
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. 電源を再起動します

---

## 使用方法
=== "r1"

    - **短い押下** MODEボタン - モード切り替え（0-7）
    - **長押し（2秒以上）** - モード0（OFF）へのリセット
    - **LED** - 上の表にしたがって現在のモードを表示
    - **LED点滅** - モードはアクティブですが、温度にはまだ達していない
    - **継続光** - 目標温度に達した

=== "r1.1"

    !!! note "リビジョンの違い"
   
        リビジョンr1.1はチャンバー加熱の自動オン・オフ用のトリガーポートを備えています。トリガーポートにサーミスタを接続すると、加熱はconfig.hで指定された温度に従ってオン・オフします。デフォルトではオンが45度、オフが80度です。

    - **短い押下** MODEボタン - モード切り替え（0-7）
    - **長押し（2秒以上）** - モード0（OFF）へのリセット
    - **LED** - 上の表にしたがって現在のモードを表示
    - **LED点滅** - モードはアクティブですが、温度にはまだ達していない
    - **継続光** - 目標温度に達した

---

## エラーが発生した場合

デバイスは自動的に再起動し、LEDでエラーコードを表示します。この場合、ファンは有効になります。


| Code  | Description                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | 空気が時間内に目標に達しなかった        |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | ヒーターが応答していない                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | 手動シャットダウン（未使用）                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | 空気サーミスタが接続されていない                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | ヒーターサーミスタが接続されていない             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | ヒーター過熱                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | 空気過熱                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | iHeater-linkとの接続喪失（pulse-ビルドのみ） | 0.25 Hz | 0.25 Hz | 0.25 Hz |
| 0xFF  | 未知のエラー                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### エラーのクリア

起動時にMODEボタンを長く押し続け、LEDの表示が変わるまで押し続けます。エラーはflashから削除され、デバイスは通常動作を続けます。

## 微調整

サーミスタの読み取り値が実際の温度と異なることが確認されている場合、微調整機能を使用できます。そのためには、対応するプルアップ抵抗の抵抗を測定し、デフォルト値の代わりに記録する必要があります


```C++ title="Actual resistance of the pull-up resistors"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

このソフトウェアはGNU General Public License v3.0に基づいてライセンスされており、非商用利用のみです。
