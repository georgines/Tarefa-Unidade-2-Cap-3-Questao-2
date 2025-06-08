#include "servidor.h"
#include "auxiliarLeds.h"
#include "auxiliarSensorTemperatura.h"

// callback de envio concluído: agora sim fecha de verdade
static err_t ao_enviar_fechar(void *arg, struct tcp_pcb *pcb, u16_t len) {
    tcp_close(pcb);
    return ERR_OK;
}

err_t aceitar_conexao_tcp(void *arg, struct tcp_pcb *novo_pcb, err_t err) {
    // limpa buffer de requisição
    req_pos = 0;
    req_buf[0] = '\0';
    // registra callbacks
    tcp_arg(novo_pcb, novo_pcb);
    tcp_recv(novo_pcb, tratar_requisicao_tcp);
    return ERR_OK;
}

err_t tratar_requisicao_tcp(void *arg, struct tcp_pcb *pcb,
                           struct pbuf *buffer, err_t err) 
{
    if (!buffer) {
        // cliente fechou
        tcp_close(pcb);
        return ERR_OK;
    }

    // 1) diz pro LWIP que consumiu esses bytes
    tcp_recved(pcb, buffer->len);

    // 2) copia pro nosso buffer (sem ultrapassar)
    size_t copiar = buffer->len;
    if (req_pos + copiar > REQ_BUF_SIZE-1) copiar = REQ_BUF_SIZE-1 - req_pos;
    memcpy(req_buf + req_pos, buffer->payload, copiar);
    req_pos += copiar;
    req_buf[req_pos] = '\0';

    // libera o pbuf
    pbuf_free(buffer);

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
    int tamanho_corpo = snprintf(NULL, 0, body_fmt, temperatura);
    int tamanho_cabecalho  = snprintf(html, sizeof(html), header_fmt, tamanho_corpo);
    snprintf(html + tamanho_cabecalho, sizeof(html) - tamanho_cabecalho, body_fmt, temperatura);

    // 5) registra callback de envio concluído e envia
    tcp_sent(pcb, ao_enviar_fechar);
    tcp_write(pcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(pcb);

    // não esperar mais requisições neste pcb
    tcp_recv(pcb, NULL);

    return ERR_OK;
}

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