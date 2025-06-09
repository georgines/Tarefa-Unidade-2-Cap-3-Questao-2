#include "auxiliar.h"

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

void inicializar_joystick()
{
    adc_init();
    adc_gpio_init(27); // X -> ADC1
    adc_gpio_init(26); // Y -> ADC0
}

int ler_posicao_joystick_x()
{
    adc_select_input(1); // ADC1 -> GPIO27
    return adc_read();
}

int ler_posicao_joystick_y()
{
    adc_select_input(0); // ADC0 -> GPIO26
    return adc_read();
}

std::string obter_direcao_joystick()
{
    int x = ler_posicao_joystick_x();
    int y = ler_posicao_joystick_y();

    const int centro = 2048;
    const int margem = 600;

    bool esquerda = x < centro - margem;
    bool direita = x > centro + margem;
    bool cima = y > centro + margem;
    bool baixo = y < centro - margem;

    if (cima && esquerda) return "Noroeste";
    if (cima && direita) return "Nordeste";
    if (baixo && esquerda) return "Sudoeste";
    if (baixo && direita) return "Sudeste";
    if (cima) return "Norte";
    if (baixo) return "Sul";
    if (esquerda) return "Oeste";
    if (direita) return "Leste";
    return "";
}

