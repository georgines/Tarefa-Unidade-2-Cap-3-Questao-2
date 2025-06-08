#include "servidor.h"
#include "ServidorHttp.h"
#include "sistema.h"
#include "auxiliarSensorTemperatura.h"
#include "auxiliarBotoes.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include <string>
#include <cstdio>
#include <cstring>

static ServidorHttp::Servidor servidor_http;

static std::string gerar_pagina_html()
{
    char buf[1024];
    float temp = obter_temperatura_interna();
    std::snprintf(buf, sizeof(buf), body_fmt, temp);
    return std::string(buf);
}

static std::string gerar_status_json()
{
    char buf[128];
    float temp = obter_temperatura_interna();
    std::snprintf(buf, sizeof(buf),
                  R"({"buttonA":%d,"buttonB":%d,"temperature":%.2f})",
                  botao_a_acionado,
                  botao_b_acionado,
                  temp);
    return std::string(buf);
}

static std::string gerar_favicon()
{
    return std::string();
}

static void registrar_rotas_http()
{
    servidor_http.adicionarRota(
        ServidorHttp::MetodoHTTP::GET, "/", gerar_pagina_html, ServidorHttp::TipoConteudo::HTML
    );
    servidor_http.adicionarRota(
        ServidorHttp::MetodoHTTP::GET, "/status", gerar_status_json, ServidorHttp::TipoConteudo::JSON
    );
    servidor_http.adicionarRota(
        ServidorHttp::MetodoHTTP::GET, "/favicon.ico", gerar_favicon, ServidorHttp::TipoConteudo::HTML
    );
}

static err_t callback_fechar_conexao(void* /*arg*/, struct tcp_pcb* pcb, u16_t /*len*/)
{
    tcp_close(pcb);
    return ERR_OK;
}

static void enviar_resposta_http(struct tcp_pcb* pcb, const std::string& resposta)
{
    if (!pcb || resposta.empty()) return; // Prevenir ponteiros vazios
    tcp_write(pcb, resposta.data(), resposta.size(), TCP_WRITE_FLAG_COPY);
    tcp_sent(pcb, callback_fechar_conexao);
    tcp_output(pcb);
}

static void enviar_404(struct tcp_pcb* pcb)
{
    if (!pcb) return; // Prevenir ponteiros vazios
    const char* hdr =
        "HTTP/1.1 404 Not Found\r\n"
        "Connection: close\r\n"
        "Content-Length: 0\r\n"
        "\r\n";
    tcp_write(pcb, hdr, strlen(hdr), TCP_WRITE_FLAG_COPY);
    tcp_sent(pcb, callback_fechar_conexao);
    tcp_output(pcb);
}

static err_t callback_receber_dados(void* /*arg*/, struct tcp_pcb* pcb, struct pbuf* p, err_t /*err*/)
{
    if (!pcb || !p) return ERR_OK; // Prevenir ponteiros vazios

    tcp_recved(pcb, p->len);

    {
        size_t len = p->len < 512 ? p->len : 511;
        char debug_buf[512];
        std::memcpy(debug_buf, p->payload, len);
        debug_buf[len] = '\0';
        printf("Depuração - requisição recebida:\n%s\n", debug_buf);
    }

    std::string req(reinterpret_cast<char*>(p->payload), p->len);
    pbuf_free(p);

    std::string resp;
    if (servidor_http.processarRequisicao(req, resp)) {
        enviar_resposta_http(pcb, resp);
    } else {
        enviar_404(pcb);
    }

    return ERR_OK;
}

static err_t callback_aceitar_conexao(void* /*arg*/, struct tcp_pcb* new_pcb, err_t /*err*/)
{
    if (!new_pcb) return ERR_OK; // Prevenir ponteiros vazios
    tcp_arg(new_pcb, new_pcb);
    tcp_recv(new_pcb, callback_receber_dados);
    return ERR_OK;
}

void inicializar_servidor()
{
    registrar_rotas_http();

    struct tcp_pcb* pcb = tcp_new();
    if (!pcb) return;
    tcp_bind(pcb, IP_ADDR_ANY, 80);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, callback_aceitar_conexao);
}
