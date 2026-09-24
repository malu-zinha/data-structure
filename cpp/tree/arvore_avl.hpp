// ÁRVORE AVL (Adelson-Velsky e Landis)
//
// - É uma ABB que se AUTO-BALANCEIA: para TODO nó,
//       fator de balanceamento (FB) = altura(esquerda) - altura(direita)
//   precisa ficar em {-1, 0, +1}.
// - Depois de inserir/remover, subindo pelo caminho, se algum nó ficar com
//   FB = +2 ou -2, aplica-se uma ROTAÇÃO.
// - Garante altura O(log n) -> busca, inserção e remoção O(log n) SEMPRE.
//
// FB do nó | FB do filho | caso | solução
// ---------|-------------|------|----------------------------------------------
//    +2    |  >= 0 (esq) |  LL  | rotação simples à DIREITA
//    -2    |  <= 0 (dir) |  RR  | rotação simples à ESQUERDA
//    +2    |   -1  (esq) |  LR  | rotação dupla: esquerda no filho, direita no nó
//    -2    |   +1  (dir) |  RL  | rotação dupla: direita no filho, esquerda no nó
//
// Rotação à direita (caso LL):
//
//           y                x
//          / \              / \
//         x   C    --->    A   y
//        / \                  / \
//       A   B                B   C
//
// Pega-ratão
// - Depois de rotacionar, atualize a altura de quem ficou EMBAIXO primeiro.
// - Na remoção pode ser preciso rotacionar em VÁRIOS níveis (na inserção, no
//   máximo uma vez).
// - Altura: folha = 0, vazia = -1.
#pragma once

#include <algorithm>
#include <cstdlib>
#include <initializer_list>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T>
class ArvoreAVL {
    struct No {
        T valor;
        No* esquerda = nullptr;
        No* direita = nullptr;
        int altura = 0;  // folha
    };

public:
    std::vector<std::string> rotacoes;  // histórico das rotações, bom para conferir exercícios

    ArvoreAVL() = default;
    ArvoreAVL(std::initializer_list<T> valores) {
        for (const T& valor : valores) inserir(valor);
    }
    ~ArvoreAVL() { liberar(raiz_); }
    ArvoreAVL(const ArvoreAVL&) = delete;
    ArvoreAVL& operator=(const ArvoreAVL&) = delete;

    bool vazia() const { return raiz_ == nullptr; }
    int tamanho() const { return tamanho_; }
    int altura() const { return altura(raiz_); }

    // ================================================================ inserção e remoção
    // Iguais às da ABB + balancear na volta da recursão.
    bool inserir(const T& valor) {
        if (contem(valor)) return false;
        raiz_ = inserir(raiz_, valor);
        tamanho_++;
        return true;
    }

    bool remover(const T& valor) {
        if (!contem(valor)) return false;
        raiz_ = remover(raiz_, valor);
        tamanho_--;
        return true;
    }

    // ================================================================ consultas
    bool contem(const T& valor) const { return buscar(valor) != nullptr; }

    const T& minimo() const {
        if (vazia()) throw std::invalid_argument("Árvore vazia");
        No* atual = raiz_;
        while (atual->esquerda) atual = atual->esquerda;
        return atual->valor;
    }

    const T& maximo() const {
        if (vazia()) throw std::invalid_argument("Árvore vazia");
        No* atual = raiz_;
        while (atual->direita) atual = atual->direita;
        return atual->valor;
    }

    int fator_balanceamento(const T& valor) const {
        No* no = buscar(valor);
        if (no == nullptr) throw std::invalid_argument("Valor não está na árvore");
        return fator(no);
    }

    std::vector<T> pre_ordem() const {
        std::vector<T> resultado;
        visitar_pre(raiz_, resultado);
        return resultado;
    }

    std::vector<T> em_ordem() const {
        std::vector<T> resultado;
        visitar_em(raiz_, resultado);
        return resultado;
    }

    std::vector<T> em_nivel() const {
        std::vector<T> resultado;
        std::queue<No*> fila;
        if (raiz_) fila.push(raiz_);
        while (!fila.empty()) {
            No* no = fila.front();
            fila.pop();
            resultado.push_back(no->valor);
            if (no->esquerda) fila.push(no->esquerda);
            if (no->direita) fila.push(no->direita);
        }
        return resultado;
    }

    // Confere: ordem de ABB, alturas guardadas corretas e |FB| <= 1.
    bool eh_valida() const {
        int altura_calculada;
        return valida(raiz_, nullptr, nullptr, altura_calculada);
    }

    // Desenho deitado mostrando o fator de balanceamento de cada nó.
    std::string desenho() const {
        if (vazia()) return "(árvore vazia)\n";
        std::ostringstream saida;
        desenhar(raiz_, 0, saida);
        return saida.str();
    }

private:
    No* raiz_ = nullptr;
    int tamanho_ = 0;

    static int altura(No* no) { return no != nullptr ? no->altura : -1; }
    static void atualizar_altura(No* no) { no->altura = 1 + std::max(altura(no->esquerda), altura(no->direita)); }
    static int fator(No* no) { return altura(no->esquerda) - altura(no->direita); }

    static void liberar(No* no) {
        if (no == nullptr) return;
        liberar(no->esquerda);
        liberar(no->direita);
        delete no;
    }

    No* buscar(const T& valor) const {
        No* atual = raiz_;
        while (atual != nullptr && atual->valor != valor) atual = valor < atual->valor ? atual->esquerda : atual->direita;
        return atual;
    }

    // ================================================================ rotações
    No* rotacao_direita(No* y) {
        No* x = y->esquerda;
        y->esquerda = x->direita;  // a subárvore B troca de pai
        x->direita = y;
        atualizar_altura(y);  // y agora está embaixo: atualiza primeiro
        atualizar_altura(x);
        registrar("direita", y->valor);
        return x;  // x é a nova raiz desta subárvore
    }

    No* rotacao_esquerda(No* x) {
        No* y = x->direita;
        x->direita = y->esquerda;
        y->esquerda = x;
        atualizar_altura(x);
        atualizar_altura(y);
        registrar("esquerda", x->valor);
        return y;
    }

    void registrar(const char* lado, const T& valor) {
        std::ostringstream texto;
        texto << lado << " em " << valor;
        rotacoes.push_back(texto.str());
    }

    // Atualiza a altura e, se o nó estiver desbalanceado, aplica a rotação certa.
    No* balancear(No* no) {
        atualizar_altura(no);
        int fb = fator(no);
        if (fb > 1) {                                                          // pesado à ESQUERDA
            if (fator(no->esquerda) < 0) no->esquerda = rotacao_esquerda(no->esquerda);  // caso LR
            return rotacao_direita(no);                                        // caso LL
        }
        if (fb < -1) {                                                         // pesado à DIREITA
            if (fator(no->direita) > 0) no->direita = rotacao_direita(no->direita);  // caso RL
            return rotacao_esquerda(no);                                       // caso RR
        }
        return no;  // já estava balanceado
    }

    No* inserir(No* no, const T& valor) {
        if (no == nullptr) return new No{valor};
        if (valor < no->valor)
            no->esquerda = inserir(no->esquerda, valor);
        else
            no->direita = inserir(no->direita, valor);
        return balancear(no);  // <- a única diferença para a ABB
    }

    No* remover(No* no, const T& valor) {
        if (no == nullptr) return nullptr;
        if (valor < no->valor) {
            no->esquerda = remover(no->esquerda, valor);
        } else if (no->valor < valor) {
            no->direita = remover(no->direita, valor);
        } else {
            if (no->esquerda == nullptr || no->direita == nullptr) {
                No* filho = no->esquerda != nullptr ? no->esquerda : no->direita;
                delete no;
                return filho;
            }
            No* sucessor = no->direita;
            while (sucessor->esquerda != nullptr) sucessor = sucessor->esquerda;
            no->valor = sucessor->valor;
            no->direita = remover(no->direita, no->valor);  // no->valor já é o do sucessor
        }
        return balancear(no);
    }

    static bool valida(No* no, const T* menor, const T* maior, int& altura_calculada) {
        if (no == nullptr) {
            altura_calculada = -1;
            return true;
        }
        if ((menor && !(*menor < no->valor)) || (maior && !(no->valor < *maior))) return false;
        int h_esq, h_dir;
        if (!valida(no->esquerda, menor, &no->valor, h_esq) || !valida(no->direita, &no->valor, maior, h_dir))
            return false;
        altura_calculada = 1 + std::max(h_esq, h_dir);
        return std::abs(h_esq - h_dir) <= 1 && no->altura == altura_calculada;
    }

    static void visitar_pre(No* no, std::vector<T>& resultado) {
        if (!no) return;
        resultado.push_back(no->valor);
        visitar_pre(no->esquerda, resultado);
        visitar_pre(no->direita, resultado);
    }

    static void visitar_em(No* no, std::vector<T>& resultado) {
        if (!no) return;
        visitar_em(no->esquerda, resultado);
        resultado.push_back(no->valor);
        visitar_em(no->direita, resultado);
    }

    static void desenhar(No* no, int nivel, std::ostringstream& saida) {
        if (!no) return;
        desenhar(no->direita, nivel + 1, saida);
        saida << std::string(6 * nivel, ' ') << no->valor << "(fb=" << fator(no) << ")\n";
        desenhar(no->esquerda, nivel + 1, saida);
    }
};
