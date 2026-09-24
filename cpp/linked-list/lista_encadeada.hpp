// LISTA SIMPLESMENTE ENCADEADA
//
// - É uma sequência de NÓS. Cada nó guarda um valor e o ponteiro para o
//   PRÓXIMO nó. O último nó aponta para nullptr.
// - A lista só precisa saber onde está o primeiro nó (`inicio`, a "cabeça").
//
//       inicio
//         |
//         v
//       [10|•]--> [20|•]--> [30|nullptr]
//
// | operação                          | custo |
// |-----------------------------------|-------|
// | inserir / remover no início       | O(1)  |
// | inserir / remover no fim          | O(n)  |  (precisa andar até o último)
// | inserir / remover em uma posição  | O(n)  |
// | obter / buscar                    | O(n)  |  (acesso é sequencial)
//
// Pega-ratão
// - Não existe `lista[i]` direto: para chegar na posição i é preciso andar i nós.
// - Para inserir/remover no meio, pare no nó ANTERIOR à posição.
// - Ao inverter, guarde o `proximo` ANTES de virar a seta, senão perde o resto.
// - Todo `new` precisa de um `delete`: remover um nó sem liberá-lo vaza memória.
#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class ListaEncadeada {
    struct No {
        T valor;
        No* proximo;
    };

public:
    ListaEncadeada() = default;
    ~ListaEncadeada() { limpar(); }
    ListaEncadeada(const ListaEncadeada&) = delete;
    ListaEncadeada& operator=(const ListaEncadeada&) = delete;

    // ---------------------------------------------------------------- consultas
    bool vazia() const { return inicio_ == nullptr; }
    int tamanho() const { return tamanho_; }
    const T& obter(int pos) const { return no_na_posicao(pos)->valor; }
    void alterar(int pos, const T& valor) { no_na_posicao(pos)->valor = valor; }

    // Devolve a posição da primeira ocorrência de `valor` ou -1.
    int buscar(const T& valor) const {
        int pos = 0;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo, pos++)
            if (atual->valor == valor) return pos;
        return -1;
    }

    bool contem(const T& valor) const { return buscar(valor) != -1; }

    // ---------------------------------------------------------------- inserção
    void inserir_inicio(const T& valor) {
        // O novo nó aponta para o antigo primeiro e vira a nova cabeça.
        inicio_ = new No{valor, inicio_};
        tamanho_++;
    }

    void inserir_fim(const T& valor) {
        No* novo = new No{valor, nullptr};
        if (vazia()) {
            inicio_ = novo;
        } else {
            No* atual = inicio_;
            while (atual->proximo != nullptr) atual = atual->proximo;  // anda até o último
            atual->proximo = novo;
        }
        tamanho_++;
    }

    // Insere na posição `pos` (0 = início, tamanho = fim).
    void inserir_posicao(int pos, const T& valor) {
        if (pos < 0 || pos > tamanho_) throw std::out_of_range("Posição inválida");
        if (pos == 0) {
            inserir_inicio(valor);
            return;
        }
        No* anterior = no_na_posicao(pos - 1);
        // o novo aponta para quem estava na posição, e o anterior aponta pro novo
        anterior->proximo = new No{valor, anterior->proximo};
        tamanho_++;
    }

    // ---------------------------------------------------------------- remoção
    T remover_inicio() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        No* removido = inicio_;
        inicio_ = removido->proximo;  // a cabeça passa a ser o segundo nó
        return liberar(removido);
    }

    T remover_fim() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        if (inicio_->proximo == nullptr) return remover_inicio();  // só um elemento
        No* anterior = inicio_;
        while (anterior->proximo->proximo != nullptr) anterior = anterior->proximo;  // para no PENÚLTIMO
        No* removido = anterior->proximo;
        anterior->proximo = nullptr;
        return liberar(removido);
    }

    T remover_posicao(int pos) {
        if (vazia()) throw std::out_of_range("Lista vazia");
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        if (pos == 0) return remover_inicio();
        No* anterior = no_na_posicao(pos - 1);
        No* removido = anterior->proximo;
        anterior->proximo = removido->proximo;  // "pula" o nó removido
        return liberar(removido);
    }

    // Remove a primeira ocorrência de `valor`. Devolve true se removeu.
    bool remover_valor(const T& valor) {
        No* anterior = nullptr;
        for (No* atual = inicio_; atual != nullptr; anterior = atual, atual = atual->proximo) {
            if (atual->valor == valor) {
                if (anterior == nullptr)  // era o primeiro
                    inicio_ = atual->proximo;
                else
                    anterior->proximo = atual->proximo;
                liberar(atual);
                return true;
            }
        }
        return false;
    }

    // ---------------------------------------------------------------- outras
    // Inverte a lista mudando só as setas (sem criar nós) -> O(n).
    void inverter() {
        No* anterior = nullptr;
        No* atual = inicio_;
        while (atual != nullptr) {
            No* proximo = atual->proximo;  // 1. guarda o resto da lista
            atual->proximo = anterior;     // 2. vira a seta para trás
            anterior = atual;              // 3. anda os dois ponteiros
            atual = proximo;
        }
        inicio_ = anterior;  // o antigo último vira a cabeça
    }

    // Libera nó por nó (em C seria um free para cada um).
    void limpar() {
        while (!vazia()) remover_inicio();
    }

    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) resultado.push_back(atual->valor);
        return resultado;
    }

private:
    No* inicio_ = nullptr;  // cabeça da lista (nullptr = lista vazia)
    int tamanho_ = 0;       // contador para tamanho() ser O(1)

    No* no_na_posicao(int pos) const {
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        No* atual = inicio_;
        for (int i = 0; i < pos; i++) atual = atual->proximo;
        return atual;
    }

    T liberar(No* no) {
        T valor = no->valor;
        delete no;
        tamanho_--;
        return valor;
    }
};
