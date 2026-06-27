# Firmware Autônomo iHeater

iHeater é uma solução compacta e acessível para impressoras 3D sem câmara térmica ativa ou com capacidades limitadas de conexão à placa-mãe. É especialmente útil para modelos com placas proprietárias, onde não há conectores livres para ventilador, aquecedor ou termistor. iHeater resolve essa tarefa de forma simples e universal.

Pode funcionar de forma independente com seu próprio firmware como dispositivo separado ou sob controle do Klipper com conexão à impressora via USB

![iHeater](../img/iHeater_200.png)

---

## Sobre o firmware

**iHeater** é um firmware para um dispositivo autônomo de controle de temperatura da câmara de uma impressora 3D.

---

## Finalidade

O dispositivo fornece aquecimento e mantém uma temperatura estável dentro da câmara da impressora 3D, o que é crítico para imprimir ABS, ASA e outros plásticos termossensíveis com alta retração e baixa adesão entre camadas.

---

## Como configurar o firmware

### Arquivo \`config.h\`

Este arquivo define os parâmetros de operação do dispositivo:
O arquivo está localizado na pasta Core/Inc/config.h

| Parâmetro                                    | Finalidade                                                                                          |
| ------------------------------------------- | --------------------------------------------------------------------------------------------------- |
| \`SELECTED_THERMISTOR_TYPE\`                  | Seleção do tipo de termistor (0-5, veja abaixo)                                                               |
| \`MODE_TEMP_0\`-\`MODE_TEMP_7\`                             | Temperaturas para cada modo. É possível definir qualquer valor, incluindo valores fracionários, por exemplo \`MODE_TEMP_4 67.5\` |
| \`MAX_TEMP\`, \`MIN_TEMP\`                      | Intervalo de temperatura permitido do aquecedor                                                          |
| \`MAX_AIR_TEMP\`, \`MIN_AIR_TEMP\`              | Intervalo de temperatura do ar                                                                         |
| \`HEATER_MIN_PWM\`                            | PWM mínimo no qual o sistema iniciará a verificação de resposta do aquecedor                             |
| \`HEATER_RESPONSE_TIMEOUT_MS\`                | Tempo de espera para aumento de temperatura do aquecedor (padrão 5 seg)                                   |
| \`AIR_TARGET_TIMEOUT_MS\`                     | Tempo máximo para atingir a temperatura-alvo do ar (padrão 10 min)                     |
| \`TIMER_CLOCK\`, \`PWM_FREQUENCY\`, \`PWM_STEPS\` | Configuração de frequência e qualidade do PWM                                                                    |

### Termistores

Selecionados em \`SELECTED_THERMISTOR_TYPE\`. Suportados:

| ID | Tipo                          | Comentário                           |
| -- | ---------------------------- | ------------------------------------- |
| 0  | ATC Semitec 104GT-2          | Padrão da E3D                       |
| 1  | ATC Semitec 104NT-4-R025H42G | Análogo ao 104GT                          |
| 2  | EPCOS 100K B57560G104F       | Popular                             |
| 3  | Generic 3950                 | **Padrão**, amplamente utilizado |
| 4  | SliceEngineering 450         | Suporte a altas temperaturas          |
| 5  | TDK NTCG104LH104JT1          | Médico, preciso                   |

### Modos de temperatura e LEDs

O modo é definido pelo valor de temperatura e exibido em forma binária com três LEDs:

Quando o modo é ativado, os LEDs piscam; quando estabilizado - acendem continuamente

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

Este é um resumo expandido com mais detalhes. Consulte a documentação completa em russopara configuração detalhada de todos os parâmetros.
