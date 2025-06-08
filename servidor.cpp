#include "servidor.h"
#include "ServidorHttp.h"
#include "sistema.h"
#include "auxiliar.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include <string>
#include <cstdio>
#include <cstring>

static ServidorHttp::Servidor servidor_http;

struct EstadoEnvioHttp
{
    std::string respostaCompleta;
    size_t bytesEnviados = 0;
};

static std::string gerar_pagina_html()
{
    char buf[TAMANHO_HTML];
    float temp = obter_temperatura_interna();
    std::snprintf(buf, sizeof(buf), body_fmt, temp);
    return std::string(buf);
}

static std::string gerar_status_json()
{
    char buf[128];
    float temp = obter_temperatura_interna();
    std::snprintf(buf, sizeof(buf), R"({"botao_a":%d,"botao_b":%d,"temperatura":%.2f})", botao_a_estado, botao_b_estado, temp);
    return std::string(buf);
}

static std::string gerar_favicon()
{
    return std::string();
}

static void registrar_rotas_http()
{
    servidor_http.adicionarRota(ServidorHttp::MetodoHTTP::GET, "/", gerar_pagina_html, ServidorHttp::TipoConteudo::HTML);
    servidor_http.adicionarRota(ServidorHttp::MetodoHTTP::GET, "/status", gerar_status_json, ServidorHttp::TipoConteudo::JSON);
    servidor_http.adicionarRota(ServidorHttp::MetodoHTTP::GET, "/favicon.ico", gerar_favicon, ServidorHttp::TipoConteudo::HTML);
    servidor_http.adicionarRota(ServidorHttp::MetodoHTTP::GET, "/az/toggle", alternar_led_azul, ServidorHttp::TipoConteudo::HTML);
    servidor_http.adicionarRota(ServidorHttp::MetodoHTTP::GET, "/vr/toggle", alternar_led_verde, ServidorHttp::TipoConteudo::HTML);
    servidor_http.adicionarRota(ServidorHttp::MetodoHTTP::GET, "/vm/toggle", alternar_led_vermelho, ServidorHttp::TipoConteudo::HTML);
}

static err_t continuar_envio_http(void *arg, tcp_pcb *pcb, u16_t)
{
    auto *estado = static_cast<EstadoEnvioHttp *>(arg);
    if (!pcb || !estado)
        return ERR_OK;

    size_t restante = estado->respostaCompleta.size() - estado->bytesEnviados;
    if (restante == 0)
    {
        delete estado;
        tcp_arg(pcb, nullptr);
        return tcp_close(pcb);
    }

    u16_t tamanhoMax = tcp_sndbuf(pcb);
    size_t bloco = std::min(restante, static_cast<size_t>(tamanhoMax));
    const char *dados = estado->respostaCompleta.data() + estado->bytesEnviados;

    err_t erro = tcp_write(pcb, dados, bloco, TCP_WRITE_FLAG_COPY);
    if (erro == ERR_OK)
    {
        estado->bytesEnviados += bloco;
        tcp_output(pcb);
    }

    return erro;
}

static void enviar_resposta_http(tcp_pcb *pcb, const std::string &resposta)
{
    if (!pcb || resposta.empty())
        return;
    EstadoEnvioHttp *estado = new EstadoEnvioHttp{resposta, 0};
    tcp_arg(pcb, estado);
    tcp_sent(pcb, continuar_envio_http);
    continuar_envio_http(estado, pcb, 0);
}

static void enviar_404(tcp_pcb *pcb)
{
    if (!pcb)
        return;
    const char *hdr = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
    tcp_write(pcb, hdr, strlen(hdr), TCP_WRITE_FLAG_COPY);
    tcp_sent(pcb, [](void *arg, tcp_pcb *p, u16_t)
             { return tcp_close(p); });
    tcp_output(pcb);
}

static err_t callback_receber_dados(void *, tcp_pcb *pcb, pbuf *p, err_t)
{
    if (!pcb || !p)
        return ERR_OK;
    tcp_recved(pcb, p->len);

    std::string req(reinterpret_cast<char *>(p->payload), p->len);
    pbuf_free(p);

    std::string resp;
    if (servidor_http.processarRequisicao(req, resp))
    {
        enviar_resposta_http(pcb, resp);
    }
    else
    {
        enviar_404(pcb);
    }

    return ERR_OK;
}

static err_t callback_aceitar_conexao(void *, tcp_pcb *new_pcb, err_t)
{
    if (!new_pcb)
        return ERR_OK;
    tcp_recv(new_pcb, callback_receber_dados);
    return ERR_OK;
}

void inicializar_servidor()
{
    registrar_rotas_http();

    tcp_pcb *pcb = tcp_new();
    if (!pcb)
        return;
    tcp_bind(pcb, IP_ADDR_ANY, PORTA_HTTP);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, callback_aceitar_conexao);
}
