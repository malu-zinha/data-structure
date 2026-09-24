// LISTA DUPLAMENTE ENCADEADA
//
// - Cada nó tem DOIS ponteiros: `anterior` e `proximo`.
// - A lista guarda o primeiro (`inicio`) e o último (`fim`) nó.
// - Dá para percorrer nos dois sentidos e remover um nó conhecido em O(1),
//   porque ele já sabe quem é o seu anterior.
//
//     nullptr <- [10] <-> [20] <-> [30] -> nullptr
//                inicio            fim
//
// | operação                          | custo |
// |-----------------------------------|-------|
// | inserir / remover no início       | O(1)  |
// | inserir / remover no fim          | O(1)  |  (graças ao ponteiro `fim`)
// | inserir / remover em uma posição  | O(n)  |  (anda pelo lado mais perto)
// | buscar                            | O(n)  |
//
// Pega-ratão
// - Toda ligação tem DOIS lados: se mexeu no `proximo` de um nó, lembre do
//   `anterior` do outro.
// - Casos especiais: lista vazia, remover o primeiro (atualiza `inicio`) e
//   remover o último (atualiza `fim`).
#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class ListaDuplamenteEncadeada {
    struct No {
        T valor;
        No* anterior;
        No* proximo;
    };

public:
    ListaDuplamenteEncadeada() = default;
    ~ListaDuplamenteEncadeada() { limpar(); }
    ListaDuplamenteEncadeada(const ListaDuplamenteEncadeada&) = delete;
    ListaDuplamenteEncadeada& operator=(const ListaDuplamenteEncadeada&) = delete;

    // ---------------------------------------------------------------- consultas
    bool vazia() const { return inicio_ == nullptr; }
    int tamanho() const { return tamanho_; }
    const T& obter(int pos) const { return no_na_posicao(pos)->valor; }
    void alterar(int pos, const T& valor) { no_na_posicao(pos)->valor = valor; }

    int buscar(const T& valor) const {
        int pos = 0;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo, pos++)
            if (atual->valor == valor) return pos;
        return -1;
    }

    bool contem(const T& valor) const { return buscar(valor) != -1; }

    // ---------------------------------------------------------------- inserção
    void inserir_inicio(const T& valor) {
        No* novo = new No{valor, nullptr, inicio_};
        if (vazia())
            fim_ = novo;  // único nó: é início e fim ao mesmo tempo
        else
            inicio_->anterior = novo;  // antigo primeiro aponta de volta pro novo
        inicio_ = novo;
        tamanho_++;
    }

    void inserir_fim(const T& valor) {
        No* novo = new No{valor, fim_, nullptr};
        if (vazia())
            inicio_ = novo;
        else
            fim_->proximo = novo;
        fim_ = novo;
        tamanho_++;
    }

    // Insere na posição `pos` (0 = início, tamanho = fim).
    void inserir_posicao(int pos, const T& valor) {
        if (pos < 0 || pos > tamanho_) throw std::out_of_range("Posição inválida");
        if (pos == 0) {
            inserir_inicio(valor);
        } else if (pos == tamanho_) {
            inserir_fim(valor);
        } else {
            No* atual = no_na_posicao(pos);  // o novo entra ANTES deste nó
            No* novo = new No{valor, atual->anterior, atual};
            atual->anterior->proximo = novo;
            atual->anterior = novo;
            tamanho_++;
        }
    }

    // ---------------------------------------------------------------- remoção
    T remover_inicio() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return desligar(inicio_);
    }

    T remover_fim() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return desligar(fim_);
    }

    T remover_posicao(int pos) {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return desligar(no_na_posicao(pos));
    }

    bool remover_valor(const T& valor) {
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) {
            if (atual->valor == valor) {
                desligar(atual);
                return true;
            }
        }
        return false;
    }

    // ---------------------------------------------------------------- outras
    // Troca `anterior` e `proximo` de cada nó e depois troca início/fim.
    void inverter() {
        No* atual = inicio_;
        while (atual != nullptr) {
            std::swap(atual->anterior, atual->proximo);
            atual = atual->anterior;  // depois da troca, o "próximo" antigo está em `anterior`
        }
        std::swap(inicio_, fim_);
    }

    void limpar() {
        while (!vazia()) remover_inicio();
    }

    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) resultado.push_back(atual->valor);
        return resultado;
    }

    // Percorre do fim para o início usando os ponteiros `anterior`.
    std::vector<T> para_vetor_reverso() const {
        std::vector<T> resultado;
        for (No* atual = fim_; atual != nullptr; atual = atual->anterior) resultado.push_back(atual->valor);
        return resultado;
    }

private:
    No* inicio_ = nullptr;
    No* fim_ = nullptr;
    int tamanho_ = 0;

    // Tira `no` da lista em O(1): o anterior e o próximo se ligam direto.
    T desligar(No* no) {
        if (no->anterior == nullptr)  // era o primeiro
            inicio_ = no->proximo;
        else
            no->anterior->proximo = no->proximo;

        if (no->proximo == nullptr)  // era o último
            fim_ = no->anterior;
        else
            no->proximo->anterior = no->anterior;

        T valor = no->valor;
        delete no;
        tamanho_--;
        return valor;
    }

    // Anda a partir da ponta mais próxima da posição (no máximo n/2 passos).
    No* no_na_posicao(int pos) const {
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        No* atual;
        if (pos < tamanho_ / 2) {
            atual = inicio_;
            for (int i = 0; i < pos; i++) atual = atual->proximo;
        } else {
            atual = fim_;
            for (int i = 0; i < tamanho_ - 1 - pos; i++) atual = atual->anterior;
        }
        return atual;
    }
};
