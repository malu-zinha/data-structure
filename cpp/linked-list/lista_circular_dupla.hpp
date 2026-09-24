// LISTA CIRCULAR DUPLAMENTE ENCADEADA
//
// - Junta as duas ideias: cada nó tem `anterior` e `proximo`, e as pontas se
//   ligam: `fim->proximo` é o início e `inicio->anterior` é o fim.
// - Guardamos só o `inicio`; o fim é `inicio->anterior` (acesso O(1)).
//
//        +--------------------------------+
//        v                                |
//      [10] <-> [20] <-> [30] <-> [40] <--+   (e 10.anterior = 40)
//
// | operação                          | custo |
// |-----------------------------------|-------|
// | inserir / remover no início       | O(1)  |
// | inserir / remover no fim          | O(1)  |
// | inserir / remover em uma posição  | O(n)  |
// | buscar                            | O(n)  |
//
// Pega-ratão
// - Igual à circular simples: nunca pare no nullptr (ele não existe).
// - Com um único nó, `anterior` e `proximo` apontam para ele mesmo.
// - Ao remover o `inicio`, o novo início é o próximo dele.
#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class ListaCircularDupla {
    struct No {
        T valor;
        No* anterior;
        No* proximo;
    };

public:
    ListaCircularDupla() = default;
    ~ListaCircularDupla() { limpar(); }
    ListaCircularDupla(const ListaCircularDupla&) = delete;
    ListaCircularDupla& operator=(const ListaCircularDupla&) = delete;

    // ---------------------------------------------------------------- consultas
    bool vazia() const { return inicio_ == nullptr; }
    int tamanho() const { return tamanho_; }
    const T& obter(int pos) const { return no_na_posicao(pos)->valor; }
    void alterar(int pos, const T& valor) { no_na_posicao(pos)->valor = valor; }

    int buscar(const T& valor) const {
        No* atual = inicio_;
        for (int pos = 0; pos < tamanho_; pos++, atual = atual->proximo)
            if (atual->valor == valor) return pos;
        return -1;
    }

    bool contem(const T& valor) const { return buscar(valor) != -1; }

    // ---------------------------------------------------------------- inserção
    void inserir_inicio(const T& valor) {
        if (vazia())
            inserir_em_lista_vazia(valor);
        else
            inicio_ = inserir_antes(inicio_, valor);
    }

    void inserir_fim(const T& valor) {
        if (vazia())
            inserir_em_lista_vazia(valor);
        else
            inserir_antes(inicio_, valor);  // "antes do início" é "depois do fim" numa lista circular
    }

    void inserir_posicao(int pos, const T& valor) {
        if (pos < 0 || pos > tamanho_) throw std::out_of_range("Posição inválida");
        if (pos == 0)
            inserir_inicio(valor);
        else if (pos == tamanho_)
            inserir_fim(valor);
        else
            inserir_antes(no_na_posicao(pos), valor);
    }

    // ---------------------------------------------------------------- remoção
    T remover_inicio() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return desligar(inicio_);
    }

    T remover_fim() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return desligar(inicio_->anterior);
    }

    T remover_posicao(int pos) {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return desligar(no_na_posicao(pos));
    }

    bool remover_valor(const T& valor) {
        No* atual = inicio_;
        for (int i = 0; i < tamanho_; i++, atual = atual->proximo) {
            if (atual->valor == valor) {
                desligar(atual);
                return true;
            }
        }
        return false;
    }

    // ---------------------------------------------------------------- outras
    // Passos positivos giram para frente, negativos para trás.
    void rotacionar(int passos = 1) {
        if (vazia()) return;
        passos = ((passos % tamanho_) + tamanho_) % tamanho_;
        for (int i = 0; i < passos; i++) inicio_ = inicio_->proximo;
    }

    void inverter() {
        if (vazia()) return;
        No* antigo_fim = inicio_->anterior;
        No* atual = inicio_;
        for (int i = 0; i < tamanho_; i++) {
            std::swap(atual->anterior, atual->proximo);
            atual = atual->anterior;  // o "próximo" antigo
        }
        inicio_ = antigo_fim;
    }

    void limpar() {
        while (!vazia()) remover_inicio();
    }

    const T& primeiro() const {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return inicio_->valor;
    }

    const T& ultimo() const {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return inicio_->anterior->valor;  // o fim é o anterior do início
    }

    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        No* atual = inicio_;
        for (int i = 0; i < tamanho_; i++, atual = atual->proximo) resultado.push_back(atual->valor);
        return resultado;
    }

    std::vector<T> para_vetor_reverso() const {
        std::vector<T> resultado;
        if (vazia()) return resultado;
        No* atual = inicio_->anterior;
        for (int i = 0; i < tamanho_; i++, atual = atual->anterior) resultado.push_back(atual->valor);
        return resultado;
    }

private:
    No* inicio_ = nullptr;
    int tamanho_ = 0;

    // Liga um nó novo entre `referencia->anterior` e `referencia`.
    No* inserir_antes(No* referencia, const T& valor) {
        No* novo = new No{valor, referencia->anterior, referencia};
        referencia->anterior->proximo = novo;
        referencia->anterior = novo;
        tamanho_++;
        return novo;
    }

    void inserir_em_lista_vazia(const T& valor) {
        No* novo = new No{valor, nullptr, nullptr};
        novo->anterior = novo;  // sozinho: aponta para si mesmo nos dois sentidos
        novo->proximo = novo;
        inicio_ = novo;
        tamanho_ = 1;
    }

    T desligar(No* no) {
        if (tamanho_ == 1) {
            inicio_ = nullptr;
        } else {
            no->anterior->proximo = no->proximo;
            no->proximo->anterior = no->anterior;
            if (no == inicio_) inicio_ = no->proximo;
        }
        T valor = no->valor;
        delete no;
        tamanho_--;
        return valor;
    }

    No* no_na_posicao(int pos) const {
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        No* atual;
        if (pos <= tamanho_ / 2) {
            atual = inicio_;
            for (int i = 0; i < pos; i++) atual = atual->proximo;
        } else {  // mais perto do fim: anda para trás
            atual = inicio_->anterior;
            for (int i = 0; i < tamanho_ - 1 - pos; i++) atual = atual->anterior;
        }
        return atual;
    }
};
