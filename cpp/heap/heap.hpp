// HEAP BINÁRIO (heap mínimo e heap máximo) + HEAPSORT
//
// - É uma árvore binária COMPLETA guardada num VETOR, sem ponteiros.
// - Heap MÍNIMO: todo pai é <= seus filhos -> o menor está na raiz.
//   Heap MÁXIMO: todo pai é >= seus filhos -> o maior está na raiz.
//
//            1                 vetor: [1, 3, 2, 7, 4, 5]
//          /   \                       0  1  2  3  4  5
//         3     2
//        / \   /             pai(i)             = (i - 1) / 2
//       7   4 5              filho_esquerdo(i)  = 2*i + 1
//                            filho_direito(i)   = 2*i + 2
//
// Operações
// - inserir: coloca no fim do vetor e SOBE (troca com o pai enquanto for menor).
// - remover: tira a raiz, põe o último elemento no lugar e DESCE (troca com o
//   menor filho enquanto for maior que ele).
// - construir (heapify): chama DESCER de trás para frente a partir do último pai.
//
// | operação          | custo      |
// |-------------------|------------|
// | topo              | O(1)       |
// | inserir / remover | O(log n)   |
// | construir         | O(n)       |  (sim, O(n), não O(n log n))
// | heapsort          | O(n log n) |
//
// Pega-ratão
// - Heap NÃO é árvore de busca: não dá para buscar um valor qualquer em O(log n).
// - Na STL o heap pronto é a `std::priority_queue` (que é um heap MÁXIMO).
#pragma once

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

// `VemAntes` é o critério de ordem: std::less -> heap mínimo; std::greater -> heap máximo.
template <typename T, typename VemAntes = std::less<T>>
class Heap {
public:
    Heap() = default;
    explicit Heap(std::vector<T> valores) : dados_(std::move(valores)) { construir(); }

    bool vazio() const { return dados_.empty(); }
    int tamanho() const { return static_cast<int>(dados_.size()); }

    const T& topo() const {
        if (vazio()) throw std::out_of_range("Heap vazio");
        return dados_[0];
    }

    void inserir(const T& valor) {
        dados_.push_back(valor);        // entra na primeira vaga livre
        subir(tamanho() - 1);           // e sobe até o lugar certo
    }

    // Remove e devolve o topo.
    T remover() {
        if (vazio()) throw std::out_of_range("Heap vazio");
        T topo = dados_[0];
        dados_[0] = dados_.back();  // o último vai para a raiz
        dados_.pop_back();
        if (!vazio()) descer(0);    // e desce até o lugar certo
        return topo;
    }

    // Confere a propriedade do heap em todos os pais.
    bool eh_valido() const {
        for (int i = 1; i < tamanho(); i++)
            if (vem_antes_(dados_[i], dados_[pai(i)])) return false;
        return true;
    }

    // O VETOR interno (ordem do heap, não ordenada).
    const std::vector<T>& para_vetor() const { return dados_; }

private:
    std::vector<T> dados_;
    VemAntes vem_antes_;

    // Fórmulas de navegação no vetor
    static int pai(int i) { return (i - 1) / 2; }
    static int esquerdo(int i) { return 2 * i + 1; }
    static int direito(int i) { return 2 * i + 2; }

    void subir(int i) {
        while (i > 0 && vem_antes_(dados_[i], dados_[pai(i)])) {
            std::swap(dados_[i], dados_[pai(i)]);
            i = pai(i);
        }
    }

    void descer(int i) {
        int n = tamanho();
        while (true) {
            int escolhido = i;
            if (esquerdo(i) < n && vem_antes_(dados_[esquerdo(i)], dados_[escolhido])) escolhido = esquerdo(i);
            if (direito(i) < n && vem_antes_(dados_[direito(i)], dados_[escolhido])) escolhido = direito(i);
            if (escolhido == i) break;  // nenhum filho deveria estar acima dele
            std::swap(dados_[i], dados_[escolhido]);
            i = escolhido;
        }
    }

    // Heapify: as folhas já são heaps; arruma de baixo para cima.
    void construir() {
        for (int i = tamanho() / 2 - 1; i >= 0; i--) descer(i);
    }
};

template <typename T>
using HeapMinimo = Heap<T, std::less<T>>;

template <typename T>
using HeapMaximo = Heap<T, std::greater<T>>;

// Ordena `vetor` NO PRÓPRIO VETOR, em ordem crescente -> O(n log n).
//  1. Transforma o vetor em heap MÁXIMO.
//  2. Repete: troca a raiz (o maior) com o último da parte não ordenada,
//     diminui essa parte em 1 e desce a nova raiz.
template <typename T>
void heapsort(std::vector<T>& vetor) {
    auto descer = [&vetor](int i, int tamanho) {
        while (true) {
            int maior = i;
            int esquerdo = 2 * i + 1, direito = 2 * i + 2;
            if (esquerdo < tamanho && vetor[esquerdo] > vetor[maior]) maior = esquerdo;
            if (direito < tamanho && vetor[direito] > vetor[maior]) maior = direito;
            if (maior == i) return;
            std::swap(vetor[i], vetor[maior]);
            i = maior;
        }
    };

    int n = static_cast<int>(vetor.size());
    for (int i = n / 2 - 1; i >= 0; i--) descer(i, n);
    for (int fim = n - 1; fim > 0; fim--) {
        std::swap(vetor[0], vetor[fim]);  // o maior vai para o final
        descer(0, fim);                   // arruma o resto
    }
}
