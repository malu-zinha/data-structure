// FILA DE PRIORIDADE
//
// - Cada elemento entra com uma PRIORIDADE. Sai sempre o de maior prioridade,
//   não importa a ordem de chegada. Aqui: MENOR número = MAIOR prioridade
//   (como no pronto-socorro: prioridade 1 é atendida antes da 5).
// - Empate na prioridade: sai quem chegou primeiro (usamos um contador de chegada).
// - Implementada com HEAP MÍNIMO (veja `heap/heap.hpp`).
//
// | implementação       | inserir  | remover o prioritário |
// |---------------------|----------|-----------------------|
// | lista desordenada   | O(1)     | O(n)                  |
// | lista ordenada      | O(n)     | O(1)                  |
// | heap binário        | O(log n) | O(log n)              |  <- a usada aqui
//
// Usos: Dijkstra, Prim, escalonamento de processos, simulações de eventos.
#pragma once

#include <algorithm>
#include <stdexcept>
#include <utility>
#include <vector>

#include "../heap/heap.hpp"

template <typename T>
class FilaPrioridade {
    // O heap compara só (prioridade, chegada). Como a chegada nunca se repete,
    // o valor em si nunca precisa ser comparado (e nem precisa ter operador <).
    struct Entrada {
        int prioridade;
        long chegada;
        T valor;
        bool operator<(const Entrada& outra) const {
            if (prioridade != outra.prioridade) return prioridade < outra.prioridade;
            return chegada < outra.chegada;
        }
    };

public:
    bool vazia() const { return heap_.vazio(); }
    int tamanho() const { return heap_.tamanho(); }

    void enfileirar(const T& valor, int prioridade) { heap_.inserir(Entrada{prioridade, chegada_++, valor}); }

    // Remove e devolve o valor mais prioritário.
    T desenfileirar() {
        if (vazia()) throw std::out_of_range("Fila de prioridade vazia");
        return heap_.remover().valor;
    }

    // Devolve {valor, prioridade} do próximo a sair, sem remover.
    std::pair<T, int> frente() const {
        if (vazia()) throw std::out_of_range("Fila de prioridade vazia");
        const Entrada& topo = heap_.topo();
        return {topo.valor, topo.prioridade};
    }

    // {valor, prioridade} na ordem em que sairiam (só para exibir).
    std::vector<std::pair<T, int>> em_ordem() const {
        std::vector<Entrada> entradas = heap_.para_vetor();
        std::sort(entradas.begin(), entradas.end());
        std::vector<std::pair<T, int>> resultado;
        for (const Entrada& entrada : entradas) resultado.emplace_back(entrada.valor, entrada.prioridade);
        return resultado;
    }

private:
    HeapMinimo<Entrada> heap_;
    long chegada_ = 0;  // desempate: quem chegou antes sai antes
};
