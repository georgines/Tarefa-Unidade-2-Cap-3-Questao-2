#pragma once

#include "sistema.h"

//botoes
void monitorar_botoes();

//leds
void ligarLED(uint8_t pino_led);

void desligarLED(uint8_t pino_led);

void inicializar_led(uint8_t pino_led);

void inicializar_leds();

//sensores
float obter_temperatura_interna();

void inicializar_sensor_temperatura();