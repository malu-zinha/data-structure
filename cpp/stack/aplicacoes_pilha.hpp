// APLICAÇÕES CLÁSSICAS DE PILHA
//
// 1. parenteses_balanceados   -> "([]{})" está bem formado?
// 2. infixa_para_posfixa      -> "A + B * C" vira "A B C * +" (algoritmo de Shunting-yard)
// 3. avaliar_posfixa          -> calcula "3 4 2 * +" = 11
// 4. avaliar_infixa           -> junta os dois acima: "3 + 4 * 2" = 11
// 5. inverter_texto           -> "abc" vira "cba"
// 6. eh_palindromo            -> "Ame a ema" é palíndromo?
// 7. converter_base           -> 10 na base 2 = "1010" (divisões sucessivas)
// 8. torre_de_hanoi           -> resolve Hanói com 3 pilhas de verdade
// 9. proximo_maior_elemento   -> para cada item, o próximo maior à direita (pilha monotônica)
#pragma once

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "pilha_encadeada.hpp"

// ----------------------------------------------------------------------
// 1. Parênteses balanceados
// ----------------------------------------------------------------------
inline char abre_de(char fecha) {
    switch (fecha) {
        case ')': return '(';
        case ']': return '[';
        case '}': return '{';
    }
    return 0;
}

// Todo símbolo que abre empilha; todo que fecha precisa casar com o topo.
inline bool parenteses_balanceados(const std::string& expressao) {
    PilhaEncadeada<char> pilha;
    for (char simbolo : expressao) {
        if (simbolo == '(' || simbolo == '[' || simbolo == '{') {
            pilha.empilhar(simbolo);
        } else if (abre_de(simbolo) != 0) {
            if (pilha.vazia() || pilha.desempilhar() != abre_de(simbolo)) return false;
        }
    }
    return pilha.vazia();  // sobrou algum aberto? então não está balanceado
}

// ----------------------------------------------------------------------
// 2, 3 e 4. Expressões infixa / pós-fixa
// ----------------------------------------------------------------------
inline int precedencia(const std::string& operador) {
    if (operador == "+" || operador == "-") return 1;
    if (operador == "*" || operador == "/") return 2;
    if (operador == "^") return 3;
    return 0;  // não é operador
}

inline bool eh_operador(const std::string& token) { return precedencia(token) > 0; }

// Quebra a expressão em números, variáveis, operadores e parênteses.
inline std::vector<std::string> tokenizar(const std::string& expressao) {
    std::vector<std::string> tokens;
    std::size_t i = 0;
    while (i < expressao.size()) {
        unsigned char c = expressao[i];
        if (std::isspace(c)) {
            i++;
        } else if (std::isdigit(c)) {  // número: 12 ou 3.5
            std::size_t fim = i;
            while (fim < expressao.size() && std::isdigit(static_cast<unsigned char>(expressao[fim]))) fim++;
            if (fim + 1 < expressao.size() && expressao[fim] == '.' &&
                std::isdigit(static_cast<unsigned char>(expressao[fim + 1]))) {
                fim++;
                while (fim < expressao.size() && std::isdigit(static_cast<unsigned char>(expressao[fim]))) fim++;
            }
            tokens.push_back(expressao.substr(i, fim - i));
            i = fim;
        } else if (std::isalpha(c) || c == '_') {  // variável: x, total_1
            std::size_t fim = i;
            while (fim < expressao.size() &&
                   (std::isalnum(static_cast<unsigned char>(expressao[fim])) || expressao[fim] == '_'))
                fim++;
            tokens.push_back(expressao.substr(i, fim - i));
            i = fim;
        } else {
            std::string simbolo(1, static_cast<char>(c));
            if (!eh_operador(simbolo) && simbolo != "(" && simbolo != ")")
                throw std::invalid_argument("Símbolo inválido: " + simbolo);
            tokens.push_back(simbolo);
            i++;
        }
    }
    return tokens;
}

// Converte notação infixa para pós-fixa (notação polonesa reversa).
//
// Regras (Shunting-yard, de Dijkstra):
// - operando  -> vai direto para a saída;
// - "("       -> empilha;
// - ")"       -> desempilha para a saída até achar o "(";
// - operador  -> antes de empilhar, desempilha os operadores do topo que têm
//                precedência MAIOR (ou IGUAL, se for associativo à esquerda);
// - no fim    -> desempilha tudo que sobrou.
// Não trata menos unário (ex.: "-3").
inline std::string infixa_para_posfixa(const std::string& expressao) {
    PilhaEncadeada<std::string> pilha;
    std::vector<std::string> saida;
    for (const std::string& token : tokenizar(expressao)) {
        if (token == "(") {
            pilha.empilhar(token);
        } else if (token == ")") {
            while (!pilha.vazia() && pilha.topo() != "(") saida.push_back(pilha.desempilhar());
            if (pilha.vazia()) throw std::invalid_argument("Parênteses desbalanceados");
            pilha.desempilhar();  // descarta o "("
        } else if (eh_operador(token)) {
            while (!pilha.vazia() && pilha.topo() != "(") {
                int do_topo = precedencia(pilha.topo());
                int do_token = precedencia(token);
                bool associativo_a_direita = token == "^";  // 2 ^ 3 ^ 2 = 2 ^ (3 ^ 2)
                bool sai_antes = do_topo > do_token || (do_topo == do_token && !associativo_a_direita);
                if (!sai_antes) break;
                saida.push_back(pilha.desempilhar());
            }
            pilha.empilhar(token);
        } else {
            saida.push_back(token);  // operando
        }
    }
    while (!pilha.vazia()) {
        std::string topo = pilha.desempilhar();
        if (topo == "(") throw std::invalid_argument("Parênteses desbalanceados");
        saida.push_back(topo);
    }

    std::string resultado;
    for (std::size_t i = 0; i < saida.size(); i++) resultado += (i > 0 ? " " : "") + saida[i];
    return resultado;
}

inline double aplicar(const std::string& operador, double a, double b) {
    if (operador == "+") return a + b;
    if (operador == "-") return a - b;
    if (operador == "*") return a * b;
    if (operador == "/") return a / b;
    return std::pow(a, b);  // "^"
}

// Calcula uma expressão pós-fixa com tokens separados por espaço.
// Operando -> empilha. Operador -> desempilha DOIS, calcula e empilha o resultado.
// Pega-ratão: o primeiro desempilhado é o operando da DIREITA ("5 2 -" = 5 - 2).
inline double avaliar_posfixa(const std::string& expressao, const std::map<std::string, double>& variaveis = {}) {
    PilhaEncadeada<double> pilha;
    std::istringstream entrada(expressao);
    std::string token;
    while (entrada >> token) {
        if (eh_operador(token)) {
            if (pilha.tamanho() < 2) throw std::invalid_argument("Expressão mal formada");
            double direita = pilha.desempilhar();
            double esquerda = pilha.desempilhar();
            pilha.empilhar(aplicar(token, esquerda, direita));
        } else if (std::isdigit(static_cast<unsigned char>(token[0]))) {
            pilha.empilhar(std::strtod(token.c_str(), nullptr));
        } else if (variaveis.count(token)) {
            pilha.empilhar(variaveis.at(token));
        } else {
            throw std::invalid_argument("Token desconhecido: " + token);
        }
    }
    if (pilha.tamanho() != 1) throw std::invalid_argument("Expressão mal formada");
    return pilha.desempilhar();
}

inline double avaliar_infixa(const std::string& expressao, const std::map<std::string, double>& variaveis = {}) {
    return avaliar_posfixa(infixa_para_posfixa(expressao), variaveis);
}

// ----------------------------------------------------------------------
// 5 e 6. Inversão e palíndromo
// ----------------------------------------------------------------------
inline std::string inverter_texto(const std::string& texto) {
    PilhaEncadeada<char> pilha;
    for (char caractere : texto) pilha.empilhar(caractere);
    std::string invertido;
    while (!pilha.vazia()) invertido += pilha.desempilhar();
    return invertido;
}

// Letras e dígitos do texto, em minúsculas e sem acento.
// Letras acentuadas em UTF-8 (á, ç, ô...) ocupam 2 bytes: 0xC3 seguido de
// 0x80..0xBF, que correspondem aos caracteres U+00C0..U+00FF. A tabela diz a
// letra sem acento de cada um ('.' = não é letra).
inline std::string letras_sem_acento(const std::string& texto) {
    static const char* tabela = "aaaaaaaceeeeiiiidnooooo.ouuuuy.saaaaaaaceeeeiiiidnooooo.ouuuuy.y";
    std::string letras;
    for (std::size_t i = 0; i < texto.size(); i++) {
        unsigned char c = texto[i];
        if (std::isalnum(c)) {
            letras += static_cast<char>(std::tolower(c));
        } else if (c == 0xC3 && i + 1 < texto.size()) {
            unsigned char seguinte = texto[++i];
            if (seguinte >= 0x80 && seguinte <= 0xBF && tabela[seguinte - 0x80] != '.')
                letras += tabela[seguinte - 0x80];
        }
    }
    return letras;
}

// Ignora espaços, pontuação, maiúsculas e acentos.
inline bool eh_palindromo(const std::string& texto) {
    std::string letras = letras_sem_acento(texto);
    PilhaEncadeada<char> pilha;
    for (char letra : letras) pilha.empilhar(letra);
    // a pilha devolve as letras na ordem inversa: basta comparar uma a uma
    for (char letra : letras)
        if (letra != pilha.desempilhar()) return false;
    return true;
}

// ----------------------------------------------------------------------
// 7. Conversão de base
// ----------------------------------------------------------------------
// Divisões sucessivas: os restos saem de trás para frente -> pilha!
inline std::string converter_base(long long numero, int base = 2) {
    if (base < 2 || base > 16) throw std::invalid_argument("Base deve estar entre 2 e 16");
    if (numero == 0) return "0";
    const char* digitos = "0123456789ABCDEF";
    std::string sinal = numero < 0 ? "-" : "";
    numero = std::llabs(numero);

    PilhaEncadeada<char> pilha;
    while (numero > 0) {
        pilha.empilhar(digitos[numero % base]);
        numero /= base;
    }
    std::string resultado = sinal;
    while (!pilha.vazia()) resultado += pilha.desempilhar();
    return resultado;
}

// ----------------------------------------------------------------------
// 8. Torre de Hanói
// ----------------------------------------------------------------------
using Movimento = std::tuple<int, char, char>;  // (disco, origem, destino)

namespace detalhe {
inline void mover(int quantidade, int origem, int destino, int auxiliar, PilhaEncadeada<int> torres[3],
                  std::vector<Movimento>& movimentos) {
    if (quantidade == 0) return;
    mover(quantidade - 1, origem, auxiliar, destino, torres, movimentos);  // tira os de cima do caminho
    int disco = torres[origem].desempilhar();
    // regra do jogo: nunca colocar disco maior sobre um menor
    if (!torres[destino].vazia() && torres[destino].topo() < disco) throw std::logic_error("Movimento inválido");
    torres[destino].empilhar(disco);
    movimentos.emplace_back(disco, static_cast<char>('A' + origem), static_cast<char>('A' + destino));
    mover(quantidade - 1, auxiliar, destino, origem, torres, movimentos);  // coloca-os de volta por cima
}
}  // namespace detalhe

// Move n discos da torre A para a C usando B. Cada torre é uma pilha.
// Devolve a lista de movimentos. Total = 2^n - 1.
inline std::vector<Movimento> torre_de_hanoi(int n) {
    PilhaEncadeada<int> torres[3];                           // A, B e C
    for (int disco = n; disco >= 1; disco--) torres[0].empilhar(disco);  // disco maior embaixo
    std::vector<Movimento> movimentos;
    detalhe::mover(n, 0, 2, 1, torres, movimentos);
    return movimentos;
}

// ----------------------------------------------------------------------
// 9. Próximo maior elemento (pilha monotônica)
// ----------------------------------------------------------------------
// {4, 5, 2, 25} -> {5, 25, 25, nenhum} em O(n).
// A pilha guarda ÍNDICES de quem ainda está esperando um valor maior.
inline std::vector<std::optional<int>> proximo_maior_elemento(const std::vector<int>& valores) {
    std::vector<std::optional<int>> resposta(valores.size());
    PilhaEncadeada<std::size_t> pilha;
    for (std::size_t i = 0; i < valores.size(); i++) {
        while (!pilha.vazia() && valores[pilha.topo()] < valores[i]) resposta[pilha.desempilhar()] = valores[i];
        pilha.empilhar(i);
    }
    return resposta;
}
