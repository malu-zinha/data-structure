#include <iostream>
#include <optional>

#include "../util.hpp"
#include "arvore_avl.hpp"
#include "arvore_binaria.hpp"
#include "arvore_binaria_busca.hpp"
#include "arvore_rubro_negra.hpp"

template <typename T>
std::string opcional(const std::optional<T>& valor) {
    return valor ? std::to_string(*valor) : "nenhum";
}

int main() {
    titulo("Árvore binária");
    auto arvore = ArvoreBinaria<int>::de_lista_por_nivel({1, 2, 3, 4, 5, std::nullopt, 7});
    std::cout << arvore.desenho();
    std::cout << "Pré-ordem: " << em_colchetes(arvore.pre_ordem())
              << " | iterativa: " << em_colchetes(arvore.pre_ordem_iterativa()) << "\n";
    std::cout << "Em ordem:  " << em_colchetes(arvore.em_ordem())
              << " | iterativa: " << em_colchetes(arvore.em_ordem_iterativa()) << "\n";
    std::cout << "Pós-ordem: " << em_colchetes(arvore.pos_ordem())
              << " | iterativa: " << em_colchetes(arvore.pos_ordem_iterativa()) << "\n";
    std::cout << "Em nível:  " << em_colchetes(arvore.em_nivel()) << "\n";
    std::cout << "Tamanho: " << arvore.tamanho() << " | altura: " << arvore.altura()
              << " | folhas: " << arvore.contar_folhas() << " | internos: " << arvore.contar_nos_internos() << "\n";
    std::cout << "Nível do 5: " << arvore.nivel(5) << "\n";
    std::cout << "Estritamente binária? " << (arvore.eh_estritamente_binaria() ? "sim" : "não")
              << " | completa? " << (arvore.eh_completa() ? "sim" : "não")
              << " | cheia? " << (arvore.eh_cheia() ? "sim" : "não") << "\n";
    auto copia = ArvoreBinaria<int>::de_percursos(arvore.pre_ordem(), arvore.em_ordem());
    std::cout << "Reconstruída a partir dos percursos é igual? " << (copia.igual_a(arvore) ? "sim" : "não") << "\n";
    arvore.espelhar();
    std::cout << "Espelhada (em ordem): " << em_colchetes(arvore.em_ordem()) << "\n";

    titulo("Árvore binária de busca");
    ArvoreBinariaBusca<int> abb{50, 30, 70, 20, 40, 60, 80, 35, 45};
    std::cout << abb.desenho();
    std::cout << "Em ordem (ordenado): " << em_colchetes(abb.em_ordem()) << "\n";
    std::cout << "Pré-ordem: " << em_colchetes(abb.pre_ordem()) << "\n";
    std::cout << "Em nível: " << em_colchetes(abb.em_nivel()) << "\n";
    std::cout << "Mínimo: " << abb.minimo() << " | máximo: " << abb.maximo() << " | altura: " << abb.altura() << "\n";
    std::cout << "Sucessor de 45: " << opcional(abb.sucessor(45))
              << " | antecessor de 50: " << opcional(abb.antecessor(50)) << "\n";
    std::cout << "3º menor: " << abb.k_esimo_menor(3) << " | valores em [33, 65]: " << em_colchetes(abb.intervalo(33, 65))
              << "\n";
    std::cout << "Ancestral comum de 35 e 45: " << abb.ancestral_comum(35, 45) << "\n";
    abb.remover(20);  // caso 1: folha
    abb.remover(40);  // caso 3: dois filhos (35 e 45)
    abb.remover(30);  // caso 2: um filho
    std::cout << "Depois de remover 20, 40 e 30:\n" << abb.desenho();
    std::cout << "Ainda é ABB válida? " << (abb.eh_valida() ? "sim" : "não") << "\n";
    ArvoreBinariaBusca<int> torta{1, 2, 3, 4, 5, 6, 7};
    std::cout << "Inserindo 1..7 em ordem, a altura fica " << torta.altura() << " (virou uma lista!)\n";

    titulo("Árvore AVL");
    ArvoreAVL<int> avl;
    for (int valor : {10, 20, 30, 40, 50, 25}) {
        avl.rotacoes.clear();
        avl.inserir(valor);
        std::cout << "inseriu " << valor << " | rotações: "
                  << (avl.rotacoes.empty() ? "nenhuma" : juntar(avl.rotacoes)) << "\n";
    }
    std::cout << avl.desenho();
    std::cout << "Em ordem: " << em_colchetes(avl.em_ordem()) << " | altura: " << avl.altura()
              << " | válida? " << (avl.eh_valida() ? "sim" : "não") << "\n";
    for (int valor : {40, 50}) {
        avl.rotacoes.clear();
        avl.remover(valor);
        std::cout << "removeu " << valor << " | rotações: "
                  << (avl.rotacoes.empty() ? "nenhuma" : juntar(avl.rotacoes)) << "\n";
    }
    std::cout << avl.desenho();
    ArvoreAVL<int> grande;
    for (int valor = 1; valor <= 1000; valor++) grande.inserir(valor);
    std::cout << "1000 valores inseridos EM ORDEM -> altura " << grande.altura() << " (uma ABB comum teria 999)\n";

    titulo("Árvore rubro-negra");
    ArvoreRubroNegra<int> rubro_negra{10, 20, 30, 15, 25, 5, 1};
    std::cout << rubro_negra.desenho();
    std::cout << "Em ordem: " << em_colchetes(rubro_negra.em_ordem()) << "\n";
    std::cout << "Altura: " << rubro_negra.altura() << " | altura negra: " << rubro_negra.altura_negra()
              << " | válida? " << (rubro_negra.eh_valida() ? "sim" : "não") << "\n";
    rubro_negra.remover(20);
    rubro_negra.remover(10);
    std::cout << "Depois de remover 20 e 10:\n" << rubro_negra.desenho();
    std::cout << "Válida? " << (rubro_negra.eh_valida() ? "sim" : "não") << "\n";
    ArvoreRubroNegra<int> grande_rn;
    for (int valor = 1; valor <= 1000; valor++) grande_rn.inserir(valor);
    std::cout << "1000 valores inseridos EM ORDEM -> altura " << grande_rn.altura() << "\n";
}
