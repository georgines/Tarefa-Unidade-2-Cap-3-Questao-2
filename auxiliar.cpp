#include "auxiliar.h"

Acionador BotaoA(PIN_BOTAO_A);
Acionador BotaoB(PIN_BOTAO_B);

void monitorar_botoes() {
    botao_a_acionado = BotaoA.verificarAcionamento();
    botao_b_acionado = BotaoB.verificarAcionamento();

    if (botao_a_acionado) {
        printf("Botão A pressionado\n");
    }
    if (botao_b_acionado) {
        printf("Botão B pressionado\n");
    }
}

void ligarLED(uint8_t pino_led)
{
    gpio_put(pino_led, true);
}

void desligarLED(uint8_t pino_led)
{
    gpio_put(pino_led, false);
}

void inicializar_led(uint8_t pino_led)
{
    gpio_init(pino_led);
    gpio_set_dir(pino_led, GPIO_OUT);
    gpio_put(pino_led, false);
}

void inicializar_leds()
{
    inicializar_led(LED_AZUL);
    inicializar_led(LED_VERDE);
    inicializar_led(LED_VERMELHO);
}

float obter_temperatura_interna()
{
    adc_select_input(ADC_INPUT_TEMPERATURA);
    uint16_t valor_bruto = adc_read();
    return TEMPERATURA_BASE - ((valor_bruto * ADC_FATOR_CONVERSAO) - TEMPERATURA_OFFSET) / TEMPERATURA_ESCALA;
}


void inicializar_sensor_temperatura()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
}
