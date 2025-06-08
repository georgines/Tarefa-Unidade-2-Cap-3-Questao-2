#include "auxiliarLeds.h"

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
