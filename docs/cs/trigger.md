# Spouštěč — automatické spuštění iHeateru podle teploty podstavy

## Co to je a k čemu slouží

Spouštěč — funkce automatického zapnutí a vypnutí topidla iHeater na začátku a konci tisku. iHeater sleduje teplotu podstavy 3D tiskárny přes dodatečný termistor (TH2) a:

- **Zapíná** sušárnu, když se podstava začíná ohřívat (začátek tisku)
- **Vypíná** sušárnu, když se podstava chladí (tisk je dokončen)

Nemusíte ručně zapínat a vypínat iHeater — vše se происходит automaticky.

!!! warning "Požadavky"
    Funkce spouštěče je k dispozici pouze na desce **iHeater rev 1.1**, kde je přítomen vstup pro třetí termistor (TH2).

---

## Jak připojit

Umístěte termistor (NTC 100K) na podstavu 3D tiskárny a připojte jej na vstup **TH2** na desce iHeater.

Termistor je třeba upevnit na kovové nebo hliníkové části podstavy, která dobře vede teplo — co nejblíže zóně ohřevu. Umístit jej přímo na topné těleso není možné, ale čím lepší je tepelný kontakt s ohřívaným povrchem, tím přesněji bude spouštěč fungovat. Údaje termistoru budou nižší než skutečná teplota topidla — to je normální, prahové hodnoty se kalibrují podle konkrétního místa instalace.

---

## Jak to funguje

```
Teplota podstavy (°C)
    ^
 90 |         ┌────────────────┐  Tisk probíhá
 85 |        /                  \
 80 |-------/----VYPNUTÍ iHeater-\-------- Prahová hodnota vypnutí (80°C)
 75 |------/----------------------\------- Prahová hodnota zapnutí (75°C)
 50 |     /    iHeater pracuje     \
 25 |____/                          \____
    └─────────────────────────────────────> Čas
         Ohřev     Tisk      Chladnutí
```

**Scénář provozu (prahová hodnota zapnutí = 75°C, prahová hodnota vypnutí = 80°C):**

1. Tiskárna zahájí tisk, podstava se ohřívá
2. Teplota podstavy dosáhne **75°C** (prahová hodnota zapnutí) → iHeater se **zapne** a začne ohřívat komoru
3. Teplota podstavy pokračuje v růstu a překročí **80°C** (prahová hodnota vypnutí) → iHeater zaznamená **dosažení cíle**
4. Tisk probíhá, podstava je horká — iHeater udržuje teplotu v komoře
5. Tisk je dokončen, podstava začíná chladnout
6. Teplota podstavy klesne na **80°C** (prahová hodnota vypnutí) → iHeater se **vypne**
7. Podstava chladne na pokojovou teplotu — systém je připraven na nový cyklus

!!! info "Proč prahová hodnota vypnutí nemusí být vyšší než prahová hodnota zapnutí"
    Systém pracuje ve **dvou fázích** a iHeater lze vypnout pouze **po dosažení cíle** (krok 3):

    **Fáze 1** — čekání na zapnutí: iHeater je vypnutý, systém čeká, až teplota stoupne na prahovou hodnotu zapnutí. Prahová hodnota vypnutí je v tomto stadiu **ignorována**.

    **Fáze 2** — po dosažení cíle: teplota prošla prahovou hodnotou vypnutí směrem nahoru, cíl je zaznamenán. Nyní, když teplota **klesne** pod prahovou hodnotu vypnutí — iHeater se vypne.

    Protože fáze fungují postupně a nepřekrývají se, prahová hodnota vypnutí může být jak vyšší, tak nižší než prahová hodnota zapnutí:

    - **Prahová hodnota vypnutí = 80°C, prahová hodnota zapnutí = 75°C** — podstava se musí ohřát na 80°C aby se cíl zaznamenal, vypnutí při chladnutí na 80°C
    - **Prahová hodnota vypnutí = 70°C, prahová hodnota zapnutí = 75°C** — cíl se zaznamená okamžitě (podstava je již nad 70°C v okamžiku zapnutí), vypnutí při chladnutí na 70°C

!!! info "Proč se iHeater znovu nezapne během chladnutí"
    Když se podstava chladí a prochází prahovou hodnotou zapnutí (například 75°C) — iHeater se **znovu nezapne**. Systém je chráněn příznakem dosažení cíle:

    1. iHeater se vypnul při poklesu na prahovou hodnotu vypnutí (80°C) — ale příznak dosažení cíle **zůstává aktivní**
    2. Podstava pokračuje v chladnutí a prochází 75°C (prahová hodnota zapnutí) — zapnutí je **blokováno**, protože příznak je stále aktivní
    3. Teplota klesne **pod** prahovou hodnotu zapnutí (pod 75°C) — pouze pak se příznak **resetuje** a systém je připraven na nový cyklus

    Aby se opět zapnul, musí se podstava **zcela ochladit** pod prahovou hodnotu zapnutí a pak se znovu ohřát — tedy aby začal nový tisk.

!!! danger "Prahová hodnota vypnutí by neměla spadat do zóny ohřevu od iHeateru"
    Po skončení tisku se podstava tiskárny vypne a začíná chladnout. Ale iHeater v tu chvíli ještě pracuje a sám vyzařuje teplo. Pokud je termistor TH2 umístěn tak, že jej iHeater ohřívá svým teplem — teplota na termistoru nemusí klesnout pod prahovou hodnotu vypnutí a iHeater se **nikdy nevypne**.

    **Zkontrolujte**, že prahová hodnota vypnutí je nastavena **pod** teplotu, kterou termistor TH2 ukazuje, když je podstava tiskárny vypnutá a iHeater je v provozu. Jinými slovy — prahová hodnota vypnutí musí být nižší než teplota, na kterou je iHeater sám schopen ohřát zónu instalace termistoru.

---

## Výchozí nastavení

| Parametr | Hodnota | Popis |
|----------|---------|-------|
| Prahová hodnota zapnutí | **75°C** | Teplota podstavy, při které se iHeater zapíná |
| Prahová hodnota vypnutí | **80°C** | Teplota podstavy, při které se iHeater vypíná |
| Režim ohřevu | **MODE 2** (60°C) | Režim, ve kterém iHeater pracuje při spuštění spouštěče |

Při spuštění spouštěče se iHeater zapne v **režimu 2** — cílová teplota vzduchu 60°C. Indikace: svítí **LED2**.

---

## Kalibrace prahových hodnot spouštěče

Pokud výchozí hodnoty (75°C / 80°C) nevyhovují vaší tiskárně, můžete prahové hodnoty upravit podle své pracovní teploty podstavy.

### Kdy je potřeba kalibrace

- Chcete, aby se iHeater zapnul dříve nebo později
- Termistor je umístěn daleko od topidla podstavy a ukazuje nižší teplotu

### Podrobný průvodce

!!! note "Důležité"
    Během kalibrace je topidlo iHeater **vypnuté** — kalibrace je bezpečná.

#### Krok 1: Vstupte do režimu kalibrace

- Zkontrolujte, že iHeater je v **režimu čekání** (MODE 0 — všechny LED jsou vypnuté)
- **Stiskněte tlačítko na 2+ sekundy** → vstup do kalibrace
- **Indikace**: LED1 rychle bliká (~10 Hz) — jste v režimu nastavení prahové hodnoty zapnutí

#### Krok 2: Nastavte prahovou hodnotu zapnutí

- Ohřejte podstavu tiskárny na teplotu, při které chcete, aby se iHeater **zapnul**
- Čekejte na stabilizaci teploty
- **Stiskněte tlačítko na 2+ sekundy** → hodnota je uložena
- **Indikace**: LED1 + LED2 rychle blikají — jste v režimu nastavení prahové hodnoty vypnutí

#### Krok 3: Nastavte prahovou hodnotu vypnutí

- Ohřejte podstavu na teplotu o něco **vyšší** než prahová hodnota zapnutí (doporučuje se +5°C)
- **Stiskněte tlačítko na 2+ sekundy** → hodnota je uložena do flash paměti
- **Indikace úspěchu**: všechny 3 LED třikrát zablikají
- iHeater se automaticky vrátí do režimu čekání (MODE 0)

#### Zrušení kalibrace

V **kterémkoli kroku** — **krátce stiskněte tlačítko** → kalibrace je zrušena, předchozí hodnoty jsou obnoveny.

### Příklad kalibrace (podstava 100°C pro ABS)

| Krok | Akce | Teplota podstavy |
|------|------|------------------|
| 1 | Dlouhé stisknutí v MODE 0 | — |
| 2 | Ohřeji podstavu, dlouhé stisknutí | **75°C** (prahová hodnota zapnutí) |
| 3 | Ohřeji podstavu výš, dlouhé stisknutí | **80°C** (prahová hodnota vypnutí) |

---

## Indikace LED

| Stav | LED1 | LED2 | LED3 | Popis |
|------|------|------|------|-------|
| Čekání (MODE 0) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater je vypnutý, spouštěč čeká |
| Spouštěč se spustil (ohřev) | ![off](../img/ball_gifs/black_ball.gif) | ![blink](../img/ball_gifs/blinking_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater se ohřívá na cílovou teplotu |
| Spouštěč se spustil (připraven) | ![off](../img/ball_gifs/black_ball.gif) | ![on](../img/ball_gifs/red_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater dosáhl cílové teploty (60°C) |
| Kalibrace krok 1 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | Nastavení prahové hodnoty zapnutí |
| Kalibrace krok 2 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | Nastavení prahové hodnoty vypnutí |
| Kalibrace dokončena | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | 3krát — úspěch |

---

## Stavový diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  ┌───────────┐     Teplota podstavy ≥ prahová hodnota zapnutí   │
│  │  Čekání   │ ─────────────────────────────────┐               │
│  │  (MODE 0) │                                  │               │
│  │  LED: všechny│  ◄─────────┐                  ▼               │
│  │  vypnuté  │            │          ┌──────────────────┐       │
│  └───────────┘            │          │  iHeater pracuje │       │
│        ▲                  │          │  (TRIGGER MODE)  │       │
│        │                  │          │  LED2 svítí      │       │
│  Podstava se chladí       │          └──────────────────┘       │
│  pod prahovou hodnotou    │                     │               │
│  zapnutí (připraven       │    Teplota podstavy│               │
│   na nový cyklus)         │    ≤ prahová hodnota│               │
│                           │       vypnutí       │               │
│                           │                     ▼               │
│                  ┌────────┴─────────────────────────┐           │
│                  │  iHeater se vypne → MODE 0      │           │
│                  └──────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

---

## Často kladené otázky

**Spouštěč se nespustí, i když je podstava horká**

- Zkontrolujte připojení termistoru TH2
- Ujistěte se, že teplota podstavy opravdu dosahuje prahové hodnoty zapnutí
- Zkalibrujte nižší prahové hodnoty, pokud tiskujete s nižší teplotou podstavy

**iHeater se zapne a ihned vypne**

- Prahová hodnota zapnutí a prahová hodnota vypnutí jsou příliš blízko sebe. Zkalibrujte s rozdílem minimálně 5°C

**Chci změnit režim ohřevu při spuštění spouštěče**

- V aktuální firmwaru je režim spouštěče pevně stanoven — **MODE 2** (60°C). Změnit jej lze pouze překompilováním firmwaru (parametr `TRIGGER_MODE` v `config.h`)
