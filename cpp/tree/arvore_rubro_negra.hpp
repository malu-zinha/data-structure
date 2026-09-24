// ÁRVORE RUBRO-NEGRA (Red-Black Tree)
//
// É uma ABB em que cada nó tem uma COR (vermelho ou preto) e valem as regras:
//   1. Todo nó é VERMELHO ou PRETO.
//   2. A RAIZ é preta.
//   3. Toda folha nula (NIL) é preta.
//   4. Nó VERMELHO não pode ter filho vermelho (nada de dois vermelhos seguidos).
//   5. Todo caminho de um nó até as folhas NIL tem a MESMA quantidade de nós
//      pretos (a "altura negra").
//
// Consequência: o caminho mais longo tem no máximo o dobro do mais curto, então
// a altura é no máximo 2*log2(n+1) -> busca, inserção e remoção O(log n).
//
// AVL x Rubro-Negra
// - AVL é MAIS balanceada: buscas um pouco mais rápidas.
// - Rubro-Negra faz MENOS rotações para inserir/remover. É a usada no std::map
//   e no std::set do C++, no TreeMap do Java e no escalonador do Linux.
//
// Inserção: o nó novo entra VERMELHO. Se o pai também for vermelho (fere a regra 4):
//   - Caso 1: tio VERMELHO    -> recolore pai e tio de preto, avô de vermelho, sobe.
//   - Caso 2: tio PRETO, nó em "joelho" (zigue-zague) -> rotaciona o pai e cai no caso 3.
//   - Caso 3: tio PRETO, nó em "linha" -> rotaciona o avô e troca as cores.
//
// Implementação baseada no livro do Cormen (CLRS), com um nó SENTINELA `nil_`
// (preto) no lugar de todos os nullptr. Isso evita um monte de `if (no == nullptr)`.
#pragma once

#include <algorithm>
#include <initializer_list>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

template <typename T>
class ArvoreRubroNegra {
public:
    enum Cor : char { VERMELHO = 'V', PRETO = 'P' };

private:
    struct No {
        T valor;
        Cor cor;
        No* esquerda;
        No* direita;
        No* pai;
    };

public:
    ArvoreRubroNegra() {
        nil_ = new No{T{}, PRETO, nullptr, nullptr, nullptr};  // sentinela: todas as folhas nulas
        raiz_ = nil_;
    }
    ArvoreRubroNegra(std::initializer_list<T> valores) : ArvoreRubroNegra() {
        for (const T& valor : valores) inserir(valor);
    }
    ~ArvoreRubroNegra() {
        liberar(raiz_);
        delete nil_;
    }
    ArvoreRubroNegra(const ArvoreRubroNegra&) = delete;
    ArvoreRubroNegra& operator=(const ArvoreRubroNegra&) = delete;

    bool vazia() const { return raiz_ == nil_; }
    int tamanho() const { return tamanho_; }

    // ================================================================ inserção
    bool inserir(const T& valor) {
        No* pai = nil_;
        No* atual = raiz_;
        while (atual != nil_) {  // desce como numa ABB comum
            if (valor == atual->valor) return false;
            pai = atual;
            atual = valor < atual->valor ? atual->esquerda : atual->direita;
        }
        No* novo = new No{valor, VERMELHO, nil_, nil_, pai};  // todo nó novo nasce vermelho
        if (pai == nil_)
            raiz_ = novo;
        else if (valor < pai->valor)
            pai->esquerda = novo;
        else
            pai->direita = novo;
        tamanho_++;
        corrigir_insercao(novo);
        return true;
    }

    // ================================================================ remoção
    bool remover(const T& valor) {
        No* z = buscar_no(valor);
        if (z == nil_) return false;

        No* y = z;  // nó que sai de fato da árvore
        Cor cor_que_saiu = y->cor;
        No* x;
        if (z->esquerda == nil_) {
            x = z->direita;
            transplantar(z, z->direita);
        } else if (z->direita == nil_) {
            x = z->esquerda;
            transplantar(z, z->esquerda);
        } else {
            y = minimo_no(z->direita);  // sucessor
            cor_que_saiu = y->cor;
            x = y->direita;
            if (y->pai == z) {
                x->pai = y;
            } else {
                transplantar(y, y->direita);
                y->direita = z->direita;
                y->direita->pai = y;
            }
            transplantar(z, y);
            y->esquerda = z->esquerda;
            y->esquerda->pai = y;
            y->cor = z->cor;
        }
        delete z;
        tamanho_--;
        // Se saiu um PRETO, algum caminho ficou com um preto a menos (regra 5).
        if (cor_que_saiu == PRETO) corrigir_remocao(x);
        return true;
    }

    // ================================================================ consultas
    bool contem(const T& valor) const { return buscar_no(valor) != nil_; }

    Cor cor_de(const T& valor) const {
        No* no = buscar_no(valor);
        if (no == nil_) throw std::invalid_argument("Valor não está na árvore");
        return no->cor;
    }

    const T& minimo() const {
        if (vazia()) throw std::invalid_argument("Árvore vazia");
        return minimo_no(raiz_)->valor;
    }

    const T& maximo() const {
        if (vazia()) throw std::invalid_argument("Árvore vazia");
        No* atual = raiz_;
        while (atual->direita != nil_) atual = atual->direita;
        return atual->valor;
    }

    int altura() const { return altura(raiz_); }

    // Nº de nós pretos da raiz até uma folha NIL (sem contar a raiz, contando o NIL).
    int altura_negra() const {
        int contagem = 0;
        for (No* atual = raiz_; atual != nil_;) {
            atual = atual->esquerda;
            if (atual->cor == PRETO) contagem++;
        }
        return contagem;
    }

    std::vector<T> em_ordem() const {
        std::vector<T> resultado;
        visitar_em(raiz_, resultado);
        return resultado;
    }

    std::vector<T> em_nivel() const {
        std::vector<T> resultado;
        std::queue<No*> fila;
        if (!vazia()) fila.push(raiz_);
        while (!fila.empty()) {
            No* no = fila.front();
            fila.pop();
            resultado.push_back(no->valor);
            if (no->esquerda != nil_) fila.push(no->esquerda);
            if (no->direita != nil_) fila.push(no->direita);
        }
        return resultado;
    }

    // Confere as 5 regras + ordem de ABB + ponteiros `pai`.
    bool eh_valida() const {
        if (raiz_->cor != PRETO || nil_->cor != PRETO) return false;
        return altura_negra_valida(raiz_, nullptr, nullptr) != -1;
    }

    // Desenho deitado: V = vermelho, P = preto.
    std::string desenho() const {
        if (vazia()) return "(árvore vazia)\n";
        std::ostringstream saida;
        desenhar(raiz_, 0, saida);
        return saida.str();
    }

private:
    No* nil_;
    No* raiz_;
    int tamanho_ = 0;

    void liberar(No* no) {
        if (no == nil_) return;
        liberar(no->esquerda);
        liberar(no->direita);
        delete no;
    }

    // ================================================================ rotações
    // Iguais às da AVL, mas também atualizam o ponteiro `pai`.
    void rotacao_esquerda(No* x) {
        No* y = x->direita;
        x->direita = y->esquerda;
        if (y->esquerda != nil_) y->esquerda->pai = x;
        y->pai = x->pai;
        if (x->pai == nil_)
            raiz_ = y;
        else if (x == x->pai->esquerda)
            x->pai->esquerda = y;
        else
            x->pai->direita = y;
        y->esquerda = x;
        x->pai = y;
    }

    void rotacao_direita(No* x) {
        No* y = x->esquerda;
        x->esquerda = y->direita;
        if (y->direita != nil_) y->direita->pai = x;
        y->pai = x->pai;
        if (x->pai == nil_)
            raiz_ = y;
        else if (x == x->pai->direita)
            x->pai->direita = y;
        else
            x->pai->esquerda = y;
        y->direita = x;
        x->pai = y;
    }

    void corrigir_insercao(No* no) {
        while (no->pai->cor == VERMELHO) {  // dois vermelhos seguidos: precisa arrumar
            No* avo = no->pai->pai;
            if (no->pai == avo->esquerda) {
                No* tio = avo->direita;
                if (tio->cor == VERMELHO) {  // Caso 1: só recolore
                    no->pai->cor = PRETO;
                    tio->cor = PRETO;
                    avo->cor = VERMELHO;
                    no = avo;  // o problema pode ter subido
                } else {
                    if (no == no->pai->direita) {  // Caso 2: joelho -> vira linha
                        no = no->pai;
                        rotacao_esquerda(no);
                    }
                    no->pai->cor = PRETO;  // Caso 3: linha
                    no->pai->pai->cor = VERMELHO;
                    rotacao_direita(no->pai->pai);
                }
            } else {  // espelho: o pai é filho DIREITO do avô
                No* tio = avo->esquerda;
                if (tio->cor == VERMELHO) {
                    no->pai->cor = PRETO;
                    tio->cor = PRETO;
                    avo->cor = VERMELHO;
                    no = avo;
                } else {
                    if (no == no->pai->esquerda) {
                        no = no->pai;
                        rotacao_direita(no);
                    }
                    no->pai->cor = PRETO;
                    no->pai->pai->cor = VERMELHO;
                    rotacao_esquerda(no->pai->pai);
                }
            }
        }
        raiz_->cor = PRETO;  // regra 2
    }

    // Coloca a subárvore `v` no lugar da subárvore `u`.
    void transplantar(No* u, No* v) {
        if (u->pai == nil_)
            raiz_ = v;
        else if (u == u->pai->esquerda)
            u->pai->esquerda = v;
        else
            u->pai->direita = v;
        v->pai = u->pai;  // pode escrever no nil_; é assim mesmo no CLRS
    }

    // `x` carrega um "preto extra" que precisa ser resolvido.
    void corrigir_remocao(No* x) {
        while (x != raiz_ && x->cor == PRETO) {
            if (x == x->pai->esquerda) {
                No* irmao = x->pai->direita;
                if (irmao->cor == VERMELHO) {  // Caso 1
                    irmao->cor = PRETO;
                    x->pai->cor = VERMELHO;
                    rotacao_esquerda(x->pai);
                    irmao = x->pai->direita;
                }
                if (irmao->esquerda->cor == PRETO && irmao->direita->cor == PRETO) {  // Caso 2
                    irmao->cor = VERMELHO;
                    x = x->pai;
                } else {
                    if (irmao->direita->cor == PRETO) {  // Caso 3
                        irmao->esquerda->cor = PRETO;
                        irmao->cor = VERMELHO;
                        rotacao_direita(irmao);
                        irmao = x->pai->direita;
                    }
                    irmao->cor = x->pai->cor;  // Caso 4
                    x->pai->cor = PRETO;
                    irmao->direita->cor = PRETO;
                    rotacao_esquerda(x->pai);
                    x = raiz_;
                }
            } else {  // espelho
                No* irmao = x->pai->esquerda;
                if (irmao->cor == VERMELHO) {
                    irmao->cor = PRETO;
                    x->pai->cor = VERMELHO;
                    rotacao_direita(x->pai);
                    irmao = x->pai->esquerda;
                }
                if (irmao->direita->cor == PRETO && irmao->esquerda->cor == PRETO) {
                    irmao->cor = VERMELHO;
                    x = x->pai;
                } else {
                    if (irmao->esquerda->cor == PRETO) {
                        irmao->direita->cor = PRETO;
                        irmao->cor = VERMELHO;
                        rotacao_esquerda(irmao);
                        irmao = x->pai->esquerda;
                    }
                    irmao->cor = x->pai->cor;
                    x->pai->cor = PRETO;
                    irmao->esquerda->cor = PRETO;
                    rotacao_direita(x->pai);
                    x = raiz_;
                }
            }
        }
        x->cor = PRETO;
    }

    No* buscar_no(const T& valor) const {
        No* atual = raiz_;
        while (atual != nil_ && atual->valor != valor) atual = valor < atual->valor ? atual->esquerda : atual->direita;
        return atual;
    }

    No* minimo_no(No* no) const {
        while (no->esquerda != nil_) no = no->esquerda;
        return no;
    }

    int altura(No* no) const { return no == nil_ ? -1 : 1 + std::max(altura(no->esquerda), altura(no->direita)); }

    void visitar_em(No* no, std::vector<T>& resultado) const {
        if (no == nil_) return;
        visitar_em(no->esquerda, resultado);
        resultado.push_back(no->valor);
        visitar_em(no->direita, resultado);
    }

    // Devolve a altura negra da subárvore ou -1 se alguma regra falhar.
    int altura_negra_valida(No* no, const T* menor, const T* maior) const {
        if (no == nil_) return 1;
        if ((menor && !(*menor < no->valor)) || (maior && !(no->valor < *maior))) return -1;
        for (No* filho : {no->esquerda, no->direita}) {
            if (filho != nil_ && filho->pai != no) return -1;
            if (no->cor == VERMELHO && filho->cor == VERMELHO) return -1;
        }
        int esquerda = altura_negra_valida(no->esquerda, menor, &no->valor);
        int direita = altura_negra_valida(no->direita, &no->valor, maior);
        if (esquerda == -1 || direita == -1 || esquerda != direita) return -1;
        return esquerda + (no->cor == PRETO ? 1 : 0);
    }

    void desenhar(No* no, int nivel, std::ostringstream& saida) const {
        if (no == nil_) return;
        desenhar(no->direita, nivel + 1, saida);
        saida << std::string(5 * nivel, ' ') << no->valor << "[" << static_cast<char>(no->cor) << "]\n";
        desenhar(no->esquerda, nivel + 1, saida);
    }
};
