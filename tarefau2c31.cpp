#include <stdio.h>
#include <string.h>
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

// pinos dos botões
#define PIN_BOTAO_A 2
#define PIN_BOTAO_B 3

// Definição dos pinos dos LEDs
#define LED_AZUL_PIN 12  // GPIO12 - LED azul
#define LED_VERDE_PIN 11 // GPIO11 - LED verde
#define LED_VERMELHO_PIN 13   // GPIO13 - LED vermelho

// credenciais Wi-Fi (sempre usar)
#define WIFI_SSID "GERALDO_CASA"
#define WIFI_PASS "joanadarcnovo"
#define PORTA_HTTP 80

static bool estado_botao_a = false;
static bool estado_botao_b = false;

static err_t tcp_servidor_receber(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char *requisicao = (char *)malloc(p->len + 1);
    memcpy(requisicao, p->payload, p->len);
    requisicao[p->len] = '\0';

    printf("Requisição: %s\n", requisicao);

    // Controle dos LEDs
    if (strstr(requisicao, "GET /blue_on") != NULL)
    {
        gpio_put(LED_AZUL_PIN, 1);
    }
    else if (strstr(requisicao, "GET /blue_off") != NULL)
    {
        gpio_put(LED_AZUL_PIN, 0);
    }

    // Leitura da temperatura interna
    adc_select_input(4);
    uint16_t valor_bruto = adc_read();
    const float fator_conversao = 3.3f / (1 << 12);
    float temperatura = 27.0f - ((valor_bruto * fator_conversao) - 0.706f) / 0.001721f;

    // Cria a resposta HTML
    char html[1024];

    snprintf(html, sizeof(html),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<!DOCTYPE html>\n"
             "<html>\n"
             "<head>\n"
             "<title>Controle de LEDs</title>\n"
             "<style>\n"
             "body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }\n"
             "h1 { font-size: 64px; margin-bottom: 30px; }\n"
             "button { font-size: 36px; margin: 10px; padding: 20px 40px; border-radius: 10px; }\n"
             ".temperatura { font-size: 48px; margin-top: 30px; color: #333; }\n"
             "</style>\n"
             "</head>\n"
             "<body>\n"
             "<h1>Controle de LEDs</h1>\n"
             "<form action=\"./blue_on\"><button>Ligar Azul</button></form>\n"
             "<form action=\"./blue_off\"><button>Desligar Azul</button></form>\n"
             "<form action=\"./green_on\"><button>Ligar Verde</button></form>\n"
             "<form action=\"./green_off\"><button>Desligar Verde</button></form>\n"
             "<form action=\"./red_on\"><button>Ligar Vermelho</button></form>\n"
             "<form action=\"./red_off\"><button>Desligar Vermelho</button></form>\n"
             "<p class=\"temperatura\">Temperatura Interna: %.2f &deg;C</p>\n"
             "</body>\n"
             "</html>\n",
             temperatura);

    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    free(requisicao);
    pbuf_free(p);

    return ERR_OK;
}

static err_t tcp_servidor_aceitar(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_servidor_receber);
    return ERR_OK;
}

void inicializar_stdio()
{
    stdio_init_all();
    printf("Iniciando monitor de botões...\n");
}

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

void inicializar_sensor_temperatura()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
}

void inicializar_wifi()
{
    int codigo_erro = cyw43_arch_init();

    while (codigo_erro)
    {
        printf("Falha ao inicializar Rádio Wi-Fi: %d\n", codigo_erro);
        sleep_ms(100);
        codigo_erro = cyw43_arch_init();
    }

    cyw43_arch_enable_sta_mode();

    int codigo_erro_conexao = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 20000);

    while (codigo_erro_conexao)
    {
        printf("Falha ao conectar no roteador Wi-Fi: %d\n", codigo_erro_conexao);
        sleep_ms(100);
        codigo_erro_conexao = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 10000);
    }

    printf("Conectado ao Wi-Fi: %s\n", WIFI_SSID);

    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }
}

void inicializar_servidor()
{

    struct tcp_pcb *pcb = tcp_new();

    if (!pcb)
    {
        printf("Erro ao criar PCB TCP\n");
        return;
    }

    err_t err = tcp_bind(pcb, IP_ADDR_ANY, PORTA_HTTP);

    if (err != ERR_OK)
    {
        printf("Erro ao vincular o PCB TCP: %d\n", err);
        return;
    }

    pcb = tcp_listen(pcb);

    if (!pcb)
    {
        printf("Erro ao escutar no PCB TCP\n");
        return;
    }

    tcp_accept(pcb, tcp_servidor_aceitar);
    printf("Servidor HTTP iniciado na porta %d\n", PORTA_HTTP);
}

int main()
{
    inicializar_stdio();
    inicializar_leds();
    inicializar_sensor_temperatura();
    inicializar_wifi();
    inicializar_servidor();
    while (true)
    {
        cyw43_arch_poll();
    }
    return 0;
}
