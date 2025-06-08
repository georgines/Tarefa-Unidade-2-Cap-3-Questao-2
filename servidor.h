#pragma once

#include "sistema.h"
#include "servidor.h"
#include "auxiliarLeds.h"
#include "auxiliarSensorTemperatura.h"

static err_t ao_enviar_fechar(void *arg, struct tcp_pcb *tpcb, u16_t len);

err_t aceitar_conexao_tcp(void *arg, struct tcp_pcb *novo_pcb, err_t err);

err_t tratar_requisicao_tcp(void *arg, struct tcp_pcb *pcb, struct pbuf *buffer, err_t err);

void inicializar_mapa_acoes_led();

void executar_acao_led(const char *requisicao);

void inicializar_servidor();