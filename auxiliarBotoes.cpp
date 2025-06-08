#include "auxiliarBotoes.h"

Acionador BotaoA(PIN_BOTAO_A);
Acionador BotaoB(PIN_BOTAO_B);

void monitorar_botoes() {
    botao_a_acionado = BotaoA.verificarAcionamento();
    botao_b_acionado = BotaoB.verificarAcionamento();

    if (botao_a_acionado) {
        printf("Botão A pressionado\n");
    }
    if (botao_b_acionado) {
        printf("Botão B pressionado\n");
    }
}
