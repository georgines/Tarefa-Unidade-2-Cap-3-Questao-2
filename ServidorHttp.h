#pragma once

#include <string>
#include <vector>
#include <functional>

namespace ServidorHttp {

enum class MetodoHTTP { GET, POST, PUT, DELETE };

enum class TipoConteudo { HTML, JSON };

using FuncaoResposta = std::function<std::string()>;

class Servidor {
public:
    void adicionarRota(MetodoHTTP metodo,
                       const std::string& caminho,
                       FuncaoResposta resposta,
                       TipoConteudo tipo);

    bool processarRequisicao(const std::string& requisicao,
                             std::string& respostaHTTP);

private:
    struct Rota {
        MetodoHTTP    metodo;
        std::string   caminho;
        FuncaoResposta resposta;
        TipoConteudo  tipo;
    };

    std::vector<Rota> _rotas;

    std::string extrairMetodo(const std::string& req) const;
    std::string extrairCaminho(const std::string& req) const;
    std::string montarCabecalho(TipoConteudo tipo, size_t comprimento) const;
    std::string metodoParaString(MetodoHTTP metodo) const;
};

}
