#include "auxiliarWifi.h"

void conectar_wifi()
{
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

void definir_modo_estacao()
{
    cyw43_arch_enable_sta_mode();
    printf("Modo estação ativado.\n");
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

    definir_modo_estacao();
    conectar_wifi();
}

