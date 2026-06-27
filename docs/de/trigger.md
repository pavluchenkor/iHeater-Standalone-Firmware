# Trigger — automatischer Start von iHeater nach Tischtemperatur

## Was ist das und wofür braucht man es?

Der Trigger ist eine Funktion zur automatischen Ein- und Ausschaltung des iHeater-Heizers beim Start und Ende eines Drucks. iHeater überwacht die Temperatur des 3D-Druckertisches über einen zusätzlichen Thermistor (TH2) und:

- **Schaltet** die Trocknungsanlage ein, wenn der Tisch zu heizen beginnt (Druckstart)
- **Schaltet** die Trocknungsanlage aus, wenn der Tisch abkühlt (Druck beendet)

Sie müssen iHeater nicht manuell ein- und ausschalten — alles läuft automatisch ab.

!!! warning "Anforderungen"
    Die Trigger-Funktion ist nur auf der Platine **iHeater rev 1.1** verfügbar, auf der ein Anschluss für einen dritten Thermistor (TH2) vorhanden ist.

---

## So verbinden Sie es

Installieren Sie einen Thermistor (NTC 100K) auf dem Tisch des 3D-Druckers und verbinden Sie ihn mit dem Anschluss **TH2** auf der iHeater-Platine.

Der Thermistor muss auf einem Metall- oder Aluminium-Teil des Tisches befestigt werden, der Wärme gut leitet — so nah wie möglich an der Heizzone. Ein direktes Anbringen am Heizelement ist nicht möglich, aber je besser der thermische Kontakt zur beheizten Oberfläche ist, desto genauer funktioniert der Trigger. Die Thermistor-Messwerte liegen unter der tatsächlichen Heizer-Temperatur — das ist normal, die Schwellwerte werden für den jeweiligen Installationsort kalibriert.

---

## Wie funktioniert es?

```
Tischtemperatur (°C)
    ^
 90 |         ┌────────────────┐  Druck läuft
 85 |        /                  \
 80 |-------/----AUS iHeater-----\-------- AUS-Schwelle (80°C)
 75 |------/----------------------\------- EIN-Schwelle (75°C)
 50 |     /    iHeater arbeitet    \
 25 |____/                          \____
    └─────────────────────────────────────> Zeit
         Aufheizung     Druck       Abkühlung
```

**Funktionsszenario (EIN-Schwelle = 75°C, AUS-Schwelle = 80°C):**

1. Der Drucker beginnt zu drucken, der Tisch wird heiß
2. Tischtemperatur erreicht **75°C** (EIN-Schwelle) → iHeater **schaltet sich ein** und beginnt mit dem Aufheizen der Kammer
3. Die Tischtemperatur steigt weiter und überschreitet **80°C** (AUS-Schwelle) → iHeater registriert **Zieltemperatur erreicht**
4. Der Druck läuft, der Tisch ist heiß — iHeater hält die Kammertemperatur
5. Der Druck ist beendet, der Tisch kühlt ab
6. Tischtemperatur fällt auf **80°C** (AUS-Schwelle) → iHeater **schaltet sich aus**
7. Der Tisch kühlt bis auf Raumtemperatur ab — das System ist bereit für einen neuen Zyklus

!!! info "Warum die AUS-Schwelle nicht höher als die EIN-Schwelle sein muss"
    Das System funktioniert in **zwei Phasen**, und iHeater kann nur **nach Erreichen des Ziels** ausgeschaltet werden (Schritt 3):

    **Phase 1** — Warten auf Einschalten: iHeater ist aus, das System wartet darauf, dass die Temperatur bis zur EIN-Schwelle ansteigt. Die AUS-Schwelle wird in diesem Stadium **ignoriert**.

    **Phase 2** — nach Zieltemperatur erreicht: Die Temperatur ist über die AUS-Schwelle gestiegen, das Ziel ist registriert. Jetzt, wenn die Temperatur **unter** die AUS-Schwelle fällt — schaltet sich iHeater aus.

    Da die Phasen sequenziell arbeiten und sich nicht überschneiden, kann die AUS-Schwelle sowohl höher als auch niedriger als die EIN-Schwelle sein:

    - **AUS-Schwelle = 80°C, EIN-Schwelle = 75°C** — Der Tisch muss bis 80°C aufgeheizt werden, um das Ziel zu registrieren, Ausschaltung beim Abkühlen auf 80°C
    - **AUS-Schwelle = 70°C, EIN-Schwelle = 75°C** — Das Ziel wird sofort registriert (Tisch ist bereits über 70°C beim Einschalten), Ausschaltung beim Abkühlen auf 70°C

!!! info "Warum iHeater beim Abkühlen nicht erneut einschaltet"
    Wenn der Tisch abkühlt und durch die EIN-Schwelle geht (z. B. 75°C) — schaltet sich iHeater **nicht erneut ein**. Das System ist durch ein Zieltemperatur-Erreicht-Flag geschützt:

    1. iHeater schaltete sich beim Fallen unter die AUS-Schwelle aus (80°C) — aber das Flag für Zieltemperatur-Erreicht **bleibt aktiv**
    2. Der Tisch kühlt weiter ab und geht durch 75°C (EIN-Schwelle) — das Einschalten ist **blockiert**, weil das Flag noch aktiv ist
    3. Die Temperatur fällt **unter** die EIN-Schwelle (unter 75°C) — nur dann wird das Flag **zurückgesetzt**, und das System ist bereit für einen neuen Zyklus

    Daher muss sich der Tisch für einen erneuten Start **vollständig abkühlen** unter die EIN-Schwelle, dann wieder bis dahin aufgeheizt werden — also ein neuer Druck beginnen.

!!! danger "AUS-Schwelle darf nicht in der Heizzone von iHeater liegen"
    Nach dem Ende des Drucks schaltet sich der Druckertisch aus und beginnt abzukühlen. Aber iHeater arbeitet in diesem Moment noch und gibt selbst Wärme ab. Wenn der Thermistor TH2 so angebracht ist, dass iHeater ihn mit seiner Wärmestrahlung aufheizt — kann die Temperatur am Thermistor nicht unter die AUS-Schwelle fallen, und iHeater **schaltet sich nie aus**.

    **Stellen Sie sicher**, dass die AUS-Schwelle **niedriger** als die Temperatur eingestellt ist, die der Thermistor TH2 anzeigt, wenn der Druckertisch ausgeschaltet ist und iHeater arbeitet. Mit anderen Worten — die AUS-Schwelle muss niedriger sein als die Temperatur, auf die iHeater allein den Bereich der Thermistor-Installation aufheizen kann.

---

## Standardeinstellungen

| Parameter | Wert | Beschreibung |
|-----------|------|----------|
| EIN-Schwelle | **75°C** | Tischtemperatur, bei der iHeater einschaltet |
| AUS-Schwelle | **80°C** | Tischtemperatur, bei der iHeater ausschaltet |
| Heizmodus | **MODE 2** (60°C) | Modus, in dem iHeater bei Trigger-Aktivierung läuft |

Wenn der Trigger ausgelöst wird, schaltet sich iHeater im **Modus 2** ein — Ziellufttemperatur 60°C. Anzeige: **LED2** leuchtet.

---

## Kalibrierung der Trigger-Schwellen

Wenn die Standardwerte (75°C / 80°C) nicht für Ihren Drucker geeignet sind, können Sie die Schwellen an Ihre Arbeitstischtemperatur anpassen.

### Wann ist eine Kalibrierung erforderlich

- Sie möchten, dass iHeater früher oder später einschaltet
- Der Thermistor ist weit vom Tisch-Heizer entfernt und zeigt zu niedrige Temperaturen an

### Schritt-für-Schritt-Anleitung

!!! note "Wichtig"
    Während der Kalibrierung ist der iHeater-Heizer **ausgeschaltet** — die Kalibrierung ist sicher.

#### Schritt 1: Kalibrierungsmodus aktivieren

- Stellen Sie sicher, dass iHeater sich im **Standby-Modus** befindet (MODE 0 — alle LED sind aus)
- **Halten Sie die Taste 2+ Sekunden gedrückt** → Kalibrierungsmodus aktiviert
- **Anzeige**: LED1 blinkt schnell (~10 Hz) — Sie befinden sich im Einstellmodus für EIN-Schwelle

#### Schritt 2: EIN-Schwelle einstellen

- Heizen Sie den Druckertisch auf die Temperatur auf, bei der iHeater **einschalten** soll
- Warten Sie auf Temperatonstabilisierung
- **Halten Sie die Taste 2+ Sekunden gedrückt** → Wert gespeichert
- **Anzeige**: LED1 + LED2 blinken schnell — Sie befinden sich im Einstellmodus für AUS-Schwelle

#### Schritt 3: AUS-Schwelle einstellen

- Heizen Sie den Tisch auf eine Temperatur **über** der EIN-Schwelle auf (empfohlen +5°C)
- **Halten Sie die Taste 2+ Sekunden gedrückt** → Wert im Flash-Speicher gespeichert
- **Erfolgsmeldung**: Alle 3 LEDs blinken 3 mal
- iHeater kehrt automatisch in den Standby-Modus (MODE 0) zurück

#### Kalibrierung abbrechen

Auf **jeder Stufe** — **kurz die Taste drücken** → Kalibrierung abgebrochen, vorherige Werte wiederhergestellt.

### Kalibrierungsbeispiel (100°C Tisch für ABS)

| Schritt | Aktion | Tischtemperatur |
|---------|--------|-------------------|
| 1 | Langer Tastendruck in MODE 0 | — |
| 2 | Tisch aufheizen, langer Tastendruck | **75°C** (EIN-Schwelle) |
| 3 | Tisch höher aufheizen, langer Tastendruck | **80°C** (AUS-Schwelle) |

---

## LED-Anzeige

| Zustand | LED1 | LED2 | LED3 | Beschreibung |
|---------|------|------|------|----------|
| Standby (MODE 0) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater ist aus, Trigger wartet |
| Trigger ausgelöst (Aufheizung) | ![off](../img/ball_gifs/black_ball.gif) | ![blink](../img/ball_gifs/blinking_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater heizt bis zur Zieltemperatur auf |
| Trigger ausgelöst (bereit) | ![off](../img/ball_gifs/black_ball.gif) | ![on](../img/ball_gifs/red_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater hat Zieltemperatur erreicht (60°C) |
| Kalibrierung Schritt 1 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | EIN-Schwelle einstellen |
| Kalibrierung Schritt 2 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | AUS-Schwelle einstellen |
| Kalibrierung beendet | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | 3 mal — Erfolg |

---

## Zustandsdiagramm

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  ┌───────────┐     Tischtemperatur ≥ EIN-Schwelle               │
│  │  Standby  │ ─────────────────────────────────┐               │
│  │  (MODE 0) │                                  │               │
│  │  LED: alle│  ◄─────────┐                     ▼               │
│  │ ausgeschaltet│         │          ┌──────────────────┐       │
│  └───────────┘            │          │  iHeater arbeitet│       │
│        ▲                  │          │  (TRIGGER MODE)  │       │
│        │                  │          │  LED2 leuchtet   │       │
│  Tisch abgekühlt          │          └──────────────────┘       │
│  unter EIN-Schwelle       │                     │               │
│  (bereit für neuen        │    Tischtemperatur  │               │
│   Zyklus)                 │    ≤ AUS-Schwelle   │               │
│                           │                     ▼               │
│                  ┌────────┴─────────────────────────┐           │
│                  │  iHeater ausschalten → MODE 0    │           │
│                  └──────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

---

## Häufig gestellte Fragen

**Trigger funktioniert nicht, obwohl der Tisch heiß ist**

- Überprüfen Sie die Verbindung des Thermistors TH2
- Stellen Sie sicher, dass die Tischtemperatur wirklich die EIN-Schwelle erreicht
- Kalibrieren Sie die Schwellen niedriger, wenn Sie mit niedriger Tischtemperatur drucken

**iHeater schaltet sich ein und sofort wieder aus**

- EIN-Schwelle und AUS-Schwelle sind zu nah beieinander. Kalibrieren Sie mit einem Mindestabstand von 5°C

**Ich möchte den Heizmodus bei Trigger-Aktivierung ändern**

- In der aktuellen Firmware ist der Trigger-Modus fest — **MODE 2** (60°C). Ändern ist nur durch Neukompilierung der Firmware möglich (Parameter `TRIGGER_MODE` in `config.h`)
