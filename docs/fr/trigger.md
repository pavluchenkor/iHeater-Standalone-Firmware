# Déclencheur — démarrage automatique d'iHeater selon la température du plateau

## Qu'est-ce que c'est et à quoi ça sert

Le déclencheur — fonction de mise sous tension et hors tension automatique du radiateur iHeater au début et à la fin de l'impression. iHeater surveille la température du plateau de l'imprimante 3D via un thermistor supplémentaire (TH2) et :

- **Allume** le sécheur lorsque le plateau commence à se réchauffer (début de l'impression)
- **Éteint** le sécheur lorsque le plateau refroidit (impression terminée)

Vous n'avez pas besoin d'allumer et d'éteindre manuellement iHeater — tout se fait automatiquement.

!!! warning "Conditions requises"
    La fonction déclencheur n'est disponible que sur la carte **iHeater rév. 1.1**, qui dispose d'une entrée pour un troisième thermistor (TH2).

---

## Comment brancher

Installez un thermistor (NTC 100K) sur le plateau de l'imprimante 3D et connectez-le à l'entrée **TH2** de la carte iHeater.

Le thermistor doit être fixé sur une partie métallique ou en aluminium du plateau qui conduit bien la chaleur — aussi près que possible de la zone de chauffage. Il n'est pas possible de le fixer directement sur l'élément chauffant, mais plus le contact thermique avec la surface chauffée est bon, plus précis sera le fonctionnement du déclencheur. Les lectures du thermistor seront inférieures à la température réelle du radiateur — c'est normal, les seuils sont étalonnés pour l'emplacement d'installation spécifique.

---

## Comment ça marche

```
Température du plateau (°C)
    ^
 90 |         ┌────────────────┐  Impression en cours
 85 |        /                  \
 80 |-------/----ARRÊT iHeater----\-------- Seuil d'arrêt (80°C)
 75 |------/----------------------\------- Seuil d'activation (75°C)
 50 |     /    iHeater fonctionne   \
 25 |____/                          \____
    └─────────────────────────────────────> Temps
         Chauffage    Impression   Refroidissement
```

**Scénario de fonctionnement (seuil d'activation = 75°C, seuil d'arrêt = 80°C) :**

1. L'imprimante commence à imprimer, le plateau se réchauffe
2. La température du plateau atteint **75°C** (seuil d'activation) → iHeater **s'allume** et commence à chauffer la chambre
3. La température du plateau continue d'augmenter et dépasse **80°C** (seuil d'arrêt) → iHeater enregistre **atteinte de l'objectif**
4. L'impression se poursuit, le plateau est chaud — iHeater maintient la température dans la chambre
5. L'impression est terminée, le plateau commence à refroidir
6. La température du plateau chute à **80°C** (seuil d'arrêt) → iHeater **s'éteint**
7. Le plateau refroidit jusqu'à la température ambiante — le système est prêt pour un nouveau cycle

!!! info "Pourquoi le seuil d'arrêt n'a pas besoin d'être plus élevé que le seuil d'activation"
    Le système fonctionne en **deux phases**, et iHeater ne peut être éteint qu'**après atteinte de l'objectif** (étape 3) :

    **Phase 1** — en attente d'activation : iHeater est éteint, le système attend que la température monte jusqu'au seuil d'activation. Le seuil d'arrêt à ce stade **est ignoré**.

    **Phase 2** — après atteinte de l'objectif : la température a dépassé le seuil d'arrêt vers le haut, l'objectif est enregistré. Maintenant, lorsque la température **baisse** en dessous du seuil d'arrêt — iHeater s'éteint.

    Puisque les phases fonctionnent séquentiellement et ne se chevauchent pas, le seuil d'arrêt peut être à la fois plus élevé et plus bas que le seuil d'activation :

    - **Seuil d'arrêt = 80°C, seuil d'activation = 75°C** — le plateau doit se réchauffer à 80°C pour que l'objectif soit enregistré, arrêt lors du refroidissement à 80°C
    - **Seuil d'arrêt = 70°C, seuil d'activation = 75°C** — l'objectif est enregistré immédiatement (le plateau est déjà au-dessus de 70°C au moment de l'activation), arrêt lors du refroidissement à 70°C

!!! info "Pourquoi iHeater ne se réallumera pas lors du refroidissement"
    Lorsque le plateau refroidit et passe par le seuil d'activation (par exemple, 75°C) — iHeater **ne se rallumera pas**. Le système est protégé par un drapeau d'atteinte de l'objectif :

    1. iHeater s'est éteint lors de la chute jusqu'au seuil d'arrêt (80°C) — mais le drapeau d'atteinte de l'objectif **reste actif**
    2. Le plateau continue de refroidir et passe 75°C (seuil d'activation) — l'activation **est bloquée**, car le drapeau est encore actif
    3. La température chute **en dessous** du seuil d'activation (en dessous de 75°C) — seulement alors le drapeau **est réinitialisé**, et le système est prêt pour un nouveau cycle

    Ainsi, pour une réactivation, le plateau doit **refroidir complètement** en dessous du seuil d'activation, puis se réchauffer à nouveau — c'est-à-dire qu'une nouvelle impression doit commencer.

!!! danger "Le seuil d'arrêt ne doit pas se trouver dans la zone de chauffage d'iHeater"
    Après la fin de l'impression, le plateau de l'imprimante s'éteint et commence à refroidir. Mais iHeater continue de fonctionner à ce moment et émet lui-même de la chaleur. Si le thermistor TH2 est positionné de sorte qu'iHeater le réchauffe par son rayonnement — la température du thermistor peut ne pas tomber en dessous du seuil d'arrêt, et iHeater **ne s'éteindra jamais**.

    **Assurez-vous** que le seuil d'arrêt est défini **en dessous** de la température que le thermistor TH2 affiche lorsque le plateau de l'imprimante est éteint et qu'iHeater fonctionne. En d'autres termes — le seuil d'arrêt doit être inférieur à la température à laquelle iHeater lui-même peut réchauffer la zone d'installation du thermistor.

---

## Paramètres par défaut

| Paramètre | Valeur | Description |
|-----------|--------|----------|
| Seuil d'activation | **75°C** | Température du plateau à laquelle iHeater s'allume |
| Seuil d'arrêt | **80°C** | Température du plateau à laquelle iHeater s'éteint |
| Mode de chauffage | **MODE 2** (60°C) | Mode dans lequel fonctionne iHeater lors du déclenchement |

Lors du déclenchement du déclencheur, iHeater s'allume en **mode 2** — température cible de l'air 60°C. Indication : **LED2** s'allume.

---

## Étalonnage des seuils du déclencheur

Si les valeurs par défaut (75°C / 80°C) ne conviennent pas à votre imprimante, vous pouvez adapter les seuils à votre température de plateau de travail.

### Quand l'étalonnage est nécessaire

- Vous voulez qu'iHeater s'allume plus tôt ou plus tard
- Le thermistor est situé loin du radiateur du plateau et affiche une température abaissée

### Instructions étape par étape

!!! note "Important"
    Pendant l'étalonnage, le radiateur iHeater est **éteint** — l'étalonnage est sûr.

#### Étape 1 : Entrez en mode d'étalonnage

- Assurez-vous qu'iHeater est en **mode d'attente** (MODE 0 — tous les LED éteints)
- **Appuyez longtemps sur le bouton pendant 2+ secondes** → entrée en étalonnage
- **Indication** : LED1 clignote rapidement (~10 Hz) — vous êtes en mode de réglage du seuil d'activation

#### Étape 2 : Définissez le seuil d'activation

- Réchauffez le plateau de l'imprimante à la température à laquelle vous souhaitez qu'iHeater **s'allume**
- Attendez la stabilisation de la température
- **Appuyez longtemps sur le bouton pendant 2+ secondes** → valeur enregistrée
- **Indication** : LED1 + LED2 clignotent rapidement — vous êtes en mode de réglage du seuil d'arrêt

#### Étape 3 : Définissez le seuil d'arrêt

- Réchauffez le plateau à une température légèrement **plus élevée** que le seuil d'activation (recommandé +5°C)
- **Appuyez longtemps sur le bouton pendant 2+ secondes** → valeur enregistrée en mémoire flash
- **Indication de succès** : les 3 LED clignotent 3 fois
- iHeater revient automatiquement en mode d'attente (MODE 0)

#### Annulation de l'étalonnage

À **n'importe quel stade** — **appuyez brièvement sur le bouton** → étalonnage annulé, valeurs précédentes restaurées.

### Exemple d'étalonnage (plateau 100°C pour ABS)

| Étape | Action | Température du plateau |
|-------|--------|----------------------|
| 1 | Appui long en MODE 0 | — |
| 2 | Réchauffer le plateau, appui long | **75°C** (seuil d'activation) |
| 3 | Réchauffer le plateau davantage, appui long | **80°C** (seuil d'arrêt) |

---

## Indication LED

| État | LED1 | LED2 | LED3 | Description |
|------|------|------|------|----------|
| Attente (MODE 0) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater éteint, déclencheur en attente |
| Déclencheur activé (chauffage) | ![off](../img/ball_gifs/black_ball.gif) | ![blink](../img/ball_gifs/blinking_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater chauffe jusqu'à la température cible |
| Déclencheur activé (prêt) | ![off](../img/ball_gifs/black_ball.gif) | ![on](../img/ball_gifs/red_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater a atteint la température cible (60°C) |
| Étalonnage étape 1 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | Réglage du seuil d'activation |
| Étalonnage étape 2 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | Réglage du seuil d'arrêt |
| Étalonnage terminé | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | 3 fois — succès |

---

## Diagramme d'état

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  ┌───────────┐     Température du plateau ≥ seuil d'activation  │
│  │  Attente  │ ─────────────────────────────────┐               │
│  │ (MODE 0)  │                                  │               │
│  │  LED : tous│  ◄─────────┐                     ▼               │
│  │  éteints  │            │          ┌──────────────────┐       │
│  └───────────┘            │          │  iHeater fonctionne│      │
│        ▲                  │          │  (TRIGGER MODE)  │       │
│        │                  │          │  LED2 s'allume   │       │
│  Plateau refroidi          │          └──────────────────┘       │
│  en dessous du seuil      │                     │               │
│  d'activation             │    Température du plateau│            │
│  (prêt pour un nouveau    │    ≤ seuil d'arrêt  │               │
│   cycle)                  │                     ▼               │
│                           │     ┌────────────────────────────┐  │
│                  ┌────────┴─────┤  iHeater s'éteint → MODE 0  │  │
│                  │              └────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

---

## Questions fréquemment posées

**Le déclencheur ne s'active pas bien que le plateau soit chaud**

- Vérifiez la connexion du thermistor TH2
- Assurez-vous que la température du plateau atteint réellement le seuil d'activation
- Étalonnez les seuils plus bas si vous imprimez à basse température du plateau

**iHeater s'allume et s'éteint immédiatement**

- Les seuils d'activation et d'arrêt sont trop proches. Étalonnez avec une différence d'au moins 5°C

**Je veux changer le mode de chauffage lors du déclenchement**

- Dans le micrologiciel actuel, le mode déclencheur est fixe — **MODE 2** (60°C). Il ne peut être modifié que par recompilation du micrologiciel (paramètre `TRIGGER_MODE` dans `config.h`)
