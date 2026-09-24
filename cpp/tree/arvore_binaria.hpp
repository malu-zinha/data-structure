// ÁRVORE BINÁRIA (genérica, sem ordem entre os valores)
//
// - Cada nó tem no máximo DOIS filhos: `esquerda` e `direita`.
// - Raiz: o nó do topo. Folha: nó sem filhos. Nível/profundidade: distância até a raiz.
//
//               1          <- raiz (nível 0)
//             /   \
//            2     3       <- nível 1
//           / \     \
//          4   5     7     <- nível 2 (4, 5 e 7 são folhas)
//
// Percursos
// - Pré-ordem  (Raiz, Esq, Dir): 1 2 4 5 3 7
// - Em ordem   (Esq, Raiz, Dir): 4 2 5 1 3 7
// - Pós-ordem  (Esq, Dir, Raiz): 4 5 2 7 3 1
// - Em nível   (largura / BFS):  1 2 3 4 5 7   <- usa FILA
//
// Definições usadas aqui (os livros variam!)
// - Altura: nº de ARESTAS da raiz até a folha mais funda. Folha = 0, vazia = -1.
// - Estritamente binária: todo nó tem 0 ou 2 filhos.
// - Completa: todos os níveis cheios, exceto talvez o último, que é preenchido
//   da esquerda para a direita.
// - Cheia (perfeita): todos os níveis completamente cheios -> n = 2^(h+1) - 1.
//
// Complexidade: percursos, altura, contagens e busca são O(n).
#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <queue>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

template <typename T>
class ArvoreBinaria {
public:
    struct No {
        T valor;
        No* esquerda = nullptr;
        No* direita = nullptr;
    };

    ArvoreBinaria() = default;
    explicit ArvoreBinaria(No* raiz) : raiz_(raiz) {}  // a árvore passa a ser dona dos nós
    ~ArvoreBinaria() { liberar(raiz_); }
    ArvoreBinaria(const ArvoreBinaria&) = delete;
    ArvoreBinaria& operator=(const ArvoreBinaria&) = delete;
    ArvoreBinaria(ArvoreBinaria&& outra) noexcept : raiz_(outra.raiz_) { outra.raiz_ = nullptr; }

    // ================================================================ construção
    // Monta a árvore a partir da representação em VETOR.
    // O filho esquerdo do índice i fica em 2i+1 e o direito em 2i+2.
    // `std::nullopt` marca uma posição sem nó.
    // Ex.: {1, 2, 3, 4, 5, nullopt, 7} gera a árvore do topo deste arquivo.
    static ArvoreBinaria de_lista_por_nivel(const std::vector<std::optional<T>>& valores) {
        std::function<No*(std::size_t)> construir = [&](std::size_t i) -> No* {
            if (i >= valores.size() || !valores[i]) return nullptr;
            return new No{*valores[i], construir(2 * i + 1), construir(2 * i + 2)};
        };
        return ArvoreBinaria(construir(0));
    }

    // Reconstrói a árvore a partir da pré-ordem + em ordem (valores únicos).
    // - O 1º da pré-ordem é a raiz.
    // - Na em ordem, tudo à esquerda da raiz é a subárvore esquerda e tudo à
    //   direita é a subárvore direita. Repete recursivamente.
    static ArvoreBinaria de_percursos(const std::vector<T>& pre_ordem, const std::vector<T>& em_ordem) {
        std::vector<T> a = pre_ordem, b = em_ordem;
        std::sort(a.begin(), a.end());
        std::sort(b.begin(), b.end());
        if (a != b || std::adjacent_find(b.begin(), b.end()) != b.end())
            throw std::invalid_argument("Percursos incompatíveis ou com valores repetidos");

        std::map<T, int> posicao;
        for (int i = 0; i < static_cast<int>(em_ordem.size()); i++) posicao[em_ordem[i]] = i;
        std::size_t proximo = 0;

        std::function<No*(int, int)> construir = [&](int ini, int fim) -> No* {  // intervalo [ini, fim] da em ordem
            if (ini > fim) return nullptr;
            const T& valor = pre_ordem[proximo++];
            int meio = posicao[valor];
            No* no = new No{valor};
            no->esquerda = construir(ini, meio - 1);
            no->direita = construir(meio + 1, fim);
            return no;
        };
        return ArvoreBinaria(construir(0, static_cast<int>(em_ordem.size()) - 1));
    }

    // Insere na primeira vaga encontrada no percurso em nível.
    // Se a árvore era completa, continua completa.
    void inserir_por_nivel(const T& valor) {
        No* novo = new No{valor};
        if (raiz_ == nullptr) {
            raiz_ = novo;
            return;
        }
        std::queue<No*> fila;
        fila.push(raiz_);
        while (!fila.empty()) {
            No* no = fila.front();
            fila.pop();
            if (no->esquerda == nullptr) {
                no->esquerda = novo;
                return;
            }
            if (no->direita == nullptr) {
                no->direita = novo;
                return;
            }
            fila.push(no->esquerda);
            fila.push(no->direita);
        }
    }

    // ================================================================ percursos recursivos
    std::vector<T> pre_ordem() const {
        std::vector<T> resultado;
        pre_ordem(raiz_, resultado);
        return resultado;
    }

    std::vector<T> em_ordem() const {
        std::vector<T> resultado;
        em_ordem(raiz_, resultado);
        return resultado;
    }

    std::vector<T> pos_ordem() const {
        std::vector<T> resultado;
        pos_ordem(raiz_, resultado);
        return resultado;
    }

    // ================================================================ percursos iterativos
    // A recursão vira uma PILHA explícita.
    std::vector<T> pre_ordem_iterativa() const {
        std::vector<T> resultado;
        if (raiz_ == nullptr) return resultado;
        std::stack<No*> pilha;
        pilha.push(raiz_);
        while (!pilha.empty()) {
            No* no = pilha.top();
            pilha.pop();
            resultado.push_back(no->valor);
            // empilha a DIREITA primeiro para a ESQUERDA sair antes
            if (no->direita) pilha.push(no->direita);
            if (no->esquerda) pilha.push(no->esquerda);
        }
        return resultado;
    }

    std::vector<T> em_ordem_iterativa() const {
        std::vector<T> resultado;
        std::stack<No*> pilha;
        No* atual = raiz_;
        while (!pilha.empty() || atual != nullptr) {
            while (atual != nullptr) {  // desce o máximo possível à esquerda
                pilha.push(atual);
                atual = atual->esquerda;
            }
            atual = pilha.top();  // visita
            pilha.pop();
            resultado.push_back(atual->valor);
            atual = atual->direita;  // e vai para a direita
        }
        return resultado;
    }

    // Faz Raiz-Dir-Esq com pilha e inverte no final: vira Esq-Dir-Raiz.
    std::vector<T> pos_ordem_iterativa() const {
        std::vector<T> resultado;
        if (raiz_ == nullptr) return resultado;
        std::stack<No*> pilha;
        pilha.push(raiz_);
        while (!pilha.empty()) {
            No* no = pilha.top();
            pilha.pop();
            resultado.push_back(no->valor);
            if (no->esquerda) pilha.push(no->esquerda);
            if (no->direita) pilha.push(no->direita);
        }
        std::reverse(resultado.begin(), resultado.end());
        return resultado;
    }

    // Percurso em largura (BFS) usando FILA.
    std::vector<T> em_nivel() const {
        std::vector<T> resultado;
        for (const auto& nivel : por_niveis()) resultado.insert(resultado.end(), nivel.begin(), nivel.end());
        return resultado;
    }

    // Em nível, separando: {{1}, {2, 3}, {4, 5, 7}}.
    std::vector<std::vector<T>> por_niveis() const {
        std::vector<std::vector<T>> niveis;
        if (raiz_ == nullptr) return niveis;
        std::queue<No*> fila;
        fila.push(raiz_);
        while (!fila.empty()) {
            std::vector<T> nivel_atual;
            for (std::size_t n = fila.size(); n > 0; n--) {  // só os nós que já estavam na fila
                No* no = fila.front();
                fila.pop();
                nivel_atual.push_back(no->valor);
                if (no->esquerda) fila.push(no->esquerda);
                if (no->direita) fila.push(no->direita);
            }
            niveis.push_back(nivel_atual);
        }
        return niveis;
    }

    // ================================================================ medidas
    int tamanho() const { return contar(raiz_); }
    int altura() const { return altura(raiz_); }
    int contar_folhas() const { return contar_folhas(raiz_); }
    int contar_nos_internos() const { return tamanho() - contar_folhas(); }

    // Profundidade do nó com `valor` (raiz = 0) ou -1 se não existir.
    int nivel(const T& valor) const { return nivel(raiz_, valor, 0); }

    // ================================================================ busca e modificação
    // Precisa olhar a árvore toda: O(n).
    bool contem(const T& valor) const { return buscar(raiz_, valor) != nullptr; }

    // Remove a primeira ocorrência (em nível) de `valor`.
    // Árvore genérica não tem ordem, então: copia o valor do nó MAIS FUNDO e
    // MAIS À DIREITA para o nó a remover e apaga esse nó mais fundo.
    bool remover(const T& valor) {
        if (raiz_ == nullptr) return false;
        No* alvo = nullptr;
        No* ultimo = nullptr;
        No* pai_do_ultimo = nullptr;
        std::queue<std::pair<No*, No*>> fila;  // (nó, pai)
        fila.push({raiz_, nullptr});
        while (!fila.empty()) {
            auto [no, pai] = fila.front();
            fila.pop();
            if (alvo == nullptr && no->valor == valor) alvo = no;
            ultimo = no;
            pai_do_ultimo = pai;
            if (no->esquerda) fila.push({no->esquerda, no});
            if (no->direita) fila.push({no->direita, no});
        }
        if (alvo == nullptr) return false;

        alvo->valor = ultimo->valor;
        if (pai_do_ultimo == nullptr)  // a árvore só tinha a raiz
            raiz_ = nullptr;
        else if (pai_do_ultimo->direita == ultimo)
            pai_do_ultimo->direita = nullptr;
        else
            pai_do_ultimo->esquerda = nullptr;
        delete ultimo;
        return true;
    }

    // Troca esquerda e direita em todos os nós (imagem no espelho).
    void espelhar() { espelhar(raiz_); }

    // ================================================================ classificações
    bool eh_estritamente_binaria() const { return estritamente_binaria(raiz_); }

    // Em nível: depois do primeiro "buraco" não pode aparecer mais nenhum nó.
    bool eh_completa() const {
        if (raiz_ == nullptr) return true;
        std::queue<No*> fila;
        fila.push(raiz_);
        bool viu_buraco = false;
        while (!fila.empty()) {
            No* no = fila.front();
            fila.pop();
            for (No* filho : {no->esquerda, no->direita}) {
                if (filho == nullptr)
                    viu_buraco = true;
                else if (viu_buraco)
                    return false;
                else
                    fila.push(filho);
            }
        }
        return true;
    }

    bool eh_cheia() const { return tamanho() == (1 << (altura() + 1)) - 1; }

    // Mesma forma e mesmos valores.
    bool igual_a(const ArvoreBinaria& outra) const { return iguais(raiz_, outra.raiz_); }

    // ================================================================ exibição
    // Desenha a árvore DEITADA (incline a cabeça para a esquerda):
    // a direita fica em cima e a esquerda embaixo.
    std::string desenho() const {
        if (raiz_ == nullptr) return "(árvore vazia)\n";
        std::ostringstream saida;
        desenhar(raiz_, 0, saida);
        return saida.str();
    }

private:
    No* raiz_ = nullptr;

    static void liberar(No* no) {
        if (no == nullptr) return;
        liberar(no->esquerda);
        liberar(no->direita);
        delete no;  // pós-ordem: os filhos saem antes do pai
    }

    static void pre_ordem(No* no, std::vector<T>& resultado) {
        if (no == nullptr) return;
        resultado.push_back(no->valor);  // R
        pre_ordem(no->esquerda, resultado);  // E
        pre_ordem(no->direita, resultado);   // D
    }

    static void em_ordem(No* no, std::vector<T>& resultado) {
        if (no == nullptr) return;
        em_ordem(no->esquerda, resultado);  // E
        resultado.push_back(no->valor);     // R
        em_ordem(no->direita, resultado);   // D
    }

    static void pos_ordem(No* no, std::vector<T>& resultado) {
        if (no == nullptr) return;
        pos_ordem(no->esquerda, resultado);  // E
        pos_ordem(no->direita, resultado);   // D
        resultado.push_back(no->valor);      // R
    }

    static int contar(No* no) { return no == nullptr ? 0 : 1 + contar(no->esquerda) + contar(no->direita); }

    static int altura(No* no) {
        if (no == nullptr) return -1;
        return 1 + std::max(altura(no->esquerda), altura(no->direita));
    }

    static int contar_folhas(No* no) {
        if (no == nullptr) return 0;
        if (no->esquerda == nullptr && no->direita == nullptr) return 1;
        return contar_folhas(no->esquerda) + contar_folhas(no->direita);
    }

    static int nivel(No* no, const T& valor, int profundidade) {
        if (no == nullptr) return -1;
        if (no->valor == valor) return profundidade;
        int achou = nivel(no->esquerda, valor, profundidade + 1);
        if (achou != -1) return achou;
        return nivel(no->direita, valor, profundidade + 1);
    }

    static No* buscar(No* no, const T& valor) {
        if (no == nullptr || no->valor == valor) return no;
        No* achou = buscar(no->esquerda, valor);
        return achou != nullptr ? achou : buscar(no->direita, valor);
    }

    static void espelhar(No* no) {
        if (no == nullptr) return;
        std::swap(no->esquerda, no->direita);
        espelhar(no->esquerda);
        espelhar(no->direita);
    }

    static bool estritamente_binaria(No* no) {
        if (no == nullptr) return true;
        if ((no->esquerda == nullptr) != (no->direita == nullptr)) return false;  // só um filho
        return estritamente_binaria(no->esquerda) && estritamente_binaria(no->direita);
    }

    static bool iguais(No* a, No* b) {
        if (a == nullptr || b == nullptr) return a == b;
        return a->valor == b->valor && iguais(a->esquerda, b->esquerda) && iguais(a->direita, b->direita);
    }

    static void desenhar(No* no, int nivel, std::ostringstream& saida) {
        if (no == nullptr) return;
        desenhar(no->direita, nivel + 1, saida);
        saida << std::string(4 * nivel, ' ') << no->valor << "\n";
        desenhar(no->esquerda, nivel + 1, saida);
    }
};
