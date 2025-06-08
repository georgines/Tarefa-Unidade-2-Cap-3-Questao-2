#include "auxiliarLeds.h"

void ligar_led_azul() { gpio_put(LED_AZUL_PIN, 1); }

void desligar_led_azul() { gpio_put(LED_AZUL_PIN, 0); }

void ligar_led_verde() { gpio_put(LED_VERDE_PIN, 1); }

void desligar_led_verde() { gpio_put(LED_VERDE_PIN, 0); }

void ligar_led_vermelho() { gpio_put(LED_VERMELHO_PIN, 1); }

void desligar_led_vermelho() { gpio_put(LED_VERMELHO_PIN, 0); }

void inicializar_led(uint8_t pino_led)
{
    gpio_init(pino_led);
    gpio_set_dir(pino_led, GPIO_OUT);
    gpio_put(pino_led, false);
}

void inicializar_leds()
{
    inicializar_led(LED_AZUL_PIN);
    inicializar_led(LED_VERDE_PIN);
    inicializar_led(LED_VERMELHO_PIN);
}
