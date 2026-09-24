// Testes das filas (FIFO), do deque, da fila de prioridade e da fila com duas pilhas.
#include <algorithm>
#include <deque>
#include <random>
#include <string>

#include "../queue/deque.hpp"
#include "../queue/fila_duas_pilhas.hpp"
#include "../queue/fila_encadeada.hpp"
#include "../queue/fila_prioridade.hpp"
#include "../queue/fila_sequencial.hpp"
#include "teste.hpp"

template <typename Fila>
void contrato_fila(Fila& fila, int limite, unsigned semente) {
    VERIFICAR(fila.vazia());
    VERIFICAR_LANCA(fila.desenfileirar(), std::out_of_range);
    VERIFICAR_LANCA(fila.frente(), std::out_of_range);

    std::mt19937 sorteio(semente);
    std::deque<int> modelo;
    for (int passo = 0; passo < 3000; passo++) {
        if (sorteio() % 2 == 0 && static_cast<int>(modelo.size()) < limite) {
            int valor = sorteio() % 100;
            fila.enfileirar(valor);
            modelo.push_back(valor);
        } else if (!modelo.empty()) {
            VERIFICAR_IGUAL(fila.desenfileirar(), modelo.front());
            modelo.pop_front();
        }
        VERIFICAR_IGUAL(fila.tamanho(), static_cast<int>(modelo.size()));
        if (!modelo.empty()) VERIFICAR_IGUAL(fila.frente(), modelo.front());
    }
}

TESTE(fila_sequencial_circular) {
    FilaSequencial<int> fila(7);  // capacidade pequena: os índices dão a volta muitas vezes
    contrato_fila(fila, 7, 1);
}

TESTE(fila_sequencial_cheia_e_ultimo) {
    FilaSequencial<int> fila(3);
    for (int valor : {1, 2, 3}) fila.enfileirar(valor);
    VERIFICAR(fila.cheia());
    VERIFICAR_LANCA(fila.enfileirar(4), std::overflow_error);
    fila.desenfileirar();
    fila.enfileirar(4);  // entra no índice 0 (deu a volta)
    VERIFICAR_IGUAL(fila.ultimo(), 4);
    VERIFICAR_IGUAL(fila.para_vetor(), (std::vector<int>{2, 3, 4}));
}

TESTE(fila_encadeada) {
    FilaEncadeada<int> fila;
    contrato_fila(fila, 1 << 30, 2);
}

TESTE(fila_encadeada_esvaziar_zera_o_fim) {
    FilaEncadeada<int> fila;
    fila.enfileirar(1);
    fila.desenfileirar();
    fila.enfileirar(2);  // se o fim apontasse para o nó liberado, o sanitizer acusaria
    VERIFICAR_IGUAL(fila.frente(), 2);
    VERIFICAR_IGUAL(fila.ultimo(), 2);
}

TESTE(fila_com_duas_pilhas) {
    FilaComDuasPilhas<int> fila;
    contrato_fila(fila, 1 << 30, 3);
}

TESTE(deque_contra_std_deque) {
    Deque<int> deque;
    VERIFICAR_LANCA(deque.remover_inicio(), std::out_of_range);
    VERIFICAR_LANCA(deque.remover_fim(), std::out_of_range);
    std::mt19937 sorteio(4);
    std::deque<int> modelo;
    for (int passo = 0; passo < 4000; passo++) {
        int operacao = sorteio() % 4;
        int valor = sorteio() % 100;
        if (operacao == 0) {
            deque.inserir_inicio(valor);
            modelo.push_front(valor);
        } else if (operacao == 1) {
            deque.inserir_fim(valor);
            modelo.push_back(valor);
        } else if (operacao == 2 && !modelo.empty()) {
            VERIFICAR_IGUAL(deque.remover_inicio(), modelo.front());
            modelo.pop_front();
        } else if (operacao == 3 && !modelo.empty()) {
            VERIFICAR_IGUAL(deque.remover_fim(), modelo.back());
            modelo.pop_back();
        }
        VERIFICAR_IGUAL(deque.para_vetor(), std::vector<int>(modelo.begin(), modelo.end()));
        if (!modelo.empty()) {
            VERIFICAR_IGUAL(deque.primeiro(), modelo.front());
            VERIFICAR_IGUAL(deque.ultimo(), modelo.back());
        }
    }
}

TESTE(fila_de_prioridade_menor_numero_sai_primeiro) {
    FilaPrioridade<std::string> fila;
    VERIFICAR_LANCA(fila.desenfileirar(), std::out_of_range);
    fila.enfileirar("c", 3);
    fila.enfileirar("a", 1);
    fila.enfileirar("b1", 2);
    fila.enfileirar("b2", 2);  // empate: sai depois do b1, que chegou antes
    VERIFICAR(fila.frente() == std::make_pair(std::string("a"), 1));
    std::vector<std::string> ordem;
    while (!fila.vazia()) ordem.push_back(fila.desenfileirar());
    VERIFICAR_IGUAL(ordem, (std::vector<std::string>{"a", "b1", "b2", "c"}));
}

TESTE(fila_de_prioridade_estavel_com_muitos_empates) {
    std::mt19937 sorteio(5);
    FilaPrioridade<int> fila;
    std::vector<std::pair<int, int>> esperado;  // (prioridade, chegada)
    for (int chegada = 0; chegada < 500; chegada++) {
        int prioridade = sorteio() % 5;
        fila.enfileirar(chegada, prioridade);
        esperado.emplace_back(prioridade, chegada);
    }
    std::sort(esperado.begin(), esperado.end());
    for (const auto& [prioridade, chegada] : esperado) VERIFICAR_IGUAL(fila.desenfileirar(), chegada);
}

int main() { return rodar_testes(); }
