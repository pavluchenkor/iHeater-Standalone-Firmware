[![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)
[![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/README_ru/)
[![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer)


# Standalone iHeater Firmware

iHeater - это компактное и доступное решение для 3D-принтеров без активной термокамеры или с ограниченными возможностями подключения к материнской плате. Он особенно полезен для моделей с проприетарными платами, где нет свободных разъёмов для вентилятора, нагревателя или термистора. iHeater решает эту задачу простым и универсальным способом.


Может работать самостоятельно с собственной прошивкой как отдельное устройство или под управлением Klipper с подключением к принтеру по USB

![iHeater](imgweb/iHeater.png)

---

## О прошивке

**iHeater** - это прошивка для автономного устройства управления температурой камеры 3D-принтера. 

---

## Назначение

Устройство обеспечивает нагрев и поддержание стабильной температуры внутри камеры 3D-принтера, что критично для печати ABS, ASA и других температурозависимых пластиков с высокой усадкой и низкой адгезией слоев.

---

## Как настроить прошивку

### Файл `config.h`

В этом файле задаются параметры работы устройства:
Файл находится в папке Core/Inc/config.h

| Параметр                                    | Назначение                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | Выбор типа термистора (0-5, см. ниже)                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | Температуры для каждого режима. Можно задать любое значение, включая дробные, например `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | Допустимый диапазон температур нагревателя                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | Диапазон температур воздуха                                                                         |
| `HEATER_MIN_PWM`                            | Минимальный PWM, при котором система начнёт проверку ответа нагревателя                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | Время ожидания роста температуры нагревателя (по умолчанию 5 сек)                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | Максимальное время достижения целевой температуры воздуха (по умолчанию 10 мин)                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | Настройка частоты и качества ШИМ                                                                    |

### Термисторы

Выбираются в `SELECTED_THERMISTOR_TYPE`. Поддерживаются:

| ID | Тип                          | Комментарий                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | Стандарт от E3D                       |
| 1  | ATC Semitec 104NT-4-R025H42G | Аналог 104GT                          |
| 2  | EPCOS 100K B57560G104F       | Популярен                             |
| 3  | Generic 3950                 | **По умолчанию**, широко используемый |
| 4  | SliceEngineering 450         | Поддержка высоких температур          |
| 5  | TDK NTCG104LH104JT1          | Медицинский, точный                   |

### Режимы температуры и светодиоды

Режим задаётся значением температуры и отображается в двоичном виде тремя светодиодами:

При выходе на режим светодиоды мигают, по выходу на режим - светят постоянно

| Mode   | Temperature | LED3 | LED2 | LED1 |
|--------|-------------|------|------|------|
| MODE_TEMP_0  | 0.0°C       |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_1  | 55.0°C      |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |
| MODE_TEMP_2  | 60.0°C      |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_3  | 65.0°C      |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |
| MODE_TEMP_4  | 70.0°C      |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_5  | 75.0°C      |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |
| MODE_TEMP_6  | 80.0°C      |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED OFF](img/ball_gifs/black_ball.gif)   |
| MODE_TEMP_7  | 85.0°C      |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |![LED 1Hz](img/ball_gifs/blinking_ball.gif)   |

---
### Автоматическое включение и выключение r1.1

Макросы
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

Отвечают за температуру включения и выключения нагревателя а так же за режим при включении. 

Предположим что термистор триггера расположен в районе нагревателя стола.
С текущими настройками нагрев камеры включится когда температура стола будет 45 и выключится когда температура стола опустится ниже 80, такие параметры приемлемы для большинства пластиков требующих активной термокамеры и не вызывают логических конфликтов при работе.

Так же можно выбрать режим который будет запущен при срабатывании триггера. Например сейчас выбран TRIGGER_MODE MODE_2 и это значит что будет запущен режим MODE_TEMP_2 -  температура в камере 60.0°C

---

## Как прошить

### Скачать

[Скачайте прошивку со страницы релизов](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### Использование готовой прошивки

Прошивка уже сконфигурирована с температурными режимами от MODE_TEMP_0 до MODE_TEMP_7, описанными выше, и предназначена для работы с термистором типа Generic 3950 (тип 3 по списку в `config.h`).

### Использование DFU режима (USB)
=== "r1"

    1. Установите джампер BOOT0
    2. Подключите USB, устройство определится как DFU
    3.1 Загрузите прошивку при помощи STM32CubeProgrammer:

    3.1.1 Укажите путь к `.bin`-файлу прошивки (например: `Firmware/iHeater_v1.2.3.bin`)
    3.1.2 Установите адрес загрузки: `0x08000000`

    3.1.3 Нажмите "Start Programming"

    или

    3.2 Загрузите прошивку через командную строку:
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. Уберите джампер и перезапустите питание

=== "r1.1"

    1. Зажмите кнопку BOOT
    2. Подключите USB, устройство определится как DFU
    3. Загрузите прошивку при помощи STM32CubeProgrammer:

        3.1.1 Укажите путь к `.bin`-файлу прошивки (например: `Firmware/iHeater_v1.2.3.bin`)

        3.1.2 Установите адрес загрузки: `0x08000000`

        3.1.3 Нажмите "Start Programming"

        или

        3.2 Загрузите прошивку через командную строку:
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. Перезапустите питание

---

## Как пользоваться
=== "r1"

    - **Короткое нажатие** кнопки MODE - переключение режимов (0-7)
    - **Долгое удержание (2+ сек)** - сброс режима до 0 (OFF)
    - **Светодиоды** отображают текущий режим по таблице выше
    - **Мерцание светодиодов** - режим активен, но ещё не достигнута температура
    - **Постоянный свет** - целевая температура достигнута

=== "r1.1"

    !!! note "Отличия ревизий"
   
        ревизия r1.1 имеет триггерный порт для автоматического включения и выключения нагрева камеры. При подключении термистора к триггерному порту нагрев будет включаться и выключаться в соответствии с температурами указанными в config.h, по умолчанию это 45 и 80 для включения и выключения нагрева.

    - **Короткое нажатие** кнопки MODE - переключение режимов (0-7)
    - **Долгое удержание (2+ сек)** - сброс режима до 0 (OFF)
    - **Светодиоды** отображают текущий режим по таблице выше
    - **Мерцание светодиодов** - режим активен, но ещё не достигнута температура
    - **Постоянный свет** - целевая температура достигнута

---

## Как быть в случае ошибки

Устройство автоматически перезапускается и показывает код ошибки светодиодами, кулер при этом будет включен.


| Code  | Description                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | Air failed to reach target in time        |![LED OFF](img/ball_gifs/black_ball.gif)  |![LED OFF](img/ball_gifs/black_ball.gif) |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | Heater not responding                     |![LED OFF](img/ball_gifs/black_ball.gif) |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](img/ball_gifs/black_ball.gif)  |
| 0x03  | Manual shutdown (unused)                  |![LED OFF](img/ball_gifs/black_ball.gif)  |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | Air thermistor disconnected                |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](img/ball_gifs/black_ball.gif) |![LED OFF](img/ball_gifs/black_ball.gif)   |
| 0x05  | Heater thermistor disconnected             |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](img/ball_gifs/black_ball.gif) |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | Heater overtemperature                     |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](img/ball_gifs/black_ball.gif)   |
| 0x07  | Air overtemperature                        |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](img/ball_gifs/blinking_ball_10hz.gif)   |
| 0xFF  | Unknown error                              |![LED ON](img/ball_gifs/red_ball.gif)  | ![LED ON](img/ball_gifs/red_ball.gif)|![LED ON](img/ball_gifs/red_ball.gif)  |



### Сброс ошибки

Удерживайте кнопку MODE при старте до тех пор, пока светодиоды не поменяют индикацию. Ошибка будет стерта из flash и устройство продолжит обычную работу.

## Тонкая настройка

В случае когда есть подтверждение, что показания термопары отличаются от фактической температуры, можно воспользоваться функцией тонкой настройки, для этого необходимо измерить сопротивления соответствующих резисторов и записать их вместо дефолтных значений


```C++ title="Actual resistance of the pull-up resistors"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

Это программное обеспечение лицензировано в соответствии с GNU General Public License v3.0 только для некоммерческого использования.