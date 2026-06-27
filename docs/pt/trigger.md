# Disparador — inicialização automática do iHeater por temperatura da mesa

## O que é e para quê

Disparador — função de ligação e desligamento automático do aquecedor iHeater no início e fim da impressão. iHeater monitora a temperatura da mesa da impressora 3D através de um termistor adicional (TH2) e:

- **Liga** o secador quando a mesa começa a aquecer (início da impressão)
- **Desliga** o secador quando a mesa esfria (impressão concluída)

Você não precisa ligar e desligar manualmente o iHeater — tudo acontece automaticamente.

!!! warning "Requisitos"
    Função de disparo disponível apenas na placa **iHeater rev 1.1**, que possui entrada para terceiro termistor (TH2).

---

## Como conectar

Instale um termistor (NTC 100K) na mesa da impressora 3D e conecte-o à entrada **TH2** na placa iHeater.

O termistor deve ser fixado em uma parte metálica ou de alumínio da mesa que conduza bem o calor — o mais perto possível da zona de aquecimento. Não é possível fixar diretamente no elemento aquecedor, mas quanto melhor o contato térmico com a superfície aquecida, mais preciso será o funcionamento do disparador. As leituras do termistor serão menores que a temperatura real do aquecedor — isso é normal, os limites são calibrados para o local específico de instalação.

---

## Como funciona

\`\`\`
Temperatura da mesa (°C)
    ^
 90 |         ┌────────────────┐  Impressão em andamento
 85 |        /                  \
 80 |-------/----DESLIGAR iHeater\-------- Limite DESLIGAR (80°C)
 75 |------/----------------------\------- Limite LIGAR (75°C)
 50 |     /    iHeater funcionando \
 25 |____/                          \____
    └─────────────────────────────────────> Tempo
         Aquecimento  Impressão   Esfriamento
\`\`\`

**Cenário de funcionamento (limite LIGAR = 75°C, limite DESLIGAR = 80°C):**

1. A impressora começa a impressão, a mesa aquece
2. Temperatura da mesa atinge **75°C** (limite LIGAR) → iHeater **liga** e começa a aquecer a câmara
3. Temperatura da mesa continua a subir e ultrapassa **80°C** (limite DESLIGAR) → iHeater registra **atingimento do objetivo**
4. Impressão em andamento, mesa quente — iHeater mantém a temperatura na câmara
5. Impressão concluída, mesa começa a esfriar
6. Temperatura da mesa cai para **80°C** (limite DESLIGAR) → iHeater **desliga**
7. Mesa esfria para temperatura ambiente — sistema pronto para novo ciclo

!!! info "Por que o limite DESLIGAR não precisa ser maior que o limite LIGAR"
    O sistema funciona em **duas fases**, e é possível desligar o iHeater apenas **após atingimento do objetivo** (passo 3):

    **Fase 1** — aguardando ligação: iHeater desligado, sistema aguarda temperatura suba até limite LIGAR. Limite DESLIGAR nesta fase é **ignorado**.

    **Fase 2** — após atingimento do objetivo: temperatura passou pelo limite DESLIGAR para cima, objetivo registrado. Agora, quando temperatura **cair** abaixo do limite DESLIGAR — iHeater desligará.

    Como as fases funcionam sequencialmente e não se sobrepõem, limite DESLIGAR pode ser tanto acima quanto abaixo do limite LIGAR:

    - **Limite DESLIGAR = 80°C, limite LIGAR = 75°C** — mesa deve aquecer até 80°C para objetivo ser registrado, desligamento ao esfriar até 80°C
    - **Limite DESLIGAR = 70°C, limite LIGAR = 75°C** — objetivo registrado imediatamente (mesa já acima de 70°C no momento da ligação), desligamento ao esfriar até 70°C

!!! info "Por que iHeater não ligará novamente ao esfriar"
    Quando a mesa esfria e passa pelo limite LIGAR (por exemplo, 75°C) — iHeater **não ligará novamente**. O sistema está protegido por uma flag de atingimento do objetivo:

    1. iHeater desligou ao cair até limite DESLIGAR (80°C) — mas a flag de atingimento do objetivo **permanece ativa**
    2. Mesa continua esfriando e passa por 75°C (limite LIGAR) — ligação **bloqueada**, pois a flag ainda está ativa
    3. Temperatura cai **abaixo** do limite LIGAR (abaixo de 75°C) — apenas então a flag é **resetada**, e o sistema fica pronto para novo ciclo

    Assim, para nova ligação a mesa deve **esfriar completamente** abaixo do limite LIGAR, então aquecer novamente até ele — ou seja, começar nova impressão.

!!! danger "Limite DESLIGAR não deve estar na zona de aquecimento do iHeater"
    Após fim da impressão a mesa da impressora desliga e começa a esfriar. Mas iHeater neste momento ainda está funcionando e irradia calor. Se termistor TH2 está posicionado de forma que iHeater o aquece com seu calor — temperatura no termistor pode não cair abaixo do limite DESLIGAR, e iHeater **nunca desligará**.

    **Certifique-se** que limite DESLIGAR está configurado **abaixo** da temperatura que termistor TH2 mostra quando mesa da impressora desligada, mas iHeater funcionando. Em outras palavras — limite DESLIGAR deve ser menor que a temperatura até qual iHeater consegue aquecer a zona de instalação do termistor.

---

## Configurações padrão

| Parâmetro | Valor | Descrição |
|-----------|-------|-----------|
| Limite LIGAR | **75°C** | Temperatura da mesa na qual iHeater liga |
| Limite DESLIGAR | **80°C** | Temperatura da mesa na qual iHeater desliga |
| Modo de aquecimento | **MODE 2** (60°C) | Modo em que iHeater funciona ao disparador ativar |

Ao disparador ativar, iHeater liga no **modo 2** — temperatura alvo do ar 60°C. Indicação: **LED2** aceso.

---

## Calibração dos limites do disparador

Se os valores padrão (75°C / 80°C) não funcionam para sua impressora, você pode ajustar os limites para sua temperatura de trabalho de mesa.

### Quando calibração é necessária

- Deseja que iHeater liga antes ou depois
- Termistor localizado longe do aquecedor de mesa e mostra temperatura baixa

### Instruções passo a passo

!!! note "Importante"
    Durante calibração aquecedor iHeater está **desligado** — calibração é segura.

#### Passo 1: Entre em modo de calibração

- Certifique-se que iHeater está em **modo de espera** (MODE 0 — todos LED desligados)
- **Mantenha botão pressionado por 2+ segundos** → entra em calibração
- **Indicação**: LED1 pisca rapidamente (~10 Hz) — está em modo de configuração de limite LIGAR

#### Passo 2: Configure limite de ligação (LIGAR)

- Aqueça mesa da impressora até temperatura na qual quer que iHeater **liga**
- Aguarde estabilização de temperatura
- **Mantenha botão pressionado por 2+ segundos** → valor salvo
- **Indicação**: LED1 + LED2 piscam rapidamente — está em modo de configuração de limite DESLIGAR

#### Passo 3: Configure limite de desligamento (DESLIGAR)

- Aqueça mesa até temperatura um pouco **acima** do limite LIGAR (recomenda-se +5°C)
- **Mantenha botão pressionado por 2+ segundos** → valor salvo na memória flash
- **Indicação de sucesso**: todos 3 LED piscam 3 vezes
- iHeater retornará automaticamente ao modo de espera (MODE 0)

#### Cancelamento de calibração

Em **qualquer passo** — **pressione botão brevemente** → calibração cancelada, valores anteriores restaurados.

### Exemplo de calibração (mesa 100°C para ABS)

| Passo | Ação | Temperatura da mesa |
|-------|------|-------------------|
| 1 | Pressão longa em MODE 0 | — |
| 2 | Aqueça mesa, pressão longa | **75°C** (limite LIGAR) |
| 3 | Aqueça mesa mais, pressão longa | **80°C** (limite DESLIGAR) |

---

## Indicação LED

| Estado | LED1 | LED2 | LED3 | Descrição |
|--------|------|------|------|-----------|
| Espera (MODE 0) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater desligado, disparador aguardando |
| Disparador ativado (aquecimento) | ![off](../img/ball_gifs/black_ball.gif) | ![blink](../img/ball_gifs/blinking_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater aquecendo até temperatura alvo |
| Disparador ativado (pronto) | ![off](../img/ball_gifs/black_ball.gif) | ![on](../img/ball_gifs/red_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | iHeater atingiu temperatura alvo (60°C) |
| Calibração passo 1 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | ![off](../img/ball_gifs/black_ball.gif) | Configuração de limite LIGAR |
| Calibração passo 2 | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![off](../img/ball_gifs/black_ball.gif) | Configuração de limite DESLIGAR |
| Calibração concluída | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | ![blink](../img/ball_gifs/blinking_ball_10hz.gif) | 3 vezes — sucesso |

---

## Diagrama de estados

\`\`\`
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  ┌───────────┐     Temperatura da mesa ≥ limite LIGAR            │
│  │  Espera   │ ─────────────────────────────────┐               │
│  │  (MODE 0) │                                  │               │
│  │  LED: todos│  ◄─────────┐                     ▼               │
│  │  desligados│            │          ┌──────────────────┐       │
│  └───────────┘            │          │  iHeater funciona│       │
│        ▲                  │          │  (TRIGGER MODE)  │       │
│        │                  │          │  LED2 aceso      │       │
│  Mesa esfriou             │          └──────────────────┘       │
│  abaixo limite LIGAR      │                     │               │
│  (pronto para novo        │    Temperatura da   │               │
│   ciclo)                  │    mesa ≤ limite    │               │
│                           │    DESLIGAR         │               │
│                           │                     ▼               │
│                  ┌────────┴─────────────────────────┐           │
│                  │  iHeater desliga → MODE 0        │           │
│                  └──────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────────┘
\`\`\`

---

## Perguntas frequentes

**Disparador não ativa mesmo com mesa quente**

- Verifique conexão de termistor TH2
- Certifique-se que temperatura da mesa realmente atinge limite LIGAR
- Calibre limites mais baixos se imprime com temperatura de mesa baixa

**iHeater liga e desliga imediatamente**

- Limite LIGAR e limite DESLIGAR muito próximos. Calibre com diferença mínima de 5°C

**Quero mudar modo de aquecimento ao disparador ativar**

- Na versão atual o modo disparador é fixado — **MODE 2** (60°C). Pode ser mudado apenas recompilando firmware (parâmetro `TRIGGER_MODE` em `config.h`)
