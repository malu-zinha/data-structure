#include <iostream>

#include "../util.hpp"
#include "lista_circular.hpp"
#include "lista_circular_dupla.hpp"
#include "lista_duplamente_encadeada.hpp"
#include "lista_encadeada.hpp"
#include "lista_heterogenea.hpp"
#include "lista_ordenada.hpp"

template <typename Lista>
std::string encadeada(const Lista& lista) {
    return lista.vazia() ? "nullptr" : juntar(lista.para_vetor(), " -> ") + " -> nullptr";
}

template <typename Lista>
std::string circular(const Lista& lista, const std::string& seta) {
    if (lista.vazia()) return "(vazia)";
    return juntar(lista.para_vetor(), seta) + seta + "(volta ao " + std::to_string(lista.obter(0)) + ")";
}

int main() {
    titulo("Lista encadeada");
    ListaEncadeada<int> lista;
    lista.inserir_fim(20);
    lista.inserir_fim(30);
    lista.inserir_inicio(10);
    lista.inserir_posicao(3, 40);
    std::cout << "Lista: " << encadeada(lista) << " | tamanho: " << lista.tamanho() << "\n";
    std::cout << "Posição do 30: " << lista.buscar(30) << " | elemento na posição 1: " << lista.obter(1) << "\n";
    int removido = lista.remover_inicio();
    std::cout << "Removido do início: " << removido << " | lista: " << encadeada(lista) << "\n";
    removido = lista.remover_fim();
    std::cout << "Removido do fim: " << removido << " | lista: " << encadeada(lista) << "\n";
    lista.inserir_fim(50);
    lista.inserir_fim(60);
    lista.remover_valor(50);
    std::cout << "Sem o 50: " << encadeada(lista) << "\n";
    lista.inverter();
    std::cout << "Invertida: " << encadeada(lista) << "\n";

    titulo("Lista duplamente encadeada");
    ListaDuplamenteEncadeada<int> dupla;
    for (int valor : {10, 20, 30}) dupla.inserir_fim(valor);
    dupla.inserir_inicio(5);
    dupla.inserir_posicao(2, 15);
    std::cout << "Lista: nullptr <- " << juntar(dupla.para_vetor(), " <-> ") << " -> nullptr\n";
    std::cout << "De trás para frente: " << em_colchetes(dupla.para_vetor_reverso()) << "\n";
    removido = dupla.remover_fim();
    std::cout << "Removido do fim: " << removido << " -> " << em_colchetes(dupla.para_vetor()) << "\n";
    removido = dupla.remover_posicao(1);
    std::cout << "Removido da posição 1: " << removido << " -> " << em_colchetes(dupla.para_vetor()) << "\n";
    dupla.inverter();
    std::cout << "Invertida: " << em_colchetes(dupla.para_vetor()) << "\n";

    titulo("Lista circular");
    ListaCircular<int> roda;
    for (int valor : {10, 20, 30}) roda.inserir_fim(valor);
    roda.inserir_inicio(5);
    std::cout << "Lista: " << circular(roda, " -> ") << "\n";
    roda.rotacionar(1);
    std::cout << "Rotacionada 1x: " << circular(roda, " -> ") << "\n";
    removido = roda.remover_fim();
    std::cout << "Removido do fim: " << removido << " | lista: " << circular(roda, " -> ") << "\n";
    roda.inverter();
    std::cout << "Invertida: " << circular(roda, " -> ") << "\n";
    auto [ordem, sobrevivente] = problema_de_josephus(7, 3);
    std::cout << "Josephus(7, 3): eliminados " << em_colchetes(ordem) << ", sobrevivente " << sobrevivente << "\n";

    titulo("Lista circular dupla");
    ListaCircularDupla<int> circular_dupla;
    for (int valor : {10, 20, 30, 40}) circular_dupla.inserir_fim(valor);
    std::cout << "Lista: " << circular(circular_dupla, " <-> ") << "\n";
    std::cout << "Fim (anterior do início): " << circular_dupla.ultimo() << "\n";
    std::cout << "De trás para frente: " << em_colchetes(circular_dupla.para_vetor_reverso()) << "\n";
    circular_dupla.rotacionar(-1);
    std::cout << "Rotacionada para trás: " << circular(circular_dupla, " <-> ") << "\n";
    circular_dupla.inverter();
    std::cout << "Invertida: " << circular(circular_dupla, " <-> ") << "\n";

    titulo("Lista ordenada");
    ListaOrdenada<int> ordenada;
    for (int valor : {40, 10, 30, 20, 25}) {
        ordenada.inserir(valor);
        std::cout << "inseriu " << valor << ": " << encadeada(ordenada) << "\n";
    }
    std::cout << "Posição do 30: " << ordenada.buscar(30) << " | posição do 35: " << ordenada.buscar(35) << "\n";
    std::cout << "Mínimo: " << ordenada.minimo() << " | máximo: " << ordenada.maximo() << "\n";
    ordenada.remover(25);
    ListaOrdenada<int> outra{5, 15, 35, 50};
    std::cout << "Mesclando " << encadeada(ordenada) << " com " << encadeada(outra) << "\n  -> "
              << encadeada(ordenada.mesclar(outra)) << "\n";

    titulo("Lista heterogênea");
    ListaHeterogenea hetero;
    hetero.inserir(10);
    hetero.inserir(3.14);
    hetero.inserir('a');
    hetero.inserir("estrutura");
    hetero.inserir(7);
    std::cout << hetero.para_texto() << "\n";
    auto contagem = hetero.contar_por_tipo();
    std::cout << "Inteiros: " << contagem[0] << " | reais: " << contagem[1] << " | caracteres: " << contagem[2]
              << " | textos: " << contagem[3] << "\n";
    std::cout << "Só inteiros: " << em_colchetes(hetero.filtrar(Tipo::INTEIRO)) << "\n";
    std::cout << "Soma dos números: " << hetero.somar_numeros() << "\n";
    auto [tipo, valor] = hetero.remover_inicio();
    std::cout << "Removido: (" << nome_do_tipo(tipo) << ", " << valor << ") | lista: " << hetero.para_texto() << "\n";
}
