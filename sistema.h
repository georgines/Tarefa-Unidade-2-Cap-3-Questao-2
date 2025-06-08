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

// botoes
#define PIN_BOTAO_A 5
#define PIN_BOTAO_B 6

// leds
#define LED_AZUL 12
#define LED_VERDE 11
#define LED_VERMELHO 13

// wifi
#define WIFI_SSID "RECEPCAO"
#define WIFI_PASS "naotedouasenha1234"

// webserver
#define PORTA_HTTP 80
#define TAMANHO_HTML 2048

// sensor de temperatura
#define ADC_INPUT_TEMPERATURA 4
#define ADC_FATOR_CONVERSAO (3.3f / (1 << 12))
#define TEMPERATURA_BASE 27.0f
#define TEMPERATURA_OFFSET 0.706f
#define TEMPERATURA_ESCALA 0.001721f

// status dos botões
extern bool botao_a_estado;
extern bool botao_b_estado;

// pagina html index
static const char body_fmt[] __in_flash("body") =
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head>\n"
    "  <meta charset=\"utf-8\">\n"
    "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "  <title>Controle de LEDs</title>\n"
    "  <style></style>\n"
    "</head>\n"
    "<body>\n"
    "  <h1>Controle o LED e veja o status da temperatura e dos botões sem recarregar a página</h1>\n"
    "  <button onclick=\"sendCommand('az/ligado')\">Ligar Azul</button>\n"
    "  <button onclick=\"sendCommand('az/desligado')\">Desligar Azul</button>\n"
    "  <button onclick=\"sendCommand('vr/ligado')\">Ligar Verde</button>\n"
    "  <button onclick=\"sendCommand('vr/desligado')\">Desligar Verde</button>\n"
    "  <button onclick=\"sendCommand('vm/ligado')\">Ligar Vermelho</button>\n"
    "  <button onclick=\"sendCommand('vm/desligado')\">Desligar Vermelho</button>\n"
    "  <p class=\"temperatura\">Temperatura: <span id=\"temperatura\">--</span> &deg;C</p>\n"
    "  <p>Status Botão A: <span id=\"botao_a\">--</span></p>\n"
    "  <p>Status Botão B: <span id=\"botao_b\">--</span></p>\n"
    "  <script>\n"
    "    function sendCommand(route) {\n"
    "      fetch(`/${route}`);\n"
    "    }\n"
    "    function updateStatus() {\n"
    "      fetch('/status').then(response => response.json()).then(data => {\n"
    "        document.getElementById('temperatura').textContent = data.temperatura.toFixed(2);\n"
    "        document.getElementById('botao_a').textContent = data.botao_a ? 'Pressionado' : 'Solto';\n"
    "        document.getElementById('botao_b').textContent = data.botao_b ? 'Pressionado' : 'Solto';\n"
    "      });\n"
    "    }\n"
    "    setInterval(updateStatus, 500);\n"
    "  </script>\n"
    "</body>\n"
    "</html>\n";