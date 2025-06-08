#include "servidor.h"
#include "auxiliarLeds.h"
#include "auxiliarSensorTemperatura.h"

// callback de envio concluído: agora sim fecha de verdade
static err_t on_sent_close(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    tcp_close(tpcb);
    return ERR_OK;
}

err_t aceitar_conexao_tcp(void *arg, struct tcp_pcb *newpcb, err_t err) {
    // limpa buffer de requisição
    req_pos = 0;
    req_buf[0] = '\0';
    // registra callbacks
    tcp_arg(newpcb, newpcb);
    tcp_recv(newpcb, tratar_requisicao_tcp);
    return ERR_OK;
}

err_t tratar_requisicao_tcp(void *arg, struct tcp_pcb *tpcb,
                           struct pbuf *p, err_t err) 
{
    if (!p) {
        // cliente fechou
        tcp_close(tpcb);
        return ERR_OK;
    }

    // 1) diz pro LWIP que consumiu esses bytes
    tcp_recved(tpcb, p->len);

    // 2) copia pro nosso buffer (sem ultrapassar)
    size_t to_copy = p->len;
    if (req_pos + to_copy > REQ_BUF_SIZE-1) to_copy = REQ_BUF_SIZE-1 - req_pos;
    memcpy(req_buf + req_pos, p->payload, to_copy);
    req_pos += to_copy;
    req_buf[req_pos] = '\0';

    // libera o pbuf
    pbuf_free(p);

    // 3) só processa QUANDO vir o final de cabeçalho
    if (!strstr(req_buf, "\r\n\r\n")) {
        // ainda não chegou tudo
        return ERR_OK;
    }

    // opcional: remover tudo após \r\n\r\n se quiser reusar req_buf
    // 

    printf("Requisição completa:\n%s\n", req_buf);
    executar_acao_led(req_buf);

    // 4) gera HTML
    char html[TAMANHO_HTML];
    float temperatura = obter_temperatura_interna();

    // calcula e monta resposta
    int body_len = snprintf(NULL, 0, body_fmt, temperatura);
    int hdr_len  = snprintf(html, sizeof(html), header_fmt, body_len);
    snprintf(html + hdr_len, sizeof(html) - hdr_len, body_fmt, temperatura);

    // 5) registra callback de envio concluído e envia
    tcp_sent(tpcb, on_sent_close);
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    // não esperar mais requisições neste pcb
    tcp_recv(tpcb, NULL);

    return ERR_OK;
}

// err_t aceitar_conexao_tcp(void *arg, struct tcp_pcb *newpcb, err_t err)
// {
//     tcp_recv(newpcb, tratar_requisicao_tcp);
//     return ERR_OK;
// }

void inicializar_mapa_acoes_led() {
    mapa_acoes_led[0] = {"GET /blue_on", ligar_led_azul};
    mapa_acoes_led[1] = {"GET /blue_off", desligar_led_azul};
    mapa_acoes_led[2] = {"GET /green_on", ligar_led_verde};
    mapa_acoes_led[3] = {"GET /green_off", desligar_led_verde};
    mapa_acoes_led[4] = {"GET /red_on", ligar_led_vermelho};
    mapa_acoes_led[5] = {"GET /red_off", desligar_led_vermelho};
}

void executar_acao_led(const char *requisicao) {
    for (size_t i = 0; i < sizeof(mapa_acoes_led) / sizeof(mapa_acoes_led[0]); ++i) {
        if (strstr(requisicao, mapa_acoes_led[i].requisicao)) {
            mapa_acoes_led[i].acao();
            return;
        }
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

    tcp_accept(pcb, aceitar_conexao_tcp);
    printf("Servidor HTTP iniciado na porta %d\n", PORTA_HTTP);
}