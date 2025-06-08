#include "ServidorHttp.h"
#include <sstream>

namespace ServidorHttp
{

    void Servidor::adicionarRota(MetodoHTTP metodo,
                                 const std::string &caminho,
                                 FuncaoResposta resposta,
                                 TipoConteudo tipo)
    {
        if (!resposta) return;
        _rotas.push_back({metodo, caminho, resposta, tipo});
    }

    bool Servidor::processarRequisicao(const std::string &requisicao,
                                       std::string &respostaHTTP)
    {
        if (requisicao.empty()) return false;

        auto metodoReq = extrairMetodo(requisicao);
        auto caminhoReq = extrairCaminho(requisicao);

        for (const auto &rota : _rotas)
        {
            if (metodoReq == metodoParaString(rota.metodo) && caminhoReq == rota.caminho)
            {
                if (!rota.resposta) return false;
                std::string corpo = rota.resposta();
                std::string cab = montarCabecalho(rota.tipo, corpo.size());
                respostaHTTP = cab + corpo;
                return true;
            }
        }
        return false;
    }

    std::string Servidor::extrairMetodo(const std::string &req) const
    {
        std::istringstream iss(req);
        std::string m;
        iss >> m;
        return m;
    }

    std::string Servidor::extrairCaminho(const std::string &req) const
    {
        std::istringstream iss(req);
        std::string m, caminho;
        iss >> m >> caminho;
        return caminho;
    }

    std::string Servidor::montarCabecalho(TipoConteudo tipo,
                                          size_t comprimento) const
    {
        const char *ct = (tipo == TipoConteudo::JSON)
                             ? "application/json"
                             : "text/html";
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: " << ct << "\r\n"
            << "Content-Length: " << comprimento << "\r\n"
            << "Connection: close\r\n\r\n";
        return oss.str();
    }

    std::string Servidor::metodoParaString(MetodoHTTP metodo) const
    {
        switch (metodo)
        {
        case MetodoHTTP::GET:
            return "GET";
        case MetodoHTTP::POST:
            return "POST";
        case MetodoHTTP::PUT:
            return "PUT";
        case MetodoHTTP::DELETE:
            return "DELETE";
        }
        return "";
    }

}