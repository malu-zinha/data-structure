// PILHA SEQUENCIAL (estática, com vetor)
//
// - LIFO: Last In, First Out -> o ÚLTIMO a entrar é o PRIMEIRO a sair.
// - Tudo acontece no TOPO. Pense numa pilha de pratos.
// - Vetor de capacidade fixa e um índice `topo_`: `topo_ == -1` significa vazia.
//
//      |    |
//      | 30 |  <- topo (índice 2)
//      | 20 |
//      | 10 |
//      +----+
//
// Complexidade: empilhar, desempilhar, topo, vazia e cheia são todos O(1).
//
// Pega-ratão
// - Checar VAZIA antes de desempilhar (stack underflow).
// - Checar CHEIA antes de empilhar (stack overflow).
// - `topo()` só consulta, não remove.
#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class PilhaSequencial {
public:
    explicit PilhaSequencial(int capacidade = 10) : capacidade_(capacidade) {
        if (capacidade <= 0) throw std::invalid_argument("A capacidade deve ser positiva");
        dados_ = new T[capacidade];
    }
    ~PilhaSequencial() { delete[] dados_; }
    PilhaSequencial(const PilhaSequencial&) = delete;
    PilhaSequencial& operator=(const PilhaSequencial&) = delete;

    bool vazia() const { return topo_ == -1; }
    bool cheia() const { return topo_ == capacidade_ - 1; }
    int tamanho() const { return topo_ + 1; }

    // push: coloca no topo.
    void empilhar(const T& valor) {
        if (cheia()) throw std::overflow_error("Pilha cheia");
        dados_[++topo_] = valor;
    }

    // pop: tira e devolve o elemento do topo.
    T desempilhar() {
        if (vazia()) throw std::out_of_range("Pilha vazia");
        return dados_[topo_--];
    }

    // peek/top: só olha o topo, sem remover.
    const T& topo() const {
        if (vazia()) throw std::out_of_range("Pilha vazia");
        return dados_[topo_];
    }

    void limpar() { topo_ = -1; }

    // Da base até o topo.
    std::vector<T> para_vetor() const { return std::vector<T>(dados_, dados_ + tamanho()); }

private:
    T* dados_;
    int capacidade_;
    int topo_ = -1;  // índice do elemento do topo (-1 = vazia)
};
