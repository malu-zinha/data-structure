// FILA SEQUENCIAL CIRCULAR (estática, com vetor)
//
// - FIFO: First In, First Out -> o PRIMEIRO a entrar é o PRIMEIRO a sair.
// - Entra pelo FIM, sai pelo INÍCIO. Pense na fila do banco.
// - Com vetor simples, cada saída deixaria um "buraco" no começo. A solução é o
//   VETOR CIRCULAR: os índices dão a volta usando o resto da divisão (%).
//
//   capacidade = 5, depois de algumas entradas e saídas:
//
//     índice:   0    1    2    3    4
//             [ 40 | 50 |    | 20 | 30 ]
//                         ^fim  ^inicio      ordem de saída: 20, 30, 40, 50
//
//   próximo índice = (índice + 1) % capacidade
//
// Complexidade: enfileirar, desenfileirar, frente, vazia e cheia são O(1).
//
// Pega-ratão
// - Só com `inicio` e `fim` não dá para diferenciar vazia de cheia (nos dois
//   casos inicio == fim). Por isso guardamos também `tamanho_`.
// - Esquecer o `% capacidade` faz o índice estourar o vetor.
#pragma once

#include <stdexcept>
#include <vector>

template <typename T>
class FilaSequencial {
public:
    explicit FilaSequencial(int capacidade = 10) : capacidade_(capacidade) {
        if (capacidade <= 0) throw std::invalid_argument("A capacidade deve ser positiva");
        dados_ = new T[capacidade];
    }
    ~FilaSequencial() { delete[] dados_; }
    FilaSequencial(const FilaSequencial&) = delete;
    FilaSequencial& operator=(const FilaSequencial&) = delete;

    bool vazia() const { return tamanho_ == 0; }
    bool cheia() const { return tamanho_ == capacidade_; }
    int tamanho() const { return tamanho_; }

    // enqueue: entra no fim.
    void enfileirar(const T& valor) {
        if (cheia()) throw std::overflow_error("Fila cheia");
        dados_[fim_] = valor;
        fim_ = (fim_ + 1) % capacidade_;  // dá a volta no vetor
        tamanho_++;
    }

    // dequeue: sai do início.
    T desenfileirar() {
        if (vazia()) throw std::out_of_range("Fila vazia");
        T valor = dados_[inicio_];
        inicio_ = (inicio_ + 1) % capacidade_;
        tamanho_--;
        return valor;
    }

    // Olha quem é o próximo a sair, sem remover.
    const T& frente() const {
        if (vazia()) throw std::out_of_range("Fila vazia");
        return dados_[inicio_];
    }

    const T& ultimo() const {
        if (vazia()) throw std::out_of_range("Fila vazia");
        return dados_[(fim_ - 1 + capacidade_) % capacidade_];  // + capacidade evita índice negativo
    }

    void limpar() { inicio_ = fim_ = tamanho_ = 0; }

    // Na ordem de saída.
    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        for (int i = 0; i < tamanho_; i++) resultado.push_back(dados_[(inicio_ + i) % capacidade_]);
        return resultado;
    }

    int indice_inicio() const { return inicio_; }
    int indice_fim() const { return fim_; }

private:
    T* dados_;
    int capacidade_;
    int inicio_ = 0;  // índice de quem sai primeiro
    int fim_ = 0;     // índice da próxima posição livre
    int tamanho_ = 0;
};
