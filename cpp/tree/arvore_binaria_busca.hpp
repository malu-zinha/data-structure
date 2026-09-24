// ÁRVORE BINÁRIA DE BUSCA (ABB / BST)
//
// Regra: para TODO nó,
//   - tudo na subárvore ESQUERDA é MENOR que ele;
//   - tudo na subárvore DIREITA é MAIOR que ele.
//
//               50
//             /    \
//           30      70
//          /  \    /  \
//        20   40  60   80
//
// - O percurso EM ORDEM sai ORDENADO: 20 30 40 50 60 70 80.
// - Mínimo = nó mais à esquerda. Máximo = nó mais à direita.
// - Valores repetidos são ignorados nesta implementação.
//
// Remoção (3 casos)
// 1. Nó FOLHA:           simplesmente some.
// 2. Nó com UM filho:    o filho sobe e ocupa o lugar dele.
// 3. Nó com DOIS filhos: copia o valor do SUCESSOR (menor da subárvore direita)
//                        e remove o sucessor lá embaixo (ele cai no caso 1 ou 2).
//
// | operação                   | caso médio | pior caso |
// |----------------------------|------------|-----------|
// | buscar / inserir / remover | O(log n)   | O(n)      |
// | mínimo / máximo            | O(log n)   | O(n)      |
// | percursos                  | O(n)       | O(n)      |
//
// Pega-ratão
// - O pior caso acontece quando os valores entram JÁ ORDENADOS: a árvore vira
//   uma "lista" torta. É por isso que existem AVL e Rubro-Negra.
// - Altura aqui conta ARESTAS (folha = 0, vazia = -1).
#pragma once

#include <algorithm>
#include <initializer_list>
#include <optional>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T>
class ArvoreBinariaBusca {
    struct No {
        T valor;
        No* esquerda = nullptr;
        No* direita = nullptr;
    };

public:
    ArvoreBinariaBusca() = default;
    ArvoreBinariaBusca(std::initializer_list<T> valores) {
        for (const T& valor : valores) inserir(valor);
    }
    ~ArvoreBinariaBusca() { liberar(raiz_); }
    ArvoreBinariaBusca(const ArvoreBinariaBusca&) = delete;
    ArvoreBinariaBusca& operator=(const ArvoreBinariaBusca&) = delete;

    bool vazia() const { return raiz_ == nullptr; }
    int tamanho() const { return tamanho_; }

    // ================================================================ inserção
    // Versão ITERATIVA: desce comparando até achar a vaga. true se inseriu.
    bool inserir(const T& valor) {
        if (raiz_ == nullptr) {
            raiz_ = new No{valor};
            tamanho_++;
            return true;
        }
        No* atual = raiz_;
        while (true) {
            if (valor == atual->valor) return false;  // repetido
            No*& lado = valor < atual->valor ? atual->esquerda : atual->direita;
            if (lado == nullptr) {
                lado = new No{valor};
                break;
            }
            atual = lado;
        }
        tamanho_++;
        return true;
    }

    // Mesma coisa, na versão RECURSIVA (a mais cobrada em prova).
    bool inserir_recursivo(const T& valor) {
        if (contem(valor)) return false;
        raiz_ = inserir(raiz_, valor);
        tamanho_++;
        return true;
    }

    // ================================================================ busca
    // Versão RECURSIVA: só precisa olhar UM lado a cada passo.
    bool buscar(const T& valor) const { return buscar(raiz_, valor) != nullptr; }

    bool contem(const T& valor) const {  // versão ITERATIVA
        No* atual = raiz_;
        while (atual != nullptr && atual->valor != valor) atual = valor < atual->valor ? atual->esquerda : atual->direita;
        return atual != nullptr;
    }

    const T& minimo() const {
        if (vazia()) throw std::invalid_argument("Árvore vazia");
        return minimo_no(raiz_)->valor;
    }

    const T& maximo() const {
        if (vazia()) throw std::invalid_argument("Árvore vazia");
        No* atual = raiz_;
        while (atual->direita != nullptr) atual = atual->direita;  // sempre para a direita
        return atual->valor;
    }

    // Menor valor da árvore que é MAIOR que `valor` (ou nenhum).
    std::optional<T> sucessor(const T& valor) const {
        std::optional<T> candidato;
        for (No* atual = raiz_; atual != nullptr;) {
            if (valor < atual->valor) {
                candidato = atual->valor;  // serve, mas pode ter um menor à esquerda
                atual = atual->esquerda;
            } else {
                atual = atual->direita;
            }
        }
        return candidato;
    }

    // Maior valor da árvore que é MENOR que `valor` (ou nenhum).
    std::optional<T> antecessor(const T& valor) const {
        std::optional<T> candidato;
        for (No* atual = raiz_; atual != nullptr;) {
            if (atual->valor < valor) {
                candidato = atual->valor;
                atual = atual->direita;
            } else {
                atual = atual->esquerda;
            }
        }
        return candidato;
    }

    // ================================================================ remoção
    // Remove `valor`. Devolve true se existia.
    bool remover(const T& valor) {
        if (!contem(valor)) return false;
        raiz_ = remover(raiz_, valor);
        tamanho_--;
        return true;
    }

    // ================================================================ percursos
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

    std::vector<T> pos_ordem() const {
        std::vector<T> resultado;
        visitar_pos(raiz_, resultado);
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

    // ================================================================ outras operações clássicas
    int altura() const { return altura(raiz_); }
    int contar_folhas() const { return contar_folhas(raiz_); }

    // Profundidade do valor (raiz = 0) ou -1. Anda só por um caminho.
    int nivel(const T& valor) const {
        int profundidade = 0;
        for (No* atual = raiz_; atual != nullptr; profundidade++) {
            if (valor == atual->valor) return profundidade;
            atual = valor < atual->valor ? atual->esquerda : atual->direita;
        }
        return -1;
    }

    // k-ésimo menor valor (k começa em 1): em ordem iterativo, para no k-ésimo.
    const T& k_esimo_menor(int k) const {
        if (k < 1 || k > tamanho_) throw std::out_of_range("k fora do intervalo");
        std::stack<No*> pilha;
        No* atual = raiz_;
        while (true) {
            while (atual != nullptr) {
                pilha.push(atual);
                atual = atual->esquerda;
            }
            atual = pilha.top();
            pilha.pop();
            if (--k == 0) return atual->valor;
            atual = atual->direita;
        }
    }

    // Valores em [minimo, maximo], ordenados. Poda os galhos inúteis.
    std::vector<T> intervalo(const T& minimo, const T& maximo) const {
        std::vector<T> resultado;
        intervalo(raiz_, minimo, maximo, resultado);
        return resultado;
    }

    // Ancestral comum mais próximo (LCA) de dois valores que existem na árvore.
    // Desce enquanto os dois estiverem do MESMO lado; onde eles se separam
    // (ou um deles é o próprio nó) está o ancestral comum.
    const T& ancestral_comum(const T& a, const T& b) const {
        if (!contem(a) || !contem(b)) throw std::invalid_argument("Os dois valores precisam estar na árvore");
        No* atual = raiz_;
        while (true) {
            if (a < atual->valor && b < atual->valor)
                atual = atual->esquerda;
            else if (atual->valor < a && atual->valor < b)
                atual = atual->direita;
            else
                return atual->valor;
        }
    }

    // Confere a regra da ABB usando limites (não basta olhar só os filhos!).
    bool eh_valida() const { return valida(raiz_, nullptr, nullptr); }

    std::string desenho() const {
        if (vazia()) return "(árvore vazia)\n";
        std::ostringstream saida;
        desenhar(raiz_, 0, saida);
        return saida.str();
    }

private:
    No* raiz_ = nullptr;
    int tamanho_ = 0;

    static void liberar(No* no) {
        if (no == nullptr) return;
        liberar(no->esquerda);
        liberar(no->direita);
        delete no;
    }

    static No* inserir(No* no, const T& valor) {
        if (no == nullptr) return new No{valor};  // achou a vaga: vira uma folha nova
        if (valor < no->valor)
            no->esquerda = inserir(no->esquerda, valor);
        else
            no->direita = inserir(no->direita, valor);
        return no;
    }

    static No* buscar(No* no, const T& valor) {
        if (no == nullptr || no->valor == valor) return no;
        return valor < no->valor ? buscar(no->esquerda, valor) : buscar(no->direita, valor);
    }

    static No* minimo_no(No* no) {
        while (no->esquerda != nullptr) no = no->esquerda;  // sempre para a esquerda
        return no;
    }

    // Remove `valor` da subárvore `no` e devolve a nova raiz dessa subárvore.
    static No* remover(No* no, const T& valor) {
        if (no == nullptr) return nullptr;
        if (valor < no->valor) {
            no->esquerda = remover(no->esquerda, valor);
        } else if (no->valor < valor) {
            no->direita = remover(no->direita, valor);
        } else {
            // Casos 1 e 2: zero ou um filho -> o filho (ou nullptr) ocupa o lugar
            if (no->esquerda == nullptr || no->direita == nullptr) {
                No* filho = no->esquerda != nullptr ? no->esquerda : no->direita;
                delete no;
                return filho;
            }
            // Caso 3: dois filhos -> copia o sucessor e remove o sucessor
            No* sucessor = minimo_no(no->direita);
            no->valor = sucessor->valor;
            no->direita = remover(no->direita, no->valor);  // no->valor já é o do sucessor
        }
        return no;
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

    static void visitar_pos(No* no, std::vector<T>& resultado) {
        if (!no) return;
        visitar_pos(no->esquerda, resultado);
        visitar_pos(no->direita, resultado);
        resultado.push_back(no->valor);
    }

    static int altura(No* no) { return no == nullptr ? -1 : 1 + std::max(altura(no->esquerda), altura(no->direita)); }

    static int contar_folhas(No* no) {
        if (no == nullptr) return 0;
        if (no->esquerda == nullptr && no->direita == nullptr) return 1;
        return contar_folhas(no->esquerda) + contar_folhas(no->direita);
    }

    static void intervalo(No* no, const T& minimo, const T& maximo, std::vector<T>& resultado) {
        if (no == nullptr) return;
        if (minimo < no->valor) intervalo(no->esquerda, minimo, maximo, resultado);  // pode haver válidos à esquerda
        if (!(no->valor < minimo) && !(maximo < no->valor)) resultado.push_back(no->valor);
        if (no->valor < maximo) intervalo(no->direita, minimo, maximo, resultado);  // pode haver válidos à direita
    }

    static bool valida(No* no, const T* menor_permitido, const T* maior_permitido) {
        if (no == nullptr) return true;
        if (menor_permitido && !(*menor_permitido < no->valor)) return false;
        if (maior_permitido && !(no->valor < *maior_permitido)) return false;
        return valida(no->esquerda, menor_permitido, &no->valor) && valida(no->direita, &no->valor, maior_permitido);
    }

    static void desenhar(No* no, int nivel, std::ostringstream& saida) {
        if (!no) return;
        desenhar(no->direita, nivel + 1, saida);
        saida << std::string(4 * nivel, ' ') << no->valor << "\n";
        desenhar(no->esquerda, nivel + 1, saida);
    }
};
