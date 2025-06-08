#pragma once

#include <stdio.h>
#include <string.h>
#include <functional>   
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/platform/sections.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "Acionador.h"

//botoes
#define PIN_BOTAO_A 5
#define PIN_BOTAO_B 6

//leds
#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

//wifi
#define WIFI_SSID "GERALDO_CASA"
#define WIFI_PASS "joanadarcnovo"

//webserver
#define PORTA_HTTP 80
#define TAMANHO_HTML 2048

//sensor de temperatura
#define ADC_INPUT_TEMPERATURA 4
#define ADC_FATOR_CONVERSAO (3.3f / (1 << 12))
#define TEMPERATURA_BASE 27.0f
#define TEMPERATURA_OFFSET 0.706f
#define TEMPERATURA_ESCALA 0.001721f

//status dos botões
static bool botao_a_acionado = false;
static bool botao_b_acionado = false;

//pagina html index
static const char body_fmt[] __in_flash("body") =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "  <meta charset=\"utf-8\">\n"
    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "  <title>Controle de LEDs</title>\n"
    "  <style>…seu CSS…</style>\n"
    "</head>\n"
    "<body>\n"
    "  <h1>Controle de LEDs</h1>\n"
    "  <form action=\"./blue_on\"><button>Ligar Azul</button></form>\n"
    "  <form action=\"./blue_off\"><button>Desligar Azul</button></form>\n"
    "  <form action=\"./green_on\"><button>Ligar Verde</button></form>\n"
    "  <form action=\"./green_off\"><button>Desligar Verde</button></form>\n"
    "  <form action=\"./red_on\"><button>Ligar Vermelho</button></form>\n"
    "  <form action=\"./red_off\"><button>Desligar Vermelho</button></form>\n"
    "  <p class=\"temperatura\">Temperatura: %.2f &deg;C</p>\n"
    "</body>\n"
    "</html>\n";