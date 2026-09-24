// FILA ENCADEADA (dinâmica)
//
// - FIFO: entra no FIM, sai do INÍCIO.
// - Guardamos dois ponteiros: `inicio` (quem sai) e `fim` (onde entra).
//
//     inicio                     fim
//       |                         |
//       v                         v
//     [10] --> [20] --> [30] --> [40] --> nullptr
//
// Complexidade: enfileirar, desenfileirar, frente e vazia são O(1).
//
// Pega-ratão
// - Quando a fila ESVAZIA, zere também o `fim` (senão ele aponta para um nó que
//   já foi liberado).
// - Na primeira inserção, o nó novo é `inicio` e `fim` ao mesmo tempo.
#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class FilaEncadeada {
    struct No {
        T valor;
        No* proximo;
    };

public:
    FilaEncadeada() = default;
    ~FilaEncadeada() { limpar(); }
    FilaEncadeada(const FilaEncadeada&) = delete;
    FilaEncadeada& operator=(const FilaEncadeada&) = delete;

    bool vazia() const { return inicio_ == nullptr; }
    int tamanho() const { return tamanho_; }

    void enfileirar(const T& valor) {
        No* novo = new No{valor, nullptr};
        if (vazia())
            inicio_ = novo;  // único elemento: é início e fim
        else
            fim_->proximo = novo;  // o antigo último aponta para o novo
        fim_ = novo;
        tamanho_++;
    }

    T desenfileirar() {
        if (vazia()) throw std::out_of_range("Fila vazia");
        No* no = inicio_;
        T valor = no->valor;
        inicio_ = no->proximo;
        if (inicio_ == nullptr) fim_ = nullptr;  // esvaziou: zera o fim também!
        delete no;
        tamanho_--;
        return valor;
    }

    const T& frente() const {
        if (vazia()) throw std::out_of_range("Fila vazia");
        return inicio_->valor;
    }

    const T& ultimo() const {
        if (vazia()) throw std::out_of_range("Fila vazia");
        return fim_->valor;
    }

    void limpar() {
        while (!vazia()) desenfileirar();
    }

    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) resultado.push_back(atual->valor);
        return resultado;
    }

private:
    No* inicio_ = nullptr;
    No* fim_ = nullptr;
    int tamanho_ = 0;
};
