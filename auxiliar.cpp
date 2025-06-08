#include "auxiliar.h"

Acionador BotaoA(PIN_BOTAO_A);
Acionador BotaoB(PIN_BOTAO_B);

bool botao_a_estado = false;
bool botao_b_estado = false;

void monitorar_botoes()
{
    BotaoA.estaPressionadoAgora() ? botao_a_estado = true : botao_a_estado = false;
    BotaoB.estaPressionadoAgora() ? botao_b_estado = true : botao_b_estado = false;
}

static bool estado_led_azul = false;
static bool estado_led_verde = false;
static bool estado_led_vermelho = false;

std::string alternar_led(uint gpio, bool &estado_led)
{
    estado_led = !estado_led;
    gpio_put(gpio, estado_led);
    char buf[32];
    std::snprintf(buf, sizeof(buf), R"({"ligado":%d})", estado_led);
    return std::string(buf);
}

std::string alternar_led_azul()
{
    return alternar_led(LED_AZUL, estado_led_azul);
}

std::string alternar_led_verde()
{
    return alternar_led(LED_VERDE, estado_led_verde);
}

std::string alternar_led_vermelho()
{
    return alternar_led(LED_VERMELHO, estado_led_vermelho);
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
