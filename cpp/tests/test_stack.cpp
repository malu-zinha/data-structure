// Testes das pilhas e das aplicações de pilha.
#include <random>

#include "../stack/aplicacoes_pilha.hpp"
#include "../stack/pilha_encadeada.hpp"
#include "../stack/pilha_sequencial.hpp"
#include "teste.hpp"

template <typename Pilha>
void contrato_pilha(Pilha& pilha, int limite, unsigned semente) {
    VERIFICAR(pilha.vazia());
    VERIFICAR_LANCA(pilha.desempilhar(), std::out_of_range);
    VERIFICAR_LANCA(pilha.topo(), std::out_of_range);

    std::mt19937 sorteio(semente);
    std::vector<int> modelo;
    for (int passo = 0; passo < 3000; passo++) {
        if (sorteio() % 2 == 0 && static_cast<int>(modelo.size()) < limite) {
            int valor = sorteio() % 100;
            pilha.empilhar(valor);
            modelo.push_back(valor);
        } else if (!modelo.empty()) {
            VERIFICAR_IGUAL(pilha.desempilhar(), modelo.back());
            modelo.pop_back();
        }
        VERIFICAR_IGUAL(pilha.tamanho(), static_cast<int>(modelo.size()));
        VERIFICAR_IGUAL(pilha.para_vetor(), modelo);
        if (!modelo.empty()) VERIFICAR_IGUAL(pilha.topo(), modelo.back());
    }
}

TESTE(pilha_sequencial) {
    PilhaSequencial<int> pilha(50);
    contrato_pilha(pilha, 50, 1);
}

TESTE(pilha_sequencial_cheia) {
    PilhaSequencial<int> pilha(2);
    pilha.empilhar(1);
    pilha.empilhar(2);
    VERIFICAR(pilha.cheia());
    VERIFICAR_LANCA(pilha.empilhar(3), std::overflow_error);
}

TESTE(pilha_encadeada) {
    PilhaEncadeada<int> pilha;
    contrato_pilha(pilha, 1 << 30, 2);
}

TESTE(parenteses_balanceados_) {
    VERIFICAR(parenteses_balanceados("([]{})"));
    VERIFICAR(parenteses_balanceados(""));
    VERIFICAR(parenteses_balanceados("a + (b * [c - d])"));
    VERIFICAR(!parenteses_balanceados("([)]"));
    VERIFICAR(!parenteses_balanceados("((a + b) * c"));
    VERIFICAR(!parenteses_balanceados(")("));
}

TESTE(infixa_para_posfixa_) {
    VERIFICAR_IGUAL(infixa_para_posfixa("A + B * C"), std::string("A B C * +"));
    VERIFICAR_IGUAL(infixa_para_posfixa("(A + B) * C"), std::string("A B + C *"));
    VERIFICAR_IGUAL(infixa_para_posfixa("A - B - C"), std::string("A B - C -"));  // associativo à esquerda
    VERIFICAR_IGUAL(infixa_para_posfixa("2 ^ 3 ^ 2"), std::string("2 3 2 ^ ^"));  // associativo à direita
    VERIFICAR_IGUAL(infixa_para_posfixa("3 + 4 * 2 / (1 - 5) ^ 2 ^ 3"), std::string("3 4 2 * 1 5 - 2 3 ^ ^ / +"));
    VERIFICAR_LANCA(infixa_para_posfixa("(A + B"), std::invalid_argument);
    VERIFICAR_LANCA(infixa_para_posfixa("A + B)"), std::invalid_argument);
    VERIFICAR_LANCA(infixa_para_posfixa("A $ B"), std::invalid_argument);
}

TESTE(avaliar_expressoes) {
    VERIFICAR_IGUAL(avaliar_posfixa("3 4 2 * +"), 11.0);
    VERIFICAR_IGUAL(avaliar_posfixa("5 2 -"), 3.0);  // o primeiro desempilhado é o da DIREITA
    VERIFICAR_IGUAL(avaliar_posfixa("5 1 2 + 4 * + 3 -"), 14.0);
    VERIFICAR_IGUAL(avaliar_infixa("3 + 4 * 2"), 11.0);
    VERIFICAR_IGUAL(avaliar_infixa("2 ^ 3 ^ 2"), 512.0);
    VERIFICAR_IGUAL(avaliar_infixa("(2 + 3) * x", {{"x", 4}}), 20.0);
    VERIFICAR_IGUAL(avaliar_infixa("7 / 2"), 3.5);
    VERIFICAR_IGUAL(avaliar_infixa("1.5 * 4"), 6.0);
    VERIFICAR_LANCA(avaliar_posfixa("1 +"), std::invalid_argument);
    VERIFICAR_LANCA(avaliar_posfixa("1 2"), std::invalid_argument);
    VERIFICAR_LANCA(avaliar_infixa("y + 1"), std::invalid_argument);
}

TESTE(inverter_e_palindromo) {
    VERIFICAR_IGUAL(inverter_texto("estrutura"), std::string("aruturtse"));
    VERIFICAR_IGUAL(inverter_texto(""), std::string(""));
    VERIFICAR(eh_palindromo("Ame a ema"));
    VERIFICAR(eh_palindromo("Socorram-me, subi no ônibus em Marrocos"));
    VERIFICAR(eh_palindromo("A cara rajada da jararaca"));
    VERIFICAR(!eh_palindromo("estrutura"));
}

TESTE(converter_base_) {
    VERIFICAR_IGUAL(converter_base(10, 2), std::string("1010"));
    VERIFICAR_IGUAL(converter_base(255, 16), std::string("FF"));
    VERIFICAR_IGUAL(converter_base(0, 8), std::string("0"));
    VERIFICAR_IGUAL(converter_base(-5, 2), std::string("-101"));
    VERIFICAR_LANCA(converter_base(10, 1), std::invalid_argument);
    VERIFICAR_LANCA(converter_base(10, 17), std::invalid_argument);
}

TESTE(torre_de_hanoi_) {
    for (int n = 0; n <= 8; n++) VERIFICAR_IGUAL(static_cast<int>(torre_de_hanoi(n).size()), (1 << n) - 1);
    auto movimentos = torre_de_hanoi(2);
    VERIFICAR(movimentos[0] == Movimento(1, 'A', 'B'));
    VERIFICAR(movimentos[1] == Movimento(2, 'A', 'C'));
    VERIFICAR(movimentos[2] == Movimento(1, 'B', 'C'));
}

TESTE(proximo_maior_elemento_) {
    auto resposta = proximo_maior_elemento({4, 5, 2, 25});
    VERIFICAR(resposta[0] == 5 && resposta[1] == 25 && resposta[2] == 25 && !resposta[3]);

    std::mt19937 sorteio(3);
    for (int rodada = 0; rodada < 100; rodada++) {  // confere contra a força bruta O(n²)
        std::vector<int> valores(sorteio() % 20);
        for (int& valor : valores) valor = sorteio() % 10;
        auto obtido = proximo_maior_elemento(valores);
        for (std::size_t i = 0; i < valores.size(); i++) {
            std::optional<int> esperado;
            for (std::size_t j = i + 1; j < valores.size() && !esperado; j++)
                if (valores[j] > valores[i]) esperado = valores[j];
            VERIFICAR(obtido[i] == esperado);
        }
    }
}

int main() { return rodar_testes(); }
