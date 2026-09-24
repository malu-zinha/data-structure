#include <iostream>

#include "../util.hpp"
#include "heap.hpp"

int main() {
    titulo("Heap mínimo");
    HeapMinimo<int> heap;
    for (int valor : {7, 3, 9, 1, 4, 2}) {
        heap.inserir(valor);
        std::cout << "inseriu " << valor << ": vetor = " << em_colchetes(heap.para_vetor()) << "\n";
    }
    std::vector<int> saida;
    while (!heap.vazio()) saida.push_back(heap.remover());
    std::cout << "Removendo em ordem: " << em_colchetes(saida) << "\n";

    titulo("Heap máximo");
    HeapMaximo<int> maximo({7, 3, 9, 1, 4, 2});
    std::cout << "Construído com heapify: " << em_colchetes(maximo.para_vetor()) << " | topo: " << maximo.topo()
              << "\n";

    titulo("Heapsort");
    std::vector<int> vetor{5, 2, 9, 1, 5, 6};
    std::cout << em_colchetes(vetor);
    heapsort(vetor);
    std::cout << " -> " << em_colchetes(vetor) << "\n";
}
