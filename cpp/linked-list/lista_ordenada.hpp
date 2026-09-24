// LISTA ENCADEADA ORDENADA
//
// - Lista encadeada que SEMPRE mantém os valores em ordem crescente.
// - Não existe "inserir no início/fim": o usuário só chama `inserir(valor)` e a
//   lista descobre sozinha a posição certa.
// - Como está ordenada, a busca pode PARAR CEDO: se achou um valor maior do que
//   o procurado, ele não está na lista.
//
// | operação                  | custo    |
// |---------------------------|----------|
// | inserir                   | O(n)     |
// | remover / buscar          | O(n)     |  (mas para cedo em média)
// | mínimo                    | O(1)     |
// | máximo                    | O(n)     |
// | mesclar duas listas       | O(n + m) |
//
// Pega-ratão
// - Inserir antes do primeiro (lista vazia ou valor menor que o início) é um
//   caso especial: muda a cabeça.
// - Na ordenada não dá para usar busca binária: não existe acesso direto ao meio.
#pragma once

#include <initializer_list>
#include <stdexcept>
#include <vector>

template <typename T>
class ListaOrdenada {
    struct No {
        T valor;
        No* proximo;
    };

public:
    ListaOrdenada() = default;
    ListaOrdenada(std::initializer_list<T> valores) {
        for (const T& valor : valores) inserir(valor);
    }
    ~ListaOrdenada() { limpar(); }
    ListaOrdenada(const ListaOrdenada&) = delete;
    ListaOrdenada& operator=(const ListaOrdenada&) = delete;

    // Mover é permitido: os nós passam para a nova lista e a antiga fica vazia.
    // É isso que deixa `mesclar` devolver uma lista por valor.
    ListaOrdenada(ListaOrdenada&& outra) noexcept : inicio_(outra.inicio_), tamanho_(outra.tamanho_) {
        outra.inicio_ = nullptr;
        outra.tamanho_ = 0;
    }

    bool vazia() const { return inicio_ == nullptr; }
    int tamanho() const { return tamanho_; }

    // Insere mantendo a ordem. Repetidos entram depois dos iguais.
    void inserir(const T& valor) {
        if (inicio_ == nullptr || valor < inicio_->valor) {
            inicio_ = new No{valor, inicio_};  // vira a nova cabeça
        } else {
            No* atual = inicio_;
            // anda enquanto o PRÓXIMO ainda for menor ou igual ao valor
            while (atual->proximo != nullptr && !(valor < atual->proximo->valor)) atual = atual->proximo;
            atual->proximo = new No{valor, atual->proximo};
        }
        tamanho_++;
    }

    // Remove uma ocorrência de `valor`. Devolve true se removeu.
    bool remover(const T& valor) {
        No* anterior = nullptr;
        No* atual = inicio_;
        while (atual != nullptr && atual->valor < valor) {
            anterior = atual;
            atual = atual->proximo;
        }
        if (atual == nullptr || atual->valor != valor) return false;  // passou do ponto: não existe
        if (anterior == nullptr)
            inicio_ = atual->proximo;
        else
            anterior->proximo = atual->proximo;
        delete atual;
        tamanho_--;
        return true;
    }

    // Remove e devolve o MENOR valor.
    T remover_inicio() {
        if (vazia()) throw std::out_of_range("Lista vazia");
        No* removido = inicio_;
        T valor = removido->valor;
        inicio_ = removido->proximo;
        delete removido;
        tamanho_--;
        return valor;
    }

    // Devolve a posição de `valor` ou -1 (para cedo ao passar do valor).
    int buscar(const T& valor) const {
        int pos = 0;
        for (No* atual = inicio_; atual != nullptr && !(valor < atual->valor); atual = atual->proximo, pos++)
            if (atual->valor == valor) return pos;
        return -1;
    }

    bool contem(const T& valor) const { return buscar(valor) != -1; }

    const T& obter(int pos) const {
        if (pos < 0 || pos >= tamanho_) throw std::out_of_range("Posição inválida");
        No* atual = inicio_;
        for (int i = 0; i < pos; i++) atual = atual->proximo;
        return atual->valor;
    }

    const T& minimo() const {
        if (vazia()) throw std::out_of_range("Lista vazia");
        return inicio_->valor;
    }

    const T& maximo() const {
        if (vazia()) throw std::out_of_range("Lista vazia");
        No* atual = inicio_;
        while (atual->proximo != nullptr) atual = atual->proximo;
        return atual->valor;
    }

    // Junta duas listas ordenadas em uma NOVA lista ordenada -> O(n + m).
    // É o mesmo passo de "intercalação" usado no Merge Sort.
    ListaOrdenada mesclar(const ListaOrdenada& outra) const {
        ListaOrdenada resultado;
        No* a = inicio_;
        No* b = outra.inicio_;
        No* cauda = nullptr;  // último nó do resultado, para inserir no fim em O(1)
        while (a != nullptr || b != nullptr) {
            T valor;
            if (b == nullptr || (a != nullptr && !(b->valor < a->valor))) {
                valor = a->valor;
                a = a->proximo;
            } else {
                valor = b->valor;
                b = b->proximo;
            }
            No* novo = new No{valor, nullptr};
            if (cauda == nullptr)
                resultado.inicio_ = novo;
            else
                cauda->proximo = novo;
            cauda = novo;
            resultado.tamanho_++;
        }
        return resultado;
    }

    void limpar() {
        while (!vazia()) remover_inicio();
    }

    std::vector<T> para_vetor() const {
        std::vector<T> resultado;
        for (No* atual = inicio_; atual != nullptr; atual = atual->proximo) resultado.push_back(atual->valor);
        return resultado;
    }

private:
    No* inicio_ = nullptr;
    int tamanho_ = 0;
};
