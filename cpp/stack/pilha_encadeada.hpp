// PILHA ENCADEADA (dinâmica)
//
// - LIFO: o último a entrar é o primeiro a sair.
// - Cada nó aponta para o nó de BAIXO. A pilha só guarda o nó do `topo_`.
// - Não tem limite de capacidade (cresce enquanto houver memória).
//
//     topo_ -> [30] -> [20] -> [10] -> nullptr
//                               (base)
//
// Empilhar = inserir no início da lista encadeada.
// Desempilhar = remover do início da lista encadeada.
// Complexidade: empilhar, desempilhar, topo e vazia são O(1).
//
// Pega-ratão
// - Checar se está vazia antes de desempilhar.
// - Não existe "cheia" (a não ser que a memória acabe).
#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

template <typename T>
class PilhaEncadeada {
    struct No {
        T valor;
        No* abaixo;  // o nó que está embaixo deste na pilha
    };

public:
    PilhaEncadeada() = default;
    ~PilhaEncadeada() { limpar(); }
    PilhaEncadeada(const PilhaEncadeada&) = delete;
    PilhaEncadeada& operator=(const PilhaEncadeada&) = delete;

    bool vazia() const { return topo_ == nullptr; }
    int tamanho() const { return tamanho_; }

    void empilhar(const T& valor) {
        // o novo nó fica por cima e aponta para o antigo topo
        topo_ = new No{valor, topo_};
        tamanho_++;
    }

    T desempilhar() {
        if (vazia()) throw std::out_of_range("Pilha vazia");
        No* no = topo_;
        T valor = no->valor;
        topo_ = no->abaixo;  // quem estava embaixo vira o topo
        delete no;
        tamanho_--;
        return valor;
    }

    const T& topo() const {
        if (vazia()) throw std::out_of_range("Pilha vazia");
        return topo_->valor;
    }

    void limpar() {
        while (!vazia()) desempilhar();
    }

    // Da base até o topo (mesma ordem da PilhaSequencial).
    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        for (No* atual = topo_; atual != nullptr; atual = atual->abaixo) resultado.push_back(atual->valor);
        std::reverse(resultado.begin(), resultado.end());
        return resultado;
    }

private:
    No* topo_ = nullptr;
    int tamanho_ = 0;
};
