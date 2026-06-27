# Disparador — encendido automático de iHeater por temperatura de la cama

## Qué es y para qué sirve

El disparador es una función de encendido y apagado automático del calentador iHeater al inicio y al final de la impresión. iHeater monitorea la temperatura de la cama de la impresora 3D a través de un termistor adicional (TH2) y:

- **Enciende** la secadora cuando la cama comienza a calentarse (inicio de impresión)
- **Apaga** la secadora cuando la cama se enfría (impresión finalizada)

No necesita encender y apagar iHeater manualmente — todo ocurre automáticamente.

!!! warning "Requisitos"
    La función disparador solo está disponible en la placa **iHeater rev 1.1**, que tiene una entrada para un tercer termistor (TH2).

---

## Cómo conectar

Instale un termistor (NTC 100K) en la cama de su impresora 3D y conéctelo a la entrada **TH2** en la placa iHeater.

El termistor debe fijarse en una parte de metal o aluminio de la cama que conduzca bien el calor — lo más cerca posible de la zona de calentamiento. No es posible fijarlo directamente en el elemento calefactor, pero cuanto mejor sea el contacto térmico con la superficie calentada, más preciso será el disparador. Las lecturas del termistor serán más bajas que la temperatura real del calentador — esto es normal, los umbrales se calibran para el lugar específico de instalación.

---

## Cómo funciona

```
Temperatura de la cama (°C)
    ^
 90 |         ┌────────────────┐  Impresión en curso
 85 |        /                  \
 80 |-------/----APAGAR iHeater--\-------- Umbral de desactivación (80°C)
 75 |------/----------------------\------- Umbral de activación (75°C)
 50 |     /    iHeater trabajando   \
 25 |____/                          \____
    └─────────────────────────────────────> Tiempo
         Calentamiento   Impresión    Enfriamiento
```

**Escenario de funcionamiento (umbral de activación = 75°C, umbral de desactivación = 80°C):**

1. La impresora comienza a imprimir, la cama se calienta
2. La temperatura de la cama alcanza **75°C** (umbral de activación) → iHeater **se enciende** y comienza a calentar la cámara
3. La temperatura de la cama sigue aumentando y pasa **80°C** (umbral de desactivación) → iHeater registra el **objetivo alcanzado**
4. La impresión continúa, la cama está caliente — iHeater mantiene la temperatura de la cámara
5. La impresión termina, la cama comienza a enfriarse
6. La temperatura de la cama cae a **80°C** (umbral de desactivación) → iHeater **se apaga**
7. La cama se enfría a temperatura ambiente — el sistema está listo para un nuevo ciclo

!!! info "Por qué el umbral de desactivación no tiene que ser más alto que el umbral de activación"
    El sistema funciona en **dos fases**, y iHeater solo puede apagarse **después de alcanzar el objetivo** (paso 3):

    **Fase 1** — esperando activación: iHeater está apagado, el sistema espera a que la temperatura suba al umbral de activación. El umbral de desactivación en esta etapa es **ignorado**.

    **Fase 2** — después de alcanzar el objetivo: la temperatura pasó por el umbral de desactivación hacia arriba, el objetivo está registrado. Ahora, cuando la temperatura **baje** por debajo del umbral de desactivación — iHeater se apagará.

    Dado que las fases funcionan secuencialmente y no se solapan, el umbral de desactivación puede ser tanto más alto como más bajo que el umbral de activación:

    - **Umbral de desactivación = 80°C, umbral de activación = 75°C** — la cama debe calentarse a 80°C para que el objetivo se registre, apagado al enfriarse a 80°C
    - **Umbral de desactivación = 70°C, umbral de activación = 75°C** — el objetivo se registra inmediatamente (la cama ya está por encima de 70°C al activarse), apagado al enfriarse a 70°C

!!! info "Por qué iHeater no se enciende nuevamente al enfriarse"
    Cuando la cama se enfría y pasa por el umbral de activación (por ejemplo, 75°C) — iHeater **no se encenderá nuevamente**. El sistema está protegido por una bandera de objetivo alcanzado:

    1. iHeater se apagó al caer por debajo del umbral de desactivación (80°C) — pero la bandera de objetivo alcanzado **permanece activa**
    2. La cama continúa enfriándose y pasa 75°C (umbral de activación) — el encendido está **bloqueado** porque la bandera sigue activa
    3. La temperatura cae **por debajo** del umbral de activación (por debajo de 75°C) — solo entonces la bandera se **restablece**, y el sistema está listo para un nuevo ciclo

    Por lo tanto, para un nuevo encendido, la cama debe **enfriarse completamente** por debajo del umbral de activación, y luego calentarse nuevamente — es decir, debe comenzar una nueva impresión.

!!! danger "El umbral de desactivación no debe estar en la zona de calentamiento de iHeater"
    Después de que termina la impresión, la cama de la impresora se apaga y comienza a enfriarse. Pero iHeater en este momento aún funciona y emite calor por radiación. Si el termistor TH2 está ubicado de tal manera que iHeater lo calienta con su calor — la temperatura en el termistor podría no caer por debajo del umbral de desactivación, e iHeater **nunca se apagará**.

    **Asegúrese** de que el umbral de desactivación se establezca **por debajo** de la temperatura que el termistor TH2 muestra cuando la cama de la impresora está apagada pero iHeater está funcionando. En otras palabras — el umbral de desactivación debe estar por debajo de la temperatura a la que iHeater puede calentar por sí solo la zona de instalación del termistor.

---

## Configuración por defecto

| Parámetro | Valor | Descripción |
|-----------|-------|-------------|
| Umbral de activación | **75°C** | Temperatura de la cama a la cual iHeater se enciende |
| Umbral de desactivación | **80°C** | Temperatura de la cama a la cual iHeater se apaga |
| Modo de calentamiento | **MODE 2** (60°C) | Modo en el que funciona iHeater cuando se dispara el activador |

Cuando se dispara el disparador, iHeater se enciende en **modo 2** — temperatura objetivo del aire 60°C. Indicación: **LED2** encendido.

---

## Calibración de umbrales del disparador

Si los valores por defecto (75°C / 80°C) no son apropiados para su impresora, puede ajustar los umbrales a su temperatura de cama de trabajo.

### Cuándo se necesita calibración

- Desea que iHeater se encienda antes o después
- El termistor está ubicado lejos del calentador de la cama y muestra una temperatura baja

### Instrucción paso a paso

!!! note "Importante"
    Durante la calibración, el calentador iHeater está **apagado** — la calibración es segura.

#### Paso 1: Ingrese al modo de calibración

- Asegúrese de que iHeater esté en **modo de espera** (MODE 0 — todos los LED apagados)
- **Mantenga pulsado el botón durante 2+ segundos** → ingreso a calibración
- **Indicación**: LED1 parpadea rápidamente (~10 Hz) — está en modo de configuración del umbral de activación

#### Paso 2: Establezca el umbral de activación (ACTIVACIÓN)

- Caliente la cama de la impresora hasta la temperatura a la cual desea que iHeater **se encienda**
- Espere a que la temperatura se estabilice
- **Mantenga pulsado el botón durante 2+ segundos** → valor guardado
- **Indicación**: LED1 + LED2 parpadean rápidamente — está en modo de configuración del umbral de desactivación

#### Paso 3: Establezca el umbral de desactivación (DESACTIVACIÓN)

- Caliente la cama hasta una temperatura un poco **más alta** que el umbral de activación (se recomienda +5°C)
- **Mantenga pulsado el botón durante 2+ segundos** → valor guardado en memoria flash
- **Indicación de éxito**: los 3 LED parpadean 3 veces
- iHeater volverá automáticamente al modo de espera (MODE 0)

#### Cancelación de calibración

En **cualquier paso** — **pulse brevemente el botón** → calibración cancelada, valores anteriores restaurados.

### Ejemplo de calibración (cama 100°C para ABS)

| Paso | Acción | Temperatura de la cama |
|------|--------|------------------------|
| 1 | Pulsación larga en MODE 0 | — |
| 2 | Calentar cama, pulsación larga | **75°C** (umbral de activación) |
| 3 | Calentar cama más, pulsación larga | **80°C** (umbral de desactivación) |

---

## Indicación de LED

| Estado | LED1 | LED2 | LED3 | Descripción |
|--------|------|------|------|-------------|
| Espera (MODE 0) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater apagado, disparador en espera |
| Disparador activado (calentamiento) | ![off](../img/ball_gifs/black_ball.gif) | ![blink](../img/ball_gifs/blinking_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater se calienta hasta la temperatura objetivo |
| Disparador activado (listo) | ![off](../img/ball_gifs/black_ball.gif) | ![on](../img/ball_gifs/red_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater alcanzó la temperatura objetivo (60°C) |
| Calibración paso 1 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | Configuración del umbral de activación |
| Calibración paso 2 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | Configuración del umbral de desactivación |
| Calibración completada | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | 3 veces — éxito |

---

## Diagrama de estados

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  ┌───────────┐     Temperatura de cama ≥ umbral de activación   │
│  │  Espera   │ ─────────────────────────────────┐               │
│  │ (MODE 0)  │                                  │               │
│  │ LED: todos│  ◄─────────┐                     ▼               │
│  │  apagados │            │          ┌──────────────────┐       │
│  └───────────┘            │          │  iHeater trabaja │       │
│        ▲                  │          │  (TRIGGER MODE)  │       │
│        │                  │          │  LED2 encendido  │       │
│  Cama enfriada            │          └──────────────────┘       │
│  por debajo del           │                     │               │
│  umbral de activación     │    Temperatura de   │               │
│  (lista para nuevo        │    cama ≤ umbral    │               │
│   ciclo)                  │    de desactivación │               │
│                           │                     ▼               │
│                  ┌────────┴─────────────────────────┐           │
│                  │  iHeater se apaga → MODE 0       │           │
│                  └──────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

---

## Preguntas frecuentes

**El disparador no se activa aunque la cama esté caliente**

- Compruebe la conexión del termistor TH2
- Asegúrese de que la temperatura de la cama realmente alcanza el umbral de activación
- Calibre los umbrales más bajos si imprime con temperaturas bajas de cama

**iHeater se enciende e inmediatamente se apaga**

- El umbral de activación y el umbral de desactivación están demasiado cerca. Calibre con una diferencia mínima de 5°C

**Quiero cambiar el modo de calentamiento cuando se dispara el disparador**

- En el firmware actual, el modo de disparador es fijo — **MODE 2** (60°C). Solo puede cambiarse recompilando el firmware (parámetro `TRIGGER_MODE` en `config.h`)
