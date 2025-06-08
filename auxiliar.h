#pragma once

#include "sistema.h"
#include "string"

// botoes
void monitorar_botoes();

// leds
std::string alternar_led(uint gpio, bool &estado_led);

std::string alternar_led_azul();

std::string alternar_led_verde();

std::string alternar_led_vermelho();

void inicializar_led(uint8_t pino_led);

void inicializar_leds();

// sensores
float obter_temperatura_interna();

void inicializar_sensor_temperatura();