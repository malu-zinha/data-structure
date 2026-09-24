#include <iostream>

#include "../util.hpp"
#include "lista_sequencial.hpp"

int main() {
    titulo("Lista sequencial");
    ListaSequencial<int> lista(5);
    lista.inserir_fim(10);
    lista.inserir_fim(30);
    lista.inserir_inicio(5);
    lista.inserir_posicao(2, 20);
    std::cout << "Lista: " << em_colchetes(lista.para_vetor()) << " | tamanho: " << lista.tamanho()
              << " | capacidade: " << lista.capacidade() << "\n";
    std::cout << "Posição do 20: " << lista.buscar(20) << "\n";

    int removido = lista.remover_inicio();
    std::cout << "Removido do início: " << removido << " -> " << em_colchetes(lista.para_vetor()) << "\n";
    removido = lista.remover_posicao(1);
    std::cout << "Removido da posição 1: " << removido << " -> " << em_colchetes(lista.para_vetor()) << "\n";

    lista.inverter();
    std::cout << "Invertida: " << em_colchetes(lista.para_vetor()) << "\n";

    lista.inserir_fim(1);
    lista.inserir_fim(2);
    lista.inserir_fim(3);
    std::cout << "Cheia? " << (lista.cheia() ? "sim" : "não") << " -> " << em_colchetes(lista.para_vetor()) << "\n";
    try {
        lista.inserir_fim(99);
    } catch (const std::overflow_error& erro) {
        std::cout << "Erro esperado: " << erro.what() << "\n";
    }
}
