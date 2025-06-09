#pragma once

#include "sistema.h"
#include "string"

// leds
std::string alternar_led(uint gpio, bool &estado_led);

std::string alternar_led_azul();

std::string alternar_led_verde();

std::string alternar_led_vermelho();

void inicializar_led(uint8_t pino_led);

void inicializar_leds();

//joystick
void inicializar_joystick();

int ler_posicao_joystick_x();

int ler_posicao_joystick_y();

std::string obter_direcao_joystick();