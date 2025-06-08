#pragma once
#include "pico/stdlib.h"
#include <functional>
#include <type_traits>

class Acionador
{
private:
    uint8_t pinoGpio;
    std::function<void(void *)> funcaoCallback;
    void *valorCallback;
    absolute_time_t ultimoTempoAcionado;
    const uint16_t intervaloDebounceMs = 50;
    bool estadoAnterior = false;

    void configurarPinoGpio(uint8_t pino);
    bool debounceExpirado() const;
    void atualizarTempoAcionado();
    void invocarCallback();

public:
    Acionador(uint8_t pino);
    void registrarCallback(void (*callback)());

    template <typename T>
    void registrarCallback(void (*callback)(T *), T *valor)
    {
        static_assert(std::is_pointer<T *>::value, "O valor deve ser um ponteiro.");
        this->funcaoCallback = [callback](void *dados)
        {
            callback(static_cast<T *>(dados));
        };
        this->valorCallback = static_cast<void *>(valor);
    }

    bool verificarAcionamento();
};


