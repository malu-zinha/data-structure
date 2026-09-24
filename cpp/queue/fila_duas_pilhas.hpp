// FILA COM DUAS PILHAS (exercício clássico)
//
// Como fazer uma fila (FIFO) usando só pilhas (LIFO)?
// - Pilha de ENTRADA: todo `enfileirar` empilha aqui.
// - Pilha de SAÍDA: todo `desenfileirar` desempilha daqui.
// - Se a saída estiver vazia, despeja TODA a entrada nela. Ao passar de uma pilha
//   para a outra a ordem inverte, e o mais antigo fica no topo.
//
//   enfileirar 1, 2, 3:   entrada = [1, 2, 3]   saída = []
//   desenfileirar:        entrada = []          saída = [3, 2, 1] -> sai o 1
//
// Complexidade: enfileirar O(1); desenfileirar O(1) amortizado (cada elemento
// é transferido no máximo uma vez).
#pragma once

#include <stdexcept>

#include "../stack/pilha_encadeada.hpp"

template <typename T>
class FilaComDuasPilhas {
public:
    bool vazia() const { return entrada_.vazia() && saida_.vazia(); }
    int tamanho() const { return entrada_.tamanho() + saida_.tamanho(); }

    void enfileirar(const T& valor) { entrada_.empilhar(valor); }

    T desenfileirar() {
        if (vazia()) throw std::out_of_range("Fila vazia");
        transferir_se_preciso();
        return saida_.desempilhar();
    }

    const T& frente() {
        if (vazia()) throw std::out_of_range("Fila vazia");
        transferir_se_preciso();
        return saida_.topo();
    }

private:
    PilhaEncadeada<T> entrada_;
    PilhaEncadeada<T> saida_;

    // Só transfere quando a saída acaba. Transferir antes disso
    // colocaria elementos novos na frente dos antigos.
    void transferir_se_preciso() {
        if (saida_.vazia())
            while (!entrada_.vazia()) saida_.empilhar(entrada_.desempilhar());
    }
};
