// DEQUE (fila de duas pontas / Double-Ended Queue)
//
// - Permite inserir e remover nas DUAS pontas: início e fim.
// - Generaliza pilha e fila:
//     * usando só uma ponta                  -> vira PILHA;
//     * entrando por uma e saindo pela outra -> vira FILA.
// - Implementado com lista DUPLAMENTE encadeada, para remover do fim em O(1).
//
//     nullptr <- [10] <-> [20] <-> [30] -> nullptr
//                inicio            fim
//
// Complexidade: todas as operações nas pontas são O(1).
//
// Pega-ratão
// - Com lista simplesmente encadeada, remover do fim seria O(n) (precisa do
//   penúltimo). Por isso usamos o ponteiro `anterior`.
// - Na STL existe o `std::deque`, que resolve o mesmo problema com blocos de vetor.
#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class Deque {
    struct No {
        T valor;
        No* anterior;
        No* proximo;
    };

public:
    Deque() = default;
    ~Deque() { limpar(); }
    Deque(const Deque&) = delete;
    Deque& operator=(const Deque&) = delete;

    bool vazio() const { return tamanho_ == 0; }
    int tamanho() const { return tamanho_; }

    void inserir_inicio(const T& valor) {
        No* novo = new No{valor, nullptr, inicio_};
        if (vazio())
            fim_ = novo;
        else
            inicio_->anterior = novo;
        inicio_ = novo;
        tamanho_++;
    }

    void inserir_fim(const T& valor) {
        No* novo = new No{valor, fim_, nullptr};
        if (vazio())
            inicio_ = novo;
        else
            fim_->proximo = novo;
        fim_ = novo;
        tamanho_++;
    }

    T remover_inicio() {
        if (vazio()) throw std::out_of_range("Deque vazio");
        No* no = inicio_;
        inicio_ = no->proximo;
        if (inicio_ == nullptr)
            fim_ = nullptr;
        else
            inicio_->anterior = nullptr;
        return liberar(no);
    }

    T remover_fim() {
        if (vazio()) throw std::out_of_range("Deque vazio");
        No* no = fim_;
        fim_ = no->anterior;
        if (fim_ == nullptr)
            inicio_ = nullptr;
        else
            fim_->proximo = nullptr;
        return liberar(no);
    }

    const T& primeiro() const {
        if (vazio()) throw std::out_of_range("Deque vazio");
        return inicio_->valor;
    }

    const T& ultimo() const {
        if (vazio()) throw std::out_of_range("Deque vazio");
        return fim_->valor;
    }

    void limpar() {
        while (!vazio()) remover_inicio();
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

    T liberar(No* no) {
        T valor = no->valor;
        delete no;
        tamanho_--;
        return valor;
    }
};
