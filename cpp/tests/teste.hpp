// Mini framework de testes, sem dependências.
//
//   TESTE(nome_do_teste) {
//       VERIFICAR(condicao);
//       VERIFICAR_IGUAL(obtido, esperado);
//       VERIFICAR_LANCA(expressao, TipoDaExcecao);
//   }
//
// Cada arquivo test_*.cpp termina com `int main() { return rodar_testes(); }`.
#pragma once

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct FalhaDeTeste : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct Teste {
    std::string nome;
    std::function<void()> funcao;
};

inline std::vector<Teste>& testes_registrados() {
    static std::vector<Teste> testes;
    return testes;
}

struct RegistroDeTeste {
    RegistroDeTeste(const char* nome, std::function<void()> funcao) { testes_registrados().push_back({nome, funcao}); }
};

#define TESTE(nome)                                          \
    static void nome();                                      \
    static RegistroDeTeste registro_##nome(#nome, nome);     \
    static void nome()

#define FALHAR(mensagem) \
    throw FalhaDeTeste(std::string(__FILE__) + ":" + std::to_string(__LINE__) + ": " + (mensagem))

#define VERIFICAR(condicao)                              \
    do {                                                 \
        if (!(condicao)) FALHAR("VERIFICAR(" #condicao ")"); \
    } while (false)

#define VERIFICAR_IGUAL(obtido, esperado)                                                    \
    do {                                                                                     \
        if (!((obtido) == (esperado))) FALHAR("VERIFICAR_IGUAL(" #obtido ", " #esperado ")"); \
    } while (false)

#define VERIFICAR_LANCA(expressao, Excecao)                                       \
    do {                                                                          \
        bool lancou = false;                                                      \
        try {                                                                     \
            (void)(expressao);                                                    \
        } catch (const Excecao&) {                                                \
            lancou = true;                                                        \
        }                                                                         \
        if (!lancou) FALHAR("VERIFICAR_LANCA(" #expressao ", " #Excecao ")");     \
    } while (false)

inline int rodar_testes() {
    int falhas = 0;
    for (const Teste& teste : testes_registrados()) {
        try {
            teste.funcao();
        } catch (const FalhaDeTeste& erro) {
            falhas++;
            std::cout << "FALHOU " << teste.nome << "\n    " << erro.what() << "\n";
        } catch (const std::exception& erro) {
            falhas++;
            std::cout << "FALHOU " << teste.nome << "\n    exceção inesperada: " << erro.what() << "\n";
        }
    }
    int total = static_cast<int>(testes_registrados().size());
    std::cout << (total - falhas) << "/" << total << " testes passaram\n";
    return falhas == 0 ? 0 : 1;
}
