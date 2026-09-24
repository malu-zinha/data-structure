#include <iostream>

#include "../util.hpp"
#include "aplicacoes_pilha.hpp"
#include "pilha_encadeada.hpp"
#include "pilha_sequencial.hpp"

int main() {
    titulo("Pilha sequencial");
    PilhaSequencial<int> pilha(3);
    for (int valor : {10, 20, 30}) {
        pilha.empilhar(valor);
        std::cout << "empilhou " << valor << ": " << em_colchetes(pilha.para_vetor()) << " <- topo\n";
    }
    std::cout << "Cheia? " << (pilha.cheia() ? "sim" : "não") << " | topo: " << pilha.topo() << "\n";
    while (!pilha.vazia()) {
        int valor = pilha.desempilhar();
        std::cout << "desempilhou " << valor << ": " << em_colchetes(pilha.para_vetor()) << " <- topo\n";
    }
    try {
        pilha.desempilhar();
    } catch (const std::out_of_range& erro) {
        std::cout << "Erro esperado: " << erro.what() << "\n";
    }

    titulo("Pilha encadeada");
    PilhaEncadeada<std::string> letras;
    for (const char* letra : {"a", "b", "c"}) letras.empilhar(letra);
    std::cout << "Pilha: " << em_colchetes(letras.para_vetor()) << " <- topo | topo: " << letras.topo() << "\n";
    std::string saiu = letras.desempilhar();
    std::cout << "Desempilhou: " << saiu << " | pilha: " << em_colchetes(letras.para_vetor()) << " <- topo\n";

    titulo("Aplicações");
    for (const char* expressao : {"([]{})", "([)]", "((a + b) * c"})
        std::cout << "balanceada(\"" << expressao << "\") = " << (parenteses_balanceados(expressao) ? "sim" : "não")
                  << "\n";
    for (const char* expressao : {"A + B * C", "(A + B) * C", "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3"})
        std::cout << "infixa: " << expressao << "  ->  pós-fixa: " << infixa_para_posfixa(expressao) << "\n";
    std::cout << "avaliar_posfixa(\"5 1 2 + 4 * + 3 -\") = " << avaliar_posfixa("5 1 2 + 4 * + 3 -") << "\n";
    std::cout << "avaliar_infixa(\"(2 + 3) * x\", x = 4) = " << avaliar_infixa("(2 + 3) * x", {{"x", 4}}) << "\n";
    std::cout << "inverter_texto(\"estrutura\") = " << inverter_texto("estrutura") << "\n";
    std::cout << "eh_palindromo(\"Socorram-me, subi no ônibus em Marrocos\") = "
              << (eh_palindromo("Socorram-me, subi no ônibus em Marrocos") ? "sim" : "não") << "\n";
    std::cout << "converter_base(10, 2) = " << converter_base(10, 2)
              << " | converter_base(255, 16) = " << converter_base(255, 16) << "\n";

    auto movimentos = torre_de_hanoi(3);
    std::cout << "Hanói com 3 discos: " << movimentos.size() << " movimentos\n";
    for (auto [disco, origem, destino] : movimentos)
        std::cout << "  disco " << disco << ": " << origem << " -> " << destino << "\n";

    std::cout << "proximo_maior_elemento({4, 5, 2, 25}) = [";
    auto resposta = proximo_maior_elemento({4, 5, 2, 25});
    for (std::size_t i = 0; i < resposta.size(); i++)
        std::cout << (i > 0 ? ", " : "") << (resposta[i] ? std::to_string(*resposta[i]) : "nenhum");
    std::cout << "]\n";
}
