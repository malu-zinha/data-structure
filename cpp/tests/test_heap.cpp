// Testes do heap mínimo, do heap máximo e do heapsort.
#include <algorithm>
#include <functional>
#include <random>

#include "../heap/heap.hpp"
#include "teste.hpp"

std::vector<int> aleatorios(std::mt19937& sorteio, int maximo_tamanho) {
    std::vector<int> valores(sorteio() % (maximo_tamanho + 1));
    for (int& valor : valores) valor = sorteio() % 100;
    return valores;
}

TESTE(heap_minimo_devolve_em_ordem_crescente) {
    std::mt19937 sorteio(88);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto valores = aleatorios(sorteio, 30);
        HeapMinimo<int> heap;
        for (int valor : valores) {
            heap.inserir(valor);
            VERIFICAR(heap.eh_valido());
        }
        std::vector<int> saida;
        while (!heap.vazio()) saida.push_back(heap.remover());
        std::sort(valores.begin(), valores.end());
        VERIFICAR_IGUAL(saida, valores);
    }
}

TESTE(heap_maximo_devolve_em_ordem_decrescente) {
    std::mt19937 sorteio(89);
    auto valores = aleatorios(sorteio, 40);
    HeapMaximo<int> heap(valores);  // construção com heapify
    VERIFICAR(heap.eh_valido());
    std::vector<int> saida;
    while (!heap.vazio()) saida.push_back(heap.remover());
    std::sort(valores.begin(), valores.end(), std::greater<int>());
    VERIFICAR_IGUAL(saida, valores);
}

TESTE(construir_com_heapify) {
    std::mt19937 sorteio(90);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto valores = aleatorios(sorteio, 25);
        HeapMinimo<int> heap(valores);
        VERIFICAR(heap.eh_valido());
        VERIFICAR_IGUAL(heap.tamanho(), static_cast<int>(valores.size()));
        if (!valores.empty()) VERIFICAR_IGUAL(heap.topo(), *std::min_element(valores.begin(), valores.end()));
    }
}

TESTE(heap_vazio) {
    HeapMinimo<int> heap;
    VERIFICAR(heap.vazio());
    VERIFICAR_LANCA(heap.remover(), std::out_of_range);
    VERIFICAR_LANCA(heap.topo(), std::out_of_range);
}

TESTE(heapsort_ordena_no_proprio_vetor) {
    std::mt19937 sorteio(91);
    for (int rodada = 0; rodada < 200; rodada++) {
        auto valores = aleatorios(sorteio, 40);
        auto esperado = valores;
        std::sort(esperado.begin(), esperado.end());
        heapsort(valores);
        VERIFICAR_IGUAL(valores, esperado);
    }
}

int main() { return rodar_testes(); }
