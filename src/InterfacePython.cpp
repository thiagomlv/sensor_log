/**
 ******************************************************************************
 * @file           : InterfacePython.cpp
 * @brief          : Implementação da classe InterfacePython
 * @note           : Trabalho Final - Etapa 2 - Linguagens de Programação - 2026.1
 *
 * @author         : Thiago Marques de Oliveira
 * @date           : Julho, 2026
 ******************************************************************************
 */

#include "InterfacePython.h"

#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <algorithm>

// -- Helpers de serialização JSON mínima -------------------------------------
// O projeto não usa bibliotecas JSON externas para manter dependências simples.
// O parsing do retorno é feito manualmente, já que o formato é bem definido.

namespace {

/** @brief Serializa um vetor de doubles como array JSON. */
std::string vecToJsonArray(const std::vector<double>& v) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        oss << v[i];
        if (i + 1 < v.size()) oss << ",";
    }
    oss << "]";
    return oss.str();
}

/** @brief Serializa um vetor de strings como array JSON. */
std::string vecStrToJsonArray(const std::vector<std::string>& v) {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < v.size(); ++i) {
        oss << "\"" << v[i] << "\"";
        if (i + 1 < v.size()) oss << ",";
    }
    oss << "]";
    return oss.str();
}

/**
 * @brief Extrai o valor de uma chave numérica de um JSON simples (flat).
 * @param json  String JSON.
 * @param chave Nome da chave.
 * @return Valor como double.
 * @throws std::runtime_error se a chave não for encontrada.
 */
double extrairDouble(const std::string& json, const std::string& chave) {
    std::string busca = "\"" + chave + "\":";
    size_t pos = json.find(busca);
    if (pos == std::string::npos)
        throw std::runtime_error("Chave '" + chave + "' nao encontrada no JSON: " + json);
    pos += busca.size();
    // pula espaços
    while (pos < json.size() && json[pos] == ' ') ++pos;
    size_t end = json.find_first_of(",}", pos);
    return std::stod(json.substr(pos, end - pos));
}

/**
 * @brief Extrai o valor de uma chave string de um JSON simples.
 * @param json  String JSON.
 * @param chave Nome da chave.
 * @return Valor como std::string (sem aspas).
 * @throws std::runtime_error se a chave não for encontrada.
 */
std::string extrairString(const std::string& json, const std::string& chave) {
    // Tenta com e sem espaco apos o ':'
    std::string busca = "\"" + chave + "\": \"";
    size_t pos = json.find(busca);
    if (pos == std::string::npos) {
        busca = "\"" + chave + "\":\"";
        pos = json.find(busca);
    }
    if (pos == std::string::npos)
        throw std::runtime_error("Chave '" + chave + "' nao encontrada no JSON: " + json);
    pos += busca.size();
    // Le ate a proxima aspa dupla nao escapada
    std::string result;
    while (pos < json.size()) {
        if (json[pos] == '\\' && pos + 1 < json.size()) {
            result += json[pos + 1];
            pos += 2;
        } else if (json[pos] == '"') {
            break;
        } else {
            result += json[pos++];
        }
    }
    return result;
}

/**
 * @brief Extrai um array de inteiros de uma chave JSON.
 * @param json  String JSON.
 * @param chave Nome da chave.
 * @return Vetor de inteiros.
 */
std::vector<int> extrairArrayInt(const std::string& json, const std::string& chave) {
    std::vector<int> result;
    std::string busca = "\"" + chave + "\": [";
    size_t pos = json.find(busca);
    if (pos == std::string::npos) {
        busca = "\"" + chave + "\":[";
        pos = json.find(busca);
    }
    if (pos == std::string::npos) return result;
    pos += busca.size();
    size_t end = json.find("]", pos);
    std::string arr = json.substr(pos, end - pos);
    if (arr.empty()) return result;
    std::istringstream ss(arr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        try { result.push_back(std::stoi(token)); } catch (...) {}
    }
    return result;
}

/**
 * @brief Extrai um array de doubles de uma chave JSON.
 * @param json  String JSON.
 * @param chave Nome da chave.
 * @return Vetor de doubles.
 */
std::vector<double> extrairArrayDouble(const std::string& json, const std::string& chave) {
    std::vector<double> result;
    std::string busca = "\"" + chave + "\": [";
    size_t pos = json.find(busca);
    if (pos == std::string::npos) {
        busca = "\"" + chave + "\":[";
        pos = json.find(busca);
    }
    if (pos == std::string::npos) return result;
    pos += busca.size();
    size_t end = json.find("]", pos);
    std::string arr = json.substr(pos, end - pos);
    if (arr.empty()) return result;
    std::istringstream ss(arr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        try { result.push_back(std::stod(token)); } catch (...) {}
    }
    return result;
}

} // namespace anônimo

// -- InterfacePython ----------------------------------------------------------

InterfacePython::InterfacePython(const std::string& scriptPath)
    : m_scriptPath(scriptPath)
{}

std::string InterfacePython::escaparAspas(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\'') out += "'\\''"; // escapa aspa simples para o shell
        else           out += c;
    }
    return out;
}

std::string InterfacePython::executar(const std::string& funcao,
                                       const std::string& payload) const {
    std::string payloadEscapado = escaparAspas(payload);
    std::string cmd = "python3 " + m_scriptPath +
                      " " + funcao +
                      " '" + payloadEscapado + "' 2>&1";

    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("Falha ao abrir pipe com o modulo Python.");

    std::string resultado;
    char buffer[512];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        resultado += buffer;

    int exitCode = pclose(pipe);

    // Remove newline final se houver
    if (!resultado.empty() && resultado.back() == '\n')
        resultado.pop_back();

    // Verifica se o Python retornou um erro no JSON
    if (resultado.find("\"erro\"") != std::string::npos) {
        std::string msg = extrairString(resultado, "erro");
        throw std::runtime_error("Erro no modulo Python: " + msg);
    }

    if (resultado.empty())
        throw std::runtime_error("Modulo Python nao retornou dados (exit code: " +
                                  std::to_string(exitCode) + ").");

    return resultado;
}

Estatisticas InterfacePython::calcularEstatisticas(const std::vector<double>& valores) const {
    if (valores.empty())
        throw std::invalid_argument("Nao ha leituras para calcular estatisticas.");

    std::string payload = "{\"valores\":" + vecToJsonArray(valores) + "}";
    std::string resposta = executar("estatisticas", payload);

    Estatisticas e;
    e.media   = extrairDouble(resposta, "media");
    e.mediana = extrairDouble(resposta, "mediana");
    e.desvio  = extrairDouble(resposta, "desvio");
    e.minimo  = extrairDouble(resposta, "min");
    e.maximo  = extrairDouble(resposta, "max");
    e.cv      = extrairDouble(resposta, "cv");
    return e;
}

ResultadoAnomalias InterfacePython::detectarAnomalias(const std::vector<double>& valores,
                                                        double k) const {
    if (valores.size() < 2)
        throw std::invalid_argument("Sao necessarias pelo menos 2 leituras para detectar anomalias.");

    std::ostringstream oss;
    oss << "{\"valores\":" << vecToJsonArray(valores) << ",\"k\":" << k << "}";
    std::string resposta = executar("anomalias", oss.str());

    ResultadoAnomalias r;
    r.indices = extrairArrayInt(resposta, "anomalias");
    r.zscores = extrairArrayDouble(resposta, "zscore");
    return r;
}

std::string InterfacePython::gerarGrafico(const std::vector<double>& valores,
                                           const std::vector<std::string>& labels,
                                           const std::string& titulo,
                                           const std::string& unidade) const {
    if (valores.size() != labels.size())
        throw std::invalid_argument("Vetores de valores e labels devem ter o mesmo tamanho.");

    std::ostringstream oss;
    oss << "{\"valores\":"  << vecToJsonArray(valores)
        << ",\"labels\":"   << vecStrToJsonArray(labels)
        << ",\"titulo\":\""  << titulo << "\""
        << ",\"unidade\":\"" << unidade << "\"}";

    std::string resposta = executar("grafico", oss.str());
    return extrairString(resposta, "arquivo");
}