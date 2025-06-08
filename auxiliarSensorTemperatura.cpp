#include "auxiliarSensorTemperatura.h"

float obter_temperatura_interna()
{
    adc_select_input(ADC_INPUT_TEMPERATURA);
    uint16_t valor_bruto = adc_read();
    return TEMPERATURA_BASE - ((valor_bruto * ADC_FATOR_CONVERSAO) - TEMPERATURA_OFFSET) / TEMPERATURA_ESCALA;
}


void inicializar_sensor_temperatura()
{
    adc_init();
    adc_set_temp_sensor_enabled(true);
}
