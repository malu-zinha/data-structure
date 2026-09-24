// LISTA SEQUENCIAL ESTÁTICA (baseada em vetor)
//
// - Os elementos ficam em posições CONTÍGUAS de um vetor com CAPACIDADE fixa,
//   definida na criação.
// - Além do vetor, guardamos QUANTOS elementos estão ocupados (`tamanho_`).
// - Acesso por posição é direto: `dados_[i]`.
// - Para inserir/remover no início ou no meio é preciso DESLOCAR elementos.
//
// | operação                          | custo |
// |-----------------------------------|-------|
// | obter / alterar (por posição)     | O(1)  |
// | inserir / remover no fim          | O(1)  |
// | inserir / remover no início/meio  | O(n)  |
// | buscar (por valor)                | O(n)  |
//
// Pega-ratão
// - Checar se está CHEIA antes de inserir e VAZIA antes de remover.
// - Ao inserir, deslocar de TRÁS PARA FRENTE (senão sobrescreve os dados).
// - Ao remover, deslocar de FRENTE PARA TRÁS.
// Posições começam em 0.
#pragma once

#include <stdexcept>
#include <utility>
#include <vector>

template <typename T>
class ListaSequencial {
public:
    explicit ListaSequencial(int capacidade = 10) : capacidade_(capacidade) {
        if (capacidade <= 0) throw std::invalid_argument("A capacidade deve ser positiva");
        dados_ = new T[capacidade];
    }
    ~ListaSequencial() { delete[] dados_; }

    // O vetor é alocado com new[]: copiar o ponteiro faria duas listas
    // liberarem a mesma memória. Por isso a cópia fica proibida.
    ListaSequencial(const ListaSequencial&) = delete;
    ListaSequencial& operator=(const ListaSequencial&) = delete;

    // ---------------------------------------------------------------- consultas
    bool vazia() const { return tamanho_ == 0; }
    bool cheia() const { return tamanho_ == capacidade_; }
    int tamanho() const { return tamanho_; }
    int capacidade() const { return capacidade_; }

    const T& obter(int pos) const {
        validar_posicao(pos);
        return dados_[pos];
    }

    void alterar(int pos, const T& valor) {
        validar_posicao(pos);
        dados_[pos] = valor;
    }

    // Busca sequencial: devolve a posição do valor ou -1 se não achar.
    int buscar(const T& valor) const {
        for (int i = 0; i < tamanho_; i++)
            if (dados_[i] == valor) return i;
        return -1;
    }

    bool contem(const T& valor) const { return buscar(valor) != -1; }

    // ---------------------------------------------------------------- inserção
    void inserir_inicio(const T& valor) { inserir_posicao(0, valor); }
    void inserir_fim(const T& valor) { inserir_posicao(tamanho_, valor); }

    // Insere `valor` na posição `pos` (0 = início, tamanho = fim).
    void inserir_posicao(int pos, const T& valor) {
        if (cheia()) throw std::overflow_error("Lista cheia");
        if (pos < 0 || pos > tamanho_) throw std::out_of_range("Posição inválida");

        // Abre espaço em `pos` empurrando todo mundo uma casa para a direita.
        // Começa pelo FIM para não sobrescrever nenhum valor.
        for (int i = tamanho_; i > pos; i--) dados_[i] = dados_[i - 1];

        dados_[pos] = valor;
        tamanho_++;
    }

    // ---------------------------------------------------------------- remoção
    T remover_inicio() { return remover_posicao(0); }
    T remover_fim() { return remover_posicao(tamanho_ - 1); }

    T remover_posicao(int pos) {
        if (vazia()) throw std::out_of_range("Lista vazia");
        validar_posicao(pos);

        T valor = dados_[pos];
        // Fecha o "buraco" puxando todo mundo uma casa para a esquerda.
        for (int i = pos; i < tamanho_ - 1; i++) dados_[i] = dados_[i + 1];
        tamanho_--;
        return valor;
    }

    // Remove a primeira ocorrência de `valor`. Devolve true se removeu.
    bool remover_valor(const T& valor) {
        int pos = buscar(valor);
        if (pos == -1) return false;
        remover_posicao(pos);
        return true;
    }

    // ---------------------------------------------------------------- outras
    // Inverte a lista no próprio vetor, trocando as pontas -> O(n).
    void inverter() {
        for (int i = 0, j = tamanho_ - 1; i < j; i++, j--) std::swap(dados_[i], dados_[j]);
    }

    void limpar() { tamanho_ = 0; }

    std::vector<T> para_vetor() const { return std::vector<T>(dados_, dados_ + tamanho_); }

private:
    T* dados_;
    int capacidade_;
    int tamanho_ = 0;  // quantas posições estão ocupadas

    void validar_posicao(int pos) const {
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
    }
};
