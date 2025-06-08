#pragma once

#include "sistema.h"

err_t tratar_requisicao_tcp(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

err_t aceitar_conexao_tcp(void *arg, struct tcp_pcb *newpcb, err_t err);

void inicializar_mapa_acoes_led();

void executar_acao_led(const char *requisicao);

void gerar_resposta_html(char *html, size_t tamanho, float temperatura);

void inicializar_servidor();