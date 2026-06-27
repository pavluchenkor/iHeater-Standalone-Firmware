<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# Firmware iHeater Independiente

iHeater es una solución compacta y asequible para impresoras 3D sin cámara térmica activa o con capacidades de conexión limitadas a la placa base. Es especialmente útil para modelos con placas propietarias donde no hay conectores disponibles para ventilador, calentador o termistor. iHeater resuelve esta tarea de manera simple y universal.

Puede funcionar de forma independiente con su propio firmware como dispositivo separado o bajo control de Klipper conectando a la impresora por USB.

![iHeater](../img/iHeater_200.png)

---

## Acerca del firmware

**iHeater** es firmware para un dispositivo autónomo de control de temperatura de cámara de una impresora 3D.

---

## Propósito

El dispositivo proporciona calefacción y mantenimiento de una temperatura estable dentro de la cámara de la impresora 3D, lo cual es crítico para imprimir ABS, ASA y otros plásticos dependientes de la temperatura con alta contracción y baja adhesión entre capas.

---

## Cómo configurar el firmware

### Archivo `config.h`

Este archivo establece los parámetros de funcionamiento del dispositivo:
El archivo se encuentra en la carpeta Core/Inc/config.h

| Parámetro                                    | Propósito                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | Selección del tipo de termistor (0-5, véase más abajo)                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | Temperaturas para cada modo. Se puede establecer cualquier valor, incluidos valores fraccionarios, por ejemplo `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | Rango permitido de temperaturas del calentador                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | Rango de temperaturas del aire                                                                         |
| `HEATER_MIN_PWM`                            | PWM mínimo al cual el sistema comienza a verificar la respuesta del calentador                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | Tiempo de espera del aumento de temperatura del calentador (por defecto 5 seg)                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | Tiempo máximo para alcanzar la temperatura objetivo del aire (por defecto 10 min)                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | Configuración de frecuencia y calidad del PWM                                                                    |

### Termistores

Se seleccionan en `SELECTED_THERMISTOR_TYPE`. Soportados:

| ID | Tipo                          | Comentario                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | Estándar de E3D                       |
| 1  | ATC Semitec 104NT-4-R025H42G | Análogo del 104GT                          |
| 2  | EPCOS 100K B57560G104F       | Popular                             |
| 3  | Generic 3950                 | **Por defecto**, ampliamente utilizado |
| 4  | SliceEngineering 450         | Soporte de altas temperaturas          |
| 5  | TDK NTCG104LH104JT1          | Médico, preciso                   |

### Modos de temperatura y LED

El modo se establece por valor de temperatura y se muestra en forma binaria mediante tres LED:

Cuando se alcanza el modo, los LED parpadean; cuando se estabiliza el modo, se iluminan constantemente.

| Modo   | Temperatura | LED3 | LED2 | LED1 |
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
### Activación y desactivación automáticas r1.1

Macros
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

Responsables de la temperatura de activación y desactivación del calentador, así como del modo al activarse.

Supongamos que el termistor del disparador se encuentra en el área de la mesa de impresión. Con la configuración actual, la calefacción de la cámara se encenderá cuando la temperatura de la mesa sea 45 y se apagará cuando la temperatura de la mesa caiga por debajo de 80. Estos parámetros son aceptables para la mayoría de los plásticos que requieren cámara térmica activa y no causan conflictos lógicos durante la operación.

También puede seleccionar el modo que se ejecutará cuando se active el disparador. Por ejemplo, ahora está seleccionado TRIGGER_MODE MODE_2 y esto significa que se ejecutará el modo MODE_TEMP_2 - temperatura en la cámara 60.0°C

---
### Trabajo con iHeater-link (construcción r1.1 pulse)

La construcción `iheater_rev1_1_pulse` está diseñada para trabajar conjuntamente con el módulo iHeater-link en ESP. El módulo recibe comandos de la impresora, portal o aplicación móvil y transmite la consigna de la cámara a iHeater a través de una línea GPIO. El firmware de iHeater no depende del lado de la impresora y no conoce el transporte - solo recibe un número: la temperatura deseada del aire en la cámara en °C.

#### Conexión

1. Grabe el firmware de iHeater con la construcción `iheater_rev1_1_pulse` (véase la sección "Cómo grabar").
2. Conecte el cable de señal de iHeater-link al conector **TH2** de la placa iHeater (pin PB1).
3. La conexión común GND entre ESP e iHeater es obligatoria: use un cable de tierra separado.
4. Deje el termistor del calentador en su conector estándar **TH0**. El termistor del aire en **TH1**. Funcionan como en cualquier otra construcción.
5. El conector del disparador no se usa en esta construcción (su línea está ocupada por una entrada digital).

!!! warning "Nunca conecte ni desconecte el cable de señal con la alimentación aplicada."

#### Comandos recibidos

| Código de iHeater-link | Significado |
|---------------------|--------|
| `0`                 | Apagar calefacción |
| `10`                | Apagar calefacción (código de desconexión acordado) |
| `45..90`            | Encender calefacción, consigna de aire en °C igual al código |
| otros valores  | Se ignoran, se conserva el estado anterior |

El mínimo de `45 °C` se establece en `config.h` con la macro `PULSE_MIN_TARGET`. El máximo está limitado por `MAX_AIR_TEMP` (por defecto 90 °C).

#### Indicación LED

| Estado | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| Espera del primer paquete después del encendido (hasta 20 s) | LED corriente LED1 → LED2 → LED3 con paso de 1000 ms | | |
| Conexión OK, calefacción apagada | encendido constante (latido de "conexión activa") | apagado | destello corto ~80 ms para cada paquete recibido |
| Conexión OK, calefacción encendida | LED corriente LED1 → LED2 → LED3 con paso de 500 ms | | |
| Conexión perdida (más de 1.5 s sin paquete después de la primera conexión) | Los tres parpadean sincrónicamente; el dispositivo se reinicia con código de error `0x08` (`ERROR_LINK_LOST`). El calentador está apagado. | | |

Los errores estándar (sobrecalentamiento, rotura de termistor, calentador que no responde) funcionan como en las construcciones analógicas - véase la sección "Qué hacer en caso de error".

#### Comportamiento al perder conexión

Después del encendido, iHeater e iHeater-link pueden iniciarse simultáneamente. El módulo iHeater-link necesita tiempo para inicializar Wi-Fi y conectarse a la fuente de comandos, por lo que en los primeros **20 segundos** después del inicio, el firmware de iHeater **no considera la ausencia de paquetes como un error** y muestra una corriente lenta (paso de 1000 ms) - "esperando el primer paquete".

Si después de estos 20 segundos aún no ha llegado ningún paquete correcto, el firmware entra en `ERROR_LINK_LOST` igual que al perder conexión en modo normal.

Después de que se recibe el primer paquete, se aplica la regla normal: pérdida de paquetes por más de 1.5 segundos → error.

Algoritmo cuando se activa un error:

1. El firmware guarda en flash el código de error `0x08` (`ERROR_LINK_LOST`).
2. Reinicia el controlador.
3. Después del reinicio, el calentador está apagado; durante una nueva ventana de 20 segundos nuevamente una corriente lenta de espera, luego parpadeo lento de todos los LED.
4. Para borrar el error, mantenga presionado el botón MODE al aplicar la alimentación, luego suéltelo. El dispositivo volverá al funcionamiento normal.

#### Botón MODE

En la construcción pulse, el botón MODE **no cambia modos y no inicia la calibración**. Su única función es borrar el error guardado al iniciar el dispositivo. Todos los comandos de calefacción provienen solo de iHeater-link.

---

## Cómo grabar

### Descargar

[Descargue el firmware desde la página de lanzamientos](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### Uso del firmware preparado

El firmware ya está configurado con modos de temperatura de MODE_TEMP_0 a MODE_TEMP_7, descritos arriba, y está diseñado para trabajar con un termistor tipo Generic 3950 (tipo 3 según la lista en `config.h`).

### Uso del modo DFU (USB)
=== "r1"

    1. Instale el jumper BOOT0
    2. Conecte USB, el dispositivo se reconocerá como DFU
    3.1 Cargue el firmware usando STM32CubeProgrammer:

    3.1.1 Indique la ruta al archivo `.bin` del firmware (por ejemplo: `Firmware/iHeater_v1.2.3.bin`)
    3.1.2 Establezca la dirección de carga: `0x08000000`

    3.1.3 Haga clic en "Start Programming"

    o

    3.2 Cargue el firmware a través de la línea de comandos:
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. Retire el jumper y reinicie la alimentación

=== "r1.1"

    1. Mantenga presionado el botón BOOT
    2. Conecte USB, el dispositivo se reconocerá como DFU
    3. Cargue el firmware usando STM32CubeProgrammer:

        3.1.1 Indique la ruta al archivo `.bin` del firmware (por ejemplo: `Firmware/iHeater_v1.2.3.bin`)

        3.1.2 Establezca la dirección de carga: `0x08000000`

        3.1.3 Haga clic en "Start Programming"

        o

        3.2 Cargue el firmware a través de la línea de comandos:
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. Reinicie la alimentación

---

## Cómo usar
=== "r1"

    - **Pulsación corta** del botón MODE - cambio de modos (0-7)
    - **Mantenimiento prolongado (2+ seg)** - reinicio de modo a 0 (OFF)
    - **LED** muestran el modo actual según la tabla anterior
    - **Parpadeo de LED** - modo activo pero temperatura aún no alcanzada
    - **Luz constante** - temperatura objetivo alcanzada

=== "r1.1"

    !!! note "Diferencias de revisiones"
   
        la revisión r1.1 tiene un puerto disparador para activación y desactivación automáticas de la calefacción de la cámara. Al conectar un termistor al puerto disparador, la calefacción se encenderá y apagará de acuerdo con las temperaturas especificadas en config.h, por defecto 45 y 80 para encendido y apagado de la calefacción.

    - **Pulsación corta** del botón MODE - cambio de modos (0-7)
    - **Mantenimiento prolongado (2+ seg)** - reinicio de modo a 0 (OFF)
    - **LED** muestran el modo actual según la tabla anterior
    - **Parpadeo de LED** - modo activo pero temperatura aún no alcanzada
    - **Luz constante** - temperatura objetivo alcanzada

---

## Qué hacer en caso de error

El dispositivo se reinicia automáticamente y muestra el código de error con LED, con el ventilador encendido.


| Código | Descripción                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | El aire no alcanzó la temperatura objetivo a tiempo        |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | Calentador no responde                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | Apagado manual (no utilizado)                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | Termistor de aire desconectado                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | Termistor del calentador desconectado             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | Sobrecalentamiento del calentador                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | Sobrecalentamiento del aire                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | Conexión perdida con iHeater-link (solo construcción pulse) | 0.25 Гц | 0.25 Гц | 0.25 Гц |
| 0xFF  | Error desconocido                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### Borrar error

Mantenga presionado el botón MODE durante el inicio hasta que los LED cambien de indicación. El error se borrará del flash y el dispositivo continuará funcionando normalmente.

## Ajuste fino

En caso de que haya confirmación de que las lecturas del termistor difieren de la temperatura real, puede usar la función de ajuste fino. Para esto, es necesario medir la resistencia de los resistores correspondientes y escribirlos en lugar de los valores predeterminados.


```C++ title="Resistencia real de los resistores de pull-up"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

Este software está licenciado bajo la Licencia Pública General GNU v3.0 solo para uso no comercial.
