// LISTA CIRCULAR (simplesmente encadeada)
//
// - Igual à lista encadeada, mas o ÚLTIMO nó aponta de volta para o PRIMEIRO.
//   Não existe nullptr no fim.
// - Guardamos só o ponteiro para o ÚLTIMO nó (`fim`). O primeiro é `fim->proximo`.
//   Assim inserir no início E no fim ficam O(1).
//
//         +-------------------------------+
//         v                               |
//       [10] --> [20] --> [30] --> [40] --+
//                                  fim
//
// | operação                          | custo |
// |-----------------------------------|-------|
// | inserir no início / no fim        | O(1)  |
// | remover no início                 | O(1)  |
// | remover no fim                    | O(n)  |  (precisa achar o penúltimo)
// | buscar / obter                    | O(n)  |
//
// Pega-ratão
// - NUNCA percorra com `while (atual != nullptr)`: não há nullptr, vira loop
//   infinito! Pare quando voltar ao primeiro nó (ou conte `tamanho` passos).
// - Com um único nó, ele aponta para ELE MESMO.
// - Aplicação clássica: problema de Josephus, escalonamento round-robin.
#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class ListaCircular {
    struct No {
        T valor;
        No* proximo;
    };

public:
    ListaCircular() = default;
    ~ListaCircular() { limpar(); }
    ListaCircular(const ListaCircular&) = delete;
    ListaCircular& operator=(const ListaCircular&) = delete;

    // ---------------------------------------------------------------- consultas
    bool vazia() const { return fim_ == nullptr; }
    int tamanho() const { return tamanho_; }
    const T& obter(int pos) const { return no_na_posicao(pos)->valor; }
    void alterar(int pos, const T& valor) { no_na_posicao(pos)->valor = valor; }

    int buscar(const T& valor) const {
        No* atual = vazia() ? nullptr : fim_->proximo;
        for (int pos = 0; pos < tamanho_; pos++, atual = atual->proximo)
            if (atual->valor == valor) return pos;
        return -1;
    }

    bool contem(const T& valor) const { return buscar(valor) != -1; }

    // ---------------------------------------------------------------- inserção
    void inserir_inicio(const T& valor) {
        No* novo = new No{valor, nullptr};
        if (vazia()) {
            novo->proximo = novo;  // sozinho: aponta para si mesmo
            fim_ = novo;
        } else {
            novo->proximo = fim_->proximo;  // novo aponta para o antigo primeiro
            fim_->proximo = novo;           // o último passa a apontar para o novo
        }
        tamanho_++;
    }

    void inserir_fim(const T& valor) {
        // Truque: insere no início e "anda" o fim uma casa.
        // O nó novo, que estava logo depois do fim, vira o próprio fim.
        inserir_inicio(valor);
        fim_ = fim_->proximo;
    }

    void inserir_posicao(int pos, const T& valor) {
        if (pos < 0 || pos > tamanho_) throw std::out_of_range("Posição inválida");
        if (pos == 0) {
            inserir_inicio(valor);
        } else if (pos == tamanho_) {
            inserir_fim(valor);
        } else {
            No* anterior = no_na_posicao(pos - 1);
            anterior->proximo = new No{valor, anterior->proximo};
            tamanho_++;
        }
    }

    // ---------------------------------------------------------------- remoção
    T remover_inicio() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        No* primeiro = fim_->proximo;
        if (primeiro == fim_)  // era o único nó
            fim_ = nullptr;
        else
            fim_->proximo = primeiro->proximo;  // o último pula o antigo primeiro
        return liberar(primeiro);
    }

    T remover_fim() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        if (tamanho_ == 1) return remover_inicio();
        No* penultimo = no_na_posicao(tamanho_ - 2);
        No* removido = fim_;
        penultimo->proximo = fim_->proximo;  // penúltimo aponta para o primeiro
        fim_ = penultimo;
        return liberar(removido);
    }

    T remover_posicao(int pos) {
        if (vazia()) throw std::out_of_range("Lista vazia");
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        if (pos == 0) return remover_inicio();
        if (pos == tamanho_ - 1) return remover_fim();
        No* anterior = no_na_posicao(pos - 1);
        No* removido = anterior->proximo;
        anterior->proximo = removido->proximo;
        return liberar(removido);
    }

    bool remover_valor(const T& valor) {
        int pos = buscar(valor);
        if (pos == -1) return false;
        remover_posicao(pos);
        return true;
    }

    // ---------------------------------------------------------------- outras
    // Gira a lista: o início anda `passos` casas para frente -> O(passos).
    //   [1, 2, 3, 4] rotacionar(1) -> [2, 3, 4, 1]
    // Na lista circular basta mover o ponteiro `fim`; nenhum nó muda.
    void rotacionar(int passos = 1) {
        if (vazia()) return;
        passos = ((passos % tamanho_) + tamanho_) % tamanho_;  // aceita negativo
        for (int i = 0; i < passos; i++) fim_ = fim_->proximo;
    }

    void inverter() {
        if (tamanho_ < 2) return;
        No* primeiro = fim_->proximo;
        No* anterior = fim_;
        No* atual = primeiro;
        for (int i = 0; i < tamanho_; i++) {
            No* proximo = atual->proximo;
            atual->proximo = anterior;
            anterior = atual;
            atual = proximo;
        }
        fim_ = primeiro;  // o antigo primeiro vira o último
    }

    void limpar() {
        while (!vazia()) remover_inicio();
    }

    // Estilo "faça-enquanto": visita e para quando VOLTAR ao primeiro.
    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        if (vazia()) return resultado;
        No* primeiro = fim_->proximo;
        No* atual = primeiro;
        do {
            resultado.push_back(atual->valor);
            atual = atual->proximo;
        } while (atual != primeiro);
        return resultado;
    }

private:
    No* fim_ = nullptr;  // último nó; o primeiro é fim_->proximo
    int tamanho_ = 0;

    No* no_na_posicao(int pos) const {
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        No* atual = fim_->proximo;
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

// n pessoas em roda (1..n); a cada k contadas, a k-ésima sai.
// Devolve {ordem_de_eliminacao, sobrevivente}.
// Ex.: n=7, k=3 -> ({3, 6, 2, 7, 5, 1}, 4)
inline std::pair<std::vector<int>, int> problema_de_josephus(int n, int k) {
    if (n < 1 || k < 1) throw std::invalid_argument("n e k devem ser positivos");
    ListaCircular<int> roda;
    for (int pessoa = 1; pessoa <= n; pessoa++) roda.inserir_fim(pessoa);

    std::vector<int> eliminados;
    while (roda.tamanho() > 1) {
        roda.rotacionar(k - 1);                     // k-1 pessoas passam a vez
        eliminados.push_back(roda.remover_inicio());  // a k-ésima sai da roda
    }
    return {eliminados, roda.obter(0)};
}
