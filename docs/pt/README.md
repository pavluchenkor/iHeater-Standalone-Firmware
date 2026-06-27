<!-- [![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) -->
<!-- [![Docs](https://img.shields.io/badge/docs-view--online-red?logo=readthedocs)](https://docs.idryer.org/iHeater/) -->
<!-- [![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer) -->
<!-- [![Discord](https://img.shields.io/badge/discord-Join%20Chat-5865F2?logo=discord)](https://discord.gg/jGce5eeHHz) -->


# Firmware Autónomo iHeater

iHeater é uma solução compacta e acessível para impressoras 3D sem câmara térmica ativa ou com capacidade limitada de conexão na placa-mãe. É particularmente útil para modelos com placas proprietárias, onde não há conectores livres para ventoinha, aquecedor ou termistor. O iHeater resolve este problema de forma simples e universal.


Pode funcionar de forma independente com o seu próprio firmware como dispositivo separado ou sob controlo do Klipper com conexão à impressora por USB

![iHeater](../img/iHeater_200.png)

---

## Sobre o firmware

**iHeater** é um firmware para um dispositivo autónomo de controlo de temperatura da câmara de uma impressora 3D.

---

## Finalidade

O dispositivo fornece aquecimento e mantém uma temperatura estável dentro da câmara da impressora 3D, que é crítico para impressão de ABS, ASA e outros plásticos dependentes da temperatura com elevada contração e baixa adesão entre camadas.

---

## Como configurar o firmware

### Ficheiro `config.h`

Este ficheiro define os parâmetros de funcionamento do dispositivo:
O ficheiro está localizado na pasta Core/Inc/config.h

| Parâmetro                                    | Finalidade                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                  | Seleção do tipo de termistor (0-5, ver abaixo)                                                               |
| `MODE_TEMP_0`-`MODE_TEMP_7`                             | Temperaturas para cada modo. Pode definir qualquer valor, incluindo decimais, por exemplo `MODE_TEMP_4 67.5` |
| `MAX_TEMP`, `MIN_TEMP`                      | Intervalo de temperatura admissível do aquecedor                                                          |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`              | Intervalo de temperatura do ar                                                                         |
| `HEATER_MIN_PWM`                            | PWM mínimo no qual o sistema começará a verificar a resposta do aquecedor                             |
| `HEATER_RESPONSE_TIMEOUT_MS`                | Tempo de espera para o aumento de temperatura do aquecedor (predefinido: 5 seg)                                   |
| `AIR_TARGET_TIMEOUT_MS`                     | Tempo máximo para atingir a temperatura alvo do ar (predefinido: 10 min)                     |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | Configuração da frequência e qualidade do PWM                                                                    |

### Termistores

Selecionados em `SELECTED_THERMISTOR_TYPE`. Suportados:

| ID | Tipo                          | Comentário                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | Padrão da E3D                       |
| 1  | ATC Semitec 104NT-4-R025H42G | Análogo do 104GT                          |
| 2  | EPCOS 100K B57560G104F       | Popular                             |
| 3  | Generic 3950                 | **Predefinido**, amplamente utilizado |
| 4  | SliceEngineering 450         | Suporte para temperaturas elevadas          |
| 5  | TDK NTCG104LH104JT1          | Médico, preciso                   |

### Modos de temperatura e LEDs

O modo é definido pelo valor de temperatura e é apresentado em forma binária através de três LEDs:

Ao ativar o modo, os LEDs piscam; ao atingir o modo, iluminam-se continuamente

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
### Ativação e desativação automáticas r1.1

Macros
```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

Controlam a temperatura de ativação e desativação do aquecedor, bem como o modo ao ativar.

Suponha que o termistor do gatilho está localizado perto do aquecedor da mesa.
Com as configurações atuais, o aquecimento da câmara ativará quando a temperatura da mesa atingir 45 e desativará quando a temperatura da mesa cair abaixo de 80, estes parâmetros são aceitáveis para a maioria dos plásticos que requerem câmara térmica ativa e não causam conflitos lógicos durante o funcionamento.

Também pode selecionar o modo que será ativado quando o gatilho for disparado. Por exemplo, agora é selecionado TRIGGER_MODE MODE_2, o que significa que o modo MODE_TEMP_2 será ativado - temperatura na câmara 60.0°C

---
### Trabalho com iHeater-link (montagem r1.1 pulse)

A montagem `iheater_rev1_1_pulse` é projetada para funcionar em conjunto com o módulo iHeater-link no ESP. O módulo recebe comandos da impressora, portal ou aplicação móvel e transmite a definição de temperatura da câmara para o iHeater através de uma linha GPIO. O firmware iHeater não depende do lado da impressora e não conhece o transporte - recebe apenas um número: a temperatura do ar desejada na câmara em °C.

#### Conexão

1. Grave o iHeater com a montagem `iheater_rev1_1_pulse` (ver secção "Como gravar").
2. Ligue o fio de sinal do iHeater-link ao conector **TH2** da placa iHeater (pino PB1).
3. A ligação à terra GND comum entre ESP e iHeater é obrigatória - utilize um fio de terra separado.
4. Deixe o termistor do aquecedor no seu conector padrão **TH0**. O termistor do ar em **TH1**. Funcionam como em qualquer outra montagem.
5. O conector do gatilho não é utilizado nesta montagem (a sua linha é ocupada pela entrada digital).

!!! warning "Nunca ligue ou desligue o fio de sinal com alimentação fornecida."

#### Comandos recebidos

| Código de iHeater-link | Significado |
|---------------------|--------|
| `0`                 | Desativar aquecimento |
| `10`                | Desativar aquecimento (código de desativação acordado) |
| `45..90`            | Ativar aquecimento, definição de ar em °C igual ao código |
| outros valores  | Ignorados, estado anterior mantido |

O mínimo `45 °C` é definido em `config.h` pela macro `PULSE_MIN_TARGET`. O máximo é limitado por `MAX_AIR_TEMP` (predefinido: 90 °C).

#### Indicação LED

| Estado | LED1 | LED2 | LED3 |
|-----------|------|------|------|
| Aguardando primeiro pacote após inicialização (até 20 s) | LED a correr LED1 → LED2 → LED3 com passo de 1000 ms | | |
| Ligação OK, aquecimento desativado | iluminado continuamente (heartbeat "ligação ativa") | apagado | piscada curta ~80 ms em cada pacote recebido |
| Ligação OK, aquecimento ativado | LED a correr LED1 → LED2 → LED3 com passo de 500 ms | | |
| Ligação perdida (mais de 1,5 s sem pacote após primeira conexão) | Todos os três piscam sincronamente; o dispositivo reinicia com código de erro `0x08` (`ERROR_LINK_LOST`). Aquecedor desativado. | | |

Os erros normais (sobreaquecimento, termistor desligado, aquecedor não responde) funcionam da mesma forma que nas montagens analógicas - ver secção "Como lidar com erros".

#### Comportamento em caso de perda de ligação

Após ligar o iHeater e o iHeater-link, podem iniciar simultaneamente. O módulo iHeater-link precisa de tempo para inicializar o Wi-Fi e conectar-se à fonte de comandos, portanto, nos primeiros **20 segundos** após o arranque, o firmware do iHeater **não considera a ausência de pacotes como um erro** e mostra uma luz corrida lenta (passo de 1000 ms) - "aguardando primeiro pacote".

Se nenhum pacote correto chegar durante estes 20 segundos, o firmware entra em `ERROR_LINK_LOST` da mesma forma que com perda de ligação no modo normal.

Após o primeiro pacote ser recebido, a regra normal aplica-se: ausência de pacotes por mais de 1,5 segundos → erro.

Algoritmo ao disparar um erro:

1. O firmware guarda na flash o código de erro `0x08` (`ERROR_LINK_LOST`).
2. Reinicia o controlador.
3. Após reinicialização, o aquecedor está desativado; durante a nova janela de 20 segundos novamente luz corrida de espera, depois - piscada lenta de todos os LEDs.
4. Para limpar o erro, mantenha o botão MODE pressionado ao ligar, depois solte. O dispositivo voltará ao funcionamento normal.

#### Botão MODE

Na montagem pulse, o botão MODE **não muda de modo e não inicia a calibração**. A sua única função é limpar o erro guardado ao iniciar o dispositivo. Todos os comandos de aquecimento vêm apenas do iHeater-link.

---

## Como gravar

### Descarregar

[Descarregue o firmware da página de lançamentos](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### Utilizar firmware pronto

O firmware é pré-configurado com modos de temperatura de MODE_TEMP_0 a MODE_TEMP_7, conforme descrito acima, e é destinado a funcionar com um termistor do tipo Generic 3950 (tipo 3 conforme a lista em `config.h`).

### Utilizar modo DFU (USB)
=== "r1"

    1. Instale o jumper BOOT0
    2. Ligue o USB, o dispositivo será reconhecido como DFU
    3.1 Carregue o firmware usando STM32CubeProgrammer:

    3.1.1 Especifique o caminho para o ficheiro `.bin` do firmware (por exemplo: `Firmware/iHeater_v1.2.3.bin`)
    3.1.2 Defina o endereço de carga: `0x08000000`

    3.1.3 Clique em "Start Programming"

    ou

    3.2 Carregue o firmware através da linha de comandos:
        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    4. Remova o jumper e reinicie a alimentação

=== "r1.1"

    1. Mantenha o botão BOOT pressionado
    2. Ligue o USB, o dispositivo será reconhecido como DFU
    3. Carregue o firmware usando STM32CubeProgrammer:

        3.1.1 Especifique o caminho para o ficheiro `.bin` do firmware (por exemplo: `Firmware/iHeater_v1.2.3.bin`)

        3.1.2 Defina o endereço de carga: `0x08000000`

        3.1.3 Clique em "Start Programming"

        ou

        3.2 Carregue o firmware através da linha de comandos:
            ```bash
            dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
            ```
    4. Reinicie a alimentação

---

## Como usar
=== "r1"

    - **Pressão curta** do botão MODE - alterna entre modos (0-7)
    - **Pressão longa (2+ seg)** - redefine o modo para 0 (OFF)
    - **LEDs** exibem o modo atual conforme a tabela acima
    - **Piscada de LEDs** - modo ativo mas temperatura ainda não atingida
    - **Iluminação contínua** - temperatura alvo atingida

=== "r1.1"

    !!! note "Diferenças entre revisões"
   
        a revisão r1.1 tem uma porta de gatilho para ativação e desativação automática do aquecimento da câmara. Ao ligar um termistor à porta de gatilho, o aquecimento ativará e desativará de acordo com as temperaturas especificadas em config.h, por predefinição 45 e 80 para ativação e desativação do aquecimento.

    - **Pressão curta** do botão MODE - alterna entre modos (0-7)
    - **Pressão longa (2+ seg)** - redefine o modo para 0 (OFF)
    - **LEDs** exibem o modo atual conforme a tabela acima
    - **Piscada de LEDs** - modo ativo mas temperatura ainda não atingida
    - **Iluminação contínua** - temperatura alvo atingida

---

## Como lidar com erros

O dispositivo reinicia automaticamente e apresenta o código de erro através dos LEDs, com o ventilador ativado.


| Código  | Descrição                               | LED3 | LED2 | LED1  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | Ar não atingiu o alvo no tempo          |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x02  | Aquecedor não responde                     |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)  |
| 0x03  | Encerramento manual (não utilizado)                  |![LED OFF](../img/ball_gifs/black_ball.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |
| 0x04  | Termistor de ar desligado                |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x05  | Termistor do aquecedor desligado             |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x06  | Sobreaquecimento do aquecedor                     |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif) |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED OFF](../img/ball_gifs/black_ball.gif)   |
| 0x07  | Sobreaquecimento do ar                        |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)  |![LED 10Hz](../img/ball_gifs/blinking_ball_10hz.gif)   |
| 0x08  | Ligação perdida com iHeater-link (apenas montagem pulse) | 0.25 Hz | 0.25 Hz | 0.25 Hz |
| 0xFF  | Erro desconhecido                              |![LED ON](../img/ball_gifs/red_ball.gif)  | ![LED ON](../img/ball_gifs/red_ball.gif)|![LED ON](../img/ball_gifs/red_ball.gif)  |



### Limpeza de erros

Mantenha o botão MODE pressionado durante o arranque até os LEDs mudarem de indicação. O erro será apagado da flash e o dispositivo continuará funcionando normalmente.

## Ajuste fino

Caso haja confirmação de que as leituras do termistor diferem da temperatura real, pode utilizar a função de ajuste fino para isso, é necessário medir a resistência dos resistores correspondentes e escrever os seus valores em vez dos valores predefinidos


```C++ title="Resistência real dos resistores de pull-up"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```


---

Este software é licenciado sob a Licença Pública Geral GNU v3.0 apenas para uso não comercial.
