#include "Acionador.h"


Acionador::Acionador(uint8_t pino) : pinoGpio(pino), funcaoCallback(nullptr), valorCallback(nullptr), ultimoTempoAcionado(get_absolute_time())
{
    this->configurarPinoGpio(pino);
}

void Acionador::configurarPinoGpio(uint8_t pino)
{
    gpio_init(pino);
    gpio_set_dir(pino, GPIO_IN);
    gpio_pull_up(pino);
}

bool Acionador::debounceExpirado() const
{
    return absolute_time_diff_us(this->ultimoTempoAcionado, get_absolute_time()) > this->intervaloDebounceMs * 1000;
}

void Acionador::atualizarTempoAcionado()
{
    this->ultimoTempoAcionado = get_absolute_time();
}

void Acionador::invocarCallback()
{
    if (this->funcaoCallback)
    {
        this->funcaoCallback(this->valorCallback);
    }
}

bool Acionador::estaPressionadoAgora() const {
    return !gpio_get(this->pinoGpio);
}

void Acionador::registrarCallback(void (*callback)())
{
    this->funcaoCallback = [callback](void *)
    {
        callback();
    };
    this->valorCallback = nullptr;
}

bool Acionador::verificarAcionamento()
{
    bool estadoAtual = this->estaPressionadoAgora();

    if (!this->debounceExpirado())
        return false;

    if (!estadoAtual)
    {
        this->estadoAnterior = false;
    }

    if (estadoAtual && !this->estadoAnterior)
    {
        this->atualizarTempoAcionado();
        this->invocarCallback();
        this->estadoAnterior = estadoAtual;
        return true;
    }

    return false;
}
