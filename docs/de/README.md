<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# Standalone iHeater Firmware

iHeater ist eine kompakte und kostengünstige Lösung für 3D-Drucker ohne aktive Druckkammer oder mit begrenzten Anschlussmöglichkeiten am Hauptboard. Sie ist besonders nützlich für Modelle mit proprietären Leiterplatten, bei denen keine freien Anschlüsse für Lüfter, Heizer oder Thermistor vorhanden sind. iHeater löst diese Aufgabe auf einfache und universelle Weise.

Kann eigenständig mit eigener Firmware arbeiten als separates Gerät oder unter Klipper-Steuerung mit USB-Verbindung zum Drucker verbunden

![iHeater](../img/iHeater_200.png)

---

## Über die Firmware

**iHeater** ist eine Firmware für ein autonomes Gerät zur Temperaturregelung einer 3D-Drucker-Druckkammer.

---

## Zweck

Das Gerät gewährleistet das Heizen und die Aufrechterhaltung einer stabilen Temperatur innerhalb der 3D-Drucker-Druckkammer, was für den Druck von ABS, ASA und anderen temperaturabhängigen Kunststoffen mit hoher Schrumpfung und niedriger Schichthaftung entscheidend ist.

---

## Konfiguration der Firmware

### Datei `config.h`

Diese Datei enthält die Parameter für den Gerätebetrieb:
Die Datei befindet sich im Ordner Core/Inc/config.h

| Parameter                                    | Zweck                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | Thermistor-Typ auswählen (0-5, siehe unten)                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                 | Temperaturen für jeden Modus. Sie können jeden Wert eingeben, einschließlich Dezimalwerte, z. B. `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | Zulässiger Temperaturbereich des Heizelements                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | Lufttemperaturbereich                                                                         |
| `HEATER_MIN_PWM`                            | Minimales PWM, bei dem das System die Reaktion des Heizelements überprüft                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | Wartezeit für Temperaturanstieg des Heizelements (Standard: 5 Sek.)                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | Maximale Zeit zum Erreichen der Ziellufttemperatur (Standard: 10 Min.)                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | PWM-Frequenz und -Qualität einstellen                                                                    |

### Thermistoren

Werden in `SELECTED_THERMISTOR_TYPE` ausgewählt. Unterstützt:

| ID | Typ                          | Kommentar                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | E3D-Standard                       |
| 1  | ATC Semitec 104NT-4-R025H42G | 104GT-Analogon                          |
| 2  | EPCOS 100K B57560G104F       | Beliebt                             |
| 3  | Generic 3950                 | **Standard**, weit verbreitet |
| 4  | SliceEngineering 450         | Unterstützung hoher Temperaturen          |
| 5  | TDK NTCG104LH104JT1          | Medizinisch, präzise                   |

### Temperatureschemata und LEDs

Der Modus wird durch einen Temperaturwert gesetzt und wird mit drei LEDs binär angezeigt:

Beim Einschalten des Modus blinken die LEDs, nach Erreichen des Modus leuchten sie konstant

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
### Automatisches Ein- und Ausschalten r1.1

Makros
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

Steuern die Temperatur zum Ein- und Ausschalten des Heizelements sowie den Modus beim Einschalten.

Angenommen, der Trigger-Thermistor befindet sich in der Nähe des Heizbetts.
Mit den aktuellen Einstellungen wird die Druckkammer beheizt, wenn die Betttempeeratur 45°C erreicht, und ausgeschaltet, wenn die Temperatur unter 80°C fällt. Diese Parameter sind für die meisten Kunststoffe, die eine aktive Druckkammer erfordern, geeignet und verursachen keine logischen Konflikte im Betrieb.

Sie können auch den Modus auswählen, der beim Auslösen des Triggers gestartet wird. Zum Beispiel ist derzeit TRIGGER_MODE MODE_2 ausgewählt, was bedeutet, dass MODE_TEMP_2 gestartet wird – eine Lufttemperatur in der Kammer von 60,0°C.

---
### Betrieb mit iHeater-link (Build r1.1 pulse)

Der Build `iheater_rev1_1_pulse` ist für die Zusammenarbeit mit dem iHeater-link Modul auf ESP konzipiert. Das Modul empfängt Befehle vom Drucker, Portal oder einer mobilen App und konvertiert die Kammer-Solltemperatur in iHeater über eine GPIO-Leitung. Die iHeater-Firmware hängt nicht von der Druckerseite ab und kennt das Transportprotokoll nicht – sie empfängt nur eine Zahl: die gewünschte Lufttemperatur in der Kammer in °C.

#### Verbindung

1. Flashen Sie iHeater mit dem Build `iheater_rev1_1_pulse` (siehe Abschnitt „Wie wird geflasht").
2. Verbinden Sie das Signalkabel von iHeater-link mit dem **TH2**-Anschluss des iHeater-Boards (Pin PB1).
3. Eine gemeinsame GND-Verbindung zwischen ESP und iHeater ist erforderlich – verwenden Sie einen separaten Erdungsdraht.
4. Lassen Sie den Heizer-Thermistor an seinem Standard-Anschluss **TH0**. Der Luft-Thermistor – an **TH1**. Sie funktionieren wie in jedem anderen Build.
5. Der Trigger-Anschluss wird in diesem Build nicht verwendet (seine Leitung ist mit dem digitalen Eingang belegt).

!!! warning "Verbinden oder trennen Sie das Signalkabel niemals bei angeschlossener Stromversorgung."

#### Empfangene Befehle

| Code von iHeater-link | Bedeutung |
|---------------------|--------|
| `0`                 | Heizung ausschalten |
| `10`                | Heizung ausschalten (vereinbarter "Off-Code") |
| `45..90`            | Heizung einschalten, Sollwert für die Luft in °C entspricht dem Code |
| andere Werte        | Werden ignoriert, vorheriger Status bleibt erhalten |

Das Minimum von `45 °C` wird durch das Makro `PULSE_MIN_TARGET` in `config.h` gesetzt. Das Maximum wird durch `MAX_AIR_TEMP` begrenzt (Standard: 90 °C).

#### LED-Anzeige

| Status | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| Warten auf erstes Paket nach dem Einschalten (bis 20 s) | laufendes Licht LED1 → LED2 → LED3 in Schritten von 1000 ms | | |
| Verbindung OK, Heizung ausgeschaltet | leuchtet konstant (Heartbeat "Verbindung aktiv") | ausgeschaltet | kurzer Blitz ~80 ms bei jedem empfangenen Paket |
| Verbindung OK, Heizung eingeschaltet | laufendes Licht LED1 → LED2 → LED3 in Schritten von 500 ms | | |
| Verbindung verloren (mehr als 1,5 s ohne Paket nach erster Verbindung) | Alle drei blinken synchron; Gerät startet mit dem Fehlercode `0x08` (`ERROR_LINK_LOST`) neu. Heizer ausgeschaltet. | | |

Standardfehler (Übertemperatur, Thermistor-Unterbrechung, Heizer reagiert nicht) funktionieren genauso wie in analogen Builds – siehe Abschnitt „Was tun im Fehlerfall".

#### Verhalten bei Verbindungsverlust

Nach dem Einschalten des iHeater und iHeater-link können beide gleichzeitig starten. Das iHeater-link Modul benötigt Zeit zur Wi-Fi-Initialisierung und zum Verbinden mit der Befehlsquelle. Daher berücksichtigt die Firmware in den ersten **20 Sekunden** nach dem Start **fehlende Pakete nicht als Fehler** und zeigt ein langsames laufendes Licht (Schritte von 1000 ms) – „Warte auf erstes Paket".

Wenn während dieser 20 Sekunden kein einziges korrektes Paket ankommt, geht die Firmware in `ERROR_LINK_LOST` genauso wie beim Verbindungsabbruch im Normalmodus.

Nachdem das erste Paket empfangen wurde, gilt die übliche Regel: Fehlende Pakete länger als 1,5 Sekunden → Fehler.

Algorithmus beim Fehler:

1. Die Firmware speichert den Fehlercode `0x08` (`ERROR_LINK_LOST`) im Flash.
2. Startet den Controller neu.
3. Nach dem Neustart ist der Heizer ausgeschaltet; während des neuen 20-Sekunden-Fensters wieder laufendes Licht zum Warten, danach – langsames Blinken aller LEDs.
4. Zum Zurücksetzen des Fehlers halten Sie die MODE-Taste gedrückt, während Sie das Gerät einschalten, und lassen Sie sie los. Das Gerät kehrt zur Normalbetrieb zurück.

#### MODE-Taste

Im Pulse-Build schaltet die MODE-Taste **nicht zwischen Modi um und startet keine Kalibrierung**. Ihre einzige Funktion ist das Zurücksetzen des gespeicherten Fehlers beim Gerätestart. Alle Heizungsbefehle stammen nur von iHeater-link.

---

## Wie wird geflasht

### Download

[Laden Sie die Firmware von der Seite mit den Versionen herunter](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### Verwendung vorkonfigurierter Firmware

Die Firmware ist bereits mit Temperatureschemata von MODE_TEMP_0 bis MODE_TEMP_7 konfiguriert, wie oben beschrieben, und ist für die Arbeit mit dem Generic 3950-Thermistor (Typ 3 in der Liste in `config.h`) bestimmt.

### DFU-Modus verwenden (USB)
=== "r1"

    1. Setzen Sie den Jumper BOOT0
    2. Verbinden Sie USB, das Gerät wird als DFU erkannt
    3.1 Laden Sie die Firmware mit STM32CubeProgrammer:

    3.1.1 Geben Sie den Pfad zur `.bin`-Firmwaredatei an (z. B.: `Firmware/iHeater_v1.2.3.bin`)
    3.1.2 Stellen Sie die Load-Adresse ein: `0x08000000`

    3.1.3 Klicken Sie auf "Start Programming"

    oder

    3.2 Laden Sie die Firmware über die Befehlszeile:
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. Entfernen Sie den Jumper und starten Sie die Stromversorgung neu

=== "r1.1"

    1. Halten Sie die BOOT-Taste gedrückt
    2. Verbinden Sie USB, das Gerät wird als DFU erkannt
    3. Laden Sie die Firmware mit STM32CubeProgrammer:

        3.1.1 Geben Sie den Pfad zur `.bin`-Firmwaredatei an (z. B.: `Firmware/iHeater_v1.2.3.bin`)

        3.1.2 Stellen Sie die Load-Adresse ein: `0x08000000`

        3.1.3 Klicken Sie auf "Start Programming"

        oder

        3.2 Laden Sie die Firmware über die Befehlszeile:
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. Starten Sie die Stromversorgung neu

---

## Bedienung

=== "r1"

    - **Kurzer Tastendruck** auf MODE - Modi umschalten (0-7)
    - **Langes Drücken (2+ Sek.)** - Modus auf 0 (OFF) zurücksetzen
    - **LEDs** zeigen den aktuellen Modus gemäß der obigen Tabelle an
    - **LED-Blinken** - Modus aktiv, Zieltemperatur aber nicht erreicht
    - **Konstantes Licht** - Zieltemperatur erreicht

=== "r1.1"

    !!! note "Unterschiede zwischen Revisionen"
   
        Revision r1.1 hat einen Trigger-Anschluss zum automatischen Ein- und Ausschalten der Druckkammerheizung. Wenn ein Thermistor an den Trigger-Anschluss angeschlossen wird, wird die Heizung ein- und ausgeschaltet, je nach den in config.h angegebenen Temperaturen. Standardmäßig sind dies 45 und 80 Grad zum Ein- und Ausschalten der Heizung.

    - **Kurzer Tastendruck** auf MODE - Modi umschalten (0-7)
    - **Langes Drücken (2+ Sek.)** - Modus auf 0 (OFF) zurücksetzen
    - **LEDs** zeigen den aktuellen Modus gemäß der obigen Tabelle an
    - **LED-Blinken** - Modus aktiv, Zieltemperatur aber nicht erreicht
    - **Konstantes Licht** - Zieltemperatur erreicht

---

## Was tun im Fehlerfall

Das Gerät startet automatisch neu und zeigt den Fehlercode mit LEDs an. Der Lüfter ist dabei eingeschaltet.


| Code  | Beschreibung                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | Luft hat Zieltemperatur nicht rechtzeitig erreicht        |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | Heizer reagiert nicht                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | Manuelles Herunterfahren (nicht verwendet)                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | Luft-Thermistor getrennt                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | Heizer-Thermistor getrennt             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | Heizer-Übertemperatur                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | Luft-Übertemperatur                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | Verbindung zu iHeater-link verloren (nur Pulse-Build) | 0.25 Hz | 0.25 Hz | 0.25 Hz |
| 0xFF  | Unbekannter Fehler                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### Fehler zurücksetzen

Halten Sie die MODE-Taste beim Start gedrückt, bis sich die LED-Anzeige ändert. Der Fehler wird aus dem Flash gelöscht und das Gerät fährt mit dem normalen Betrieb fort.

## Feinabstimmung

Für den Fall, dass Bestätigung besteht, dass die Thermistor-Messwerte von der tatsächlichen Temperatur abweichen, können Sie die Feinabstimmungsfunktion nutzen. Dazu müssen Sie den Widerstand der entsprechenden Widerstände messen und diese anstelle der Standardwerte eintragen


```C++ title="Actual resistance of the pull-up resistors"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

Diese Software ist unter der GNU General Public License v3.0 nur für die nichtkommerzielle Nutzung lizenziert.
