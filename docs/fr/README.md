<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# Firmware iHeater Autonome

iHeater est une solution compacte et abordable pour les imprimantes 3D sans chambre thermique active ou avec des possibilités de connexion limitées à la carte mère. Elle est particulièrement utile pour les modèles avec des cartes propriétaires où il n'y a pas de connecteurs libres pour le ventilateur, le radiateur ou la thermistance. iHeater résout ce problème de façon simple et universelle.


Peut fonctionner de manière autonome avec son propre firmware en tant que dispositif séparé ou sous le contrôle de Klipper avec connexion à l'imprimante par USB

![iHeater](../img/iHeater_200.png)

---

## À propos du firmware

**iHeater** est un firmware pour un dispositif de contrôle autonome de la température de la chambre d'une imprimante 3D.

---

## Objectif

Le dispositif assure le chauffage et le maintien d'une température stable à l'intérieur de la chambre de l'imprimante 3D, ce qui est critique pour l'impression d'ABS, d'ASA et d'autres plastiques dépendant de la température avec un retrait élevé et une adhésion inter-couches faible.

---

## Comment configurer le firmware

### Fichier `config.h`

Ce fichier définit les paramètres de fonctionnement du dispositif :
Le fichier se trouve dans le dossier Core/Inc/config.h

| Paramètre                                    | Objectif                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | Sélection du type de thermistance (0-5, voir ci-dessous)                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | Températures pour chaque mode. Vous pouvez définir n'importe quelle valeur, y compris les décimales, par exemple `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | Plage de température admissible du radiateur                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | Plage de température de l'air                                                                         |
| `HEATER_MIN_PWM`                            | PWM minimum auquel le système commencera à vérifier la réponse du radiateur                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | Délai d'attente pour l'augmentation de la température du radiateur (5 secondes par défaut)                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | Temps maximum pour atteindre la température cible de l'air (10 minutes par défaut)                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | Configuration de la fréquence et de la qualité du PWM                                                                    |

### Thermistances

Sélectionnées dans `SELECTED_THERMISTOR_TYPE`. Pris en charge :

| ID | Type                          | Commentaire                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | Standard E3D                       |
| 1  | ATC Semitec 104NT-4-R025H42G | Analogue 104GT                          |
| 2  | EPCOS 100K B57560G104F       | Populaire                             |
| 3  | Generic 3950                 | **Par défaut**, largement utilisé |
| 4  | SliceEngineering 450         | Support des hautes températures          |
| 5  | TDK NTCG104LH104JT1          | Médical, précis                   |

### Modes de température et LED

Le mode est défini par la valeur de température et s'affiche en binaire via trois LED :

Au démarrage du mode, les LED clignotent ; à la fin, elles brillent en continu.

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
### Activation et désactivation automatiques r1.1

Macros
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

Sont responsables de la température d'activation et de désactivation du radiateur ainsi que du mode lors de l'activation.

Supposons que la thermistance de déclenchement se trouve près du radiateur de la table.
Avec les paramètres actuels, le chauffage de la chambre s'activera lorsque la température de la table atteindra 45 et se désactivera lorsque la température de la table tombera en dessous de 80. Ces paramètres conviennent à la plupart des plastiques nécessitant une chambre thermique active et ne provoquent pas de conflits logiques lors du fonctionnement.

Vous pouvez également sélectionner le mode qui sera lancé au moment du déclenchement. Par exemple, actuellement `TRIGGER_MODE MODE_2` est sélectionné, ce qui signifie que le mode `MODE_TEMP_2` sera lancé - température de la chambre 60.0°C

---
### Fonctionnement avec iHeater-link (build r1.1 pulse)

Le build `iheater_rev1_1_pulse` est conçu pour fonctionner avec le module iHeater-link sur ESP. Le module reçoit des commandes de l'imprimante, du portail ou de l'application mobile et traduit la consigne de la chambre en iHeater via une seule ligne GPIO. Le firmware iHeater ne dépend pas du côté imprimante et ne connaît pas le transport - il ne reçoit qu'un nombre : la température d'air cible souhaitée dans la chambre en °C.

#### Connexion

1. Flashez iHeater avec le build `iheater_rev1_1_pulse` (voir la section « Comment flasher »).
2. Connectez le fil de signal de iHeater-link au connecteur **TH2** de la carte iHeater (broche PB1).
3. Une terre commune entre ESP et iHeater est obligatoire - utilisez un fil de terre séparé.
4. Laissez la thermistance du radiateur sur son connecteur standard **TH0**. La thermistance de l'air sur **TH1**. Elles fonctionnent comme dans tout autre build.
5. Le connecteur de déclenchement n'est pas utilisé dans ce build (sa ligne est occupée par une entrée numérique).

!!! warning "Ne connectez jamais et ne déconnectez jamais le fil de signal sous tension."

#### Commandes reçues

| Code de iHeater-link | Signification |
|---------------------|--------|
| `0`                 | Désactiver le chauffage |
| `10`                | Désactiver le chauffage (code « off » convenu) |
| `45..90`            | Activer le chauffage, la consigne d'air en °C égale au code |
| autres valeurs  | Ignorées, l'état précédent est conservé |

Le minimum `45 °C` est défini dans `config.h` par la macro `PULSE_MIN_TARGET`. Le maximum est limité par `MAX_AIR_TEMP` (90 °C par défaut).

#### Indication LED

| État | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| Attente du premier paquet après activation | feu clignotant LED1 → LED2 → LED3 avec étape de 1000 ms | | |
| Connexion OK, chauffage désactivé | allumée en permanence (battement de cœur « connexion active ») | éteinte | courte impulsion ~80 ms pour chaque paquet reçu |
| Connexion OK, chauffage activé | feu clignotant LED1 → LED2 → LED3 avec étape de 200 ms | | |
| Connexion perdue (plus de 5 s sans paquet après la première connexion) | « Balise » : les trois LED s'allument ensemble pendant ~100 ms toutes les 2 s. Le radiateur est désactivé, le ventilateur est activé. | | |

Les erreurs standards (surchauffe, thermistance cassée, radiateur ne répond pas) fonctionnent comme dans les builds analogues - voir la section « Que faire en cas d'erreur ».

#### Comportement en cas de perte de connexion

Le firmware coupe le chauffage lorsque les commandes d'iHeater-link cessent d'arriver et reprend le fonctionnement lorsqu'elles arrivent à nouveau.

Après la mise sous tension, le firmware attend le premier paquet et affiche un feu clignotant lent (étape de 1000 ms). Le radiateur est désactivé. L'attente n'est pas limitée dans le temps : tant que la connexion n'a jamais été établie, l'absence de paquets n'est pas considérée comme une perte de connexion.

Une fois le premier paquet reçu, la règle suivante s'applique : si aucun paquet n'arrive pendant plus de **5 secondes**, la connexion est considérée comme perdue. Pendant ces 5 secondes, iHeater continue d'exécuter la dernière commande.

En cas de perte de connexion :

1. Le radiateur est désactivé.
2. Le ventilateur est activé et fonctionne jusqu'au rétablissement de la connexion.
3. Les LED affichent la « balise » : les trois LED s'allument pendant ~100 ms toutes les 2 s.

Aucune erreur n'est enregistrée dans la flash, le contrôleur ne redémarre pas et il n'est pas nécessaire d'appuyer sur le bouton MODE. Lorsque les paquets arrivent à nouveau, iHeater exécute la commande reçue : avec une consigne `45..90`, il reprend le chauffage ; avec `0` ou `10`, il reste désactivé.

#### Bouton MODE

Dans le build pulse, le bouton MODE **ne bascule pas les modes et ne lance pas l'étalonnage**. Sa seule fonction est de réinitialiser l'erreur sauvegardée au démarrage de l'appareil. Toutes les commandes de chauffage proviennent uniquement de iHeater-link.

---

## Comment flasher

### Télécharger

[Téléchargez le firmware depuis la page des versions](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### Utilisation du firmware préconfiguré

Le firmware est déjà configuré avec les modes de température de MODE_TEMP_0 à MODE_TEMP_7, décrits ci-dessus, et est conçu pour fonctionner avec la thermistance Generic 3950 (type 3 dans la liste `config.h`).

### Utilisation du mode DFU (USB)
=== "r1"

    1. Installez le cavalier BOOT0
    2. Connectez l'USB, l'appareil sera reconnu comme DFU
    3.1 Chargez le firmware à l'aide de STM32CubeProgrammer :

    3.1.1 Spécifiez le chemin du fichier `.bin` du firmware (par exemple : `Firmware/iHeater_v1.2.3.bin`)
    3.1.2 Définissez l'adresse de téléchargement : `0x08000000`

    3.1.3 Cliquez sur « Start Programming »

    ou

    3.2 Chargez le firmware via la ligne de commande :
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. Retirez le cavalier et redémarrez l'alimentation

=== "r1.1"

    1. Maintenez le bouton BOOT enfoncé
    2. Connectez l'USB, l'appareil sera reconnu comme DFU
    3. Chargez le firmware à l'aide de STM32CubeProgrammer :

        3.1.1 Spécifiez le chemin du fichier `.bin` du firmware (par exemple : `Firmware/iHeater_v1.2.3.bin`)

        3.1.2 Définissez l'adresse de téléchargement : `0x08000000`

        3.1.3 Cliquez sur « Start Programming »

        ou

        3.2 Chargez le firmware via la ligne de commande :
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. Redémarrez l'alimentation

---

## Mode d'emploi
=== "r1"

    - **Appui court** sur le bouton MODE - changement de mode (0-7)
    - **Maintien prolongé (2+ secondes)** - réinitialisation du mode à 0 (OFF)
    - **LED** affichent le mode actuel selon le tableau ci-dessus
    - **Clignotement des LED** - le mode est actif, mais la température cible n'est pas atteinte
    - **Lumière constante** - la température cible est atteinte

=== "r1.1"

    !!! note "Différences entre les révisions"
   
        la révision r1.1 dispose d'un port de déclenchement pour l'activation et la désactivation automatiques du chauffage de la chambre. Lors de la connexion d'une thermistance au port de déclenchement, le chauffage s'activera et se désactivera en fonction des températures spécifiées dans config.h, par défaut 45 et 80 pour l'activation et la désactivation du chauffage.

    - **Appui court** sur le bouton MODE - changement de mode (0-7)
    - **Maintien prolongé (2+ secondes)** - réinitialisation du mode à 0 (OFF)
    - **LED** affichent le mode actuel selon le tableau ci-dessus
    - **Clignotement des LED** - le mode est actif, mais la température cible n'est pas atteinte
    - **Lumière constante** - la température cible est atteinte

---

## Que faire en cas d'erreur

L'appareil redémarre automatiquement et affiche le code d'erreur à l'aide des LED. Le ventilateur sera activé.


| Code  | Description                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | L'air n'a pas atteint la cible à temps        |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | Le radiateur ne répond pas                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | Arrêt manuel (non utilisé)                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | Thermistance de l'air déconnectée                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | Thermistance du radiateur déconnectée             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | Surchauffe du radiateur                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | Surchauffe de l'air                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | Connexion perdue avec iHeater-link (build pulse v0.5.0 ; n'est plus enregistré depuis v0.5.1) | 0.25 Hz | 0.25 Hz | 0.25 Hz |
| 0xFF  | Erreur inconnue                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### Réinitialisation d'erreur

Maintenez le bouton MODE lors du démarrage jusqu'à ce que les LED changent d'indication. L'erreur sera effacée de la flash et l'appareil continuera à fonctionner normalement.

## Réglage fin

Au cas où il y aurait confirmation que les lectures de thermistance diffèrent de la température réelle, vous pouvez utiliser la fonction de réglage fin. Pour cela, vous devez mesurer la résistance des résistances correspondantes et les écrire à la place des valeurs par défaut.


```C++ title="Résistance réelle des résistances de tirage vers le haut"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

Ce logiciel est distribué sous licence GNU GPL version 3 ou ultérieure. L'usage commercial est autorisé. Les détails se trouvent dans le fichier [LICENSE](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/blob/main/LICENSE).
