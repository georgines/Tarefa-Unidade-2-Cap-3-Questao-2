#include "sistema.h"
#include "auxiliar.h"
#include "servidor.h"
#include "auxiliarWifi.h"

void inicializar_stdio()
{
    stdio_init_all();
    printf("Iniciando monitor de botões...\n");
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
        monitorar_botoes();
    }
    return 0;
}
