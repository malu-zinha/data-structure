// Testes das árvores (binária, ABB, AVL, rubro-negra).
//
// Além dos casos conhecidos, fazemos muitas inserções e remoções aleatórias e
// conferimos a cada passo que
//   - o percurso EM ORDEM continua igual ao de um std::set; e
//   - as INVARIANTES da estrutura continuam valendo (ABB válida, |FB| <= 1 na
//     AVL, as 5 regras na rubro-negra).
#include <cmath>
#include <optional>
#include <random>
#include <set>

#include "../tree/arvore_avl.hpp"
#include "../tree/arvore_binaria.hpp"
#include "../tree/arvore_binaria_busca.hpp"
#include "../tree/arvore_rubro_negra.hpp"
#include "teste.hpp"

using std::nullopt;
using Vetor = std::vector<int>;

// ================================================================ árvore binária
ArvoreBinaria<int> exemplo() { return ArvoreBinaria<int>::de_lista_por_nivel({1, 2, 3, 4, 5, nullopt, 7}); }

// Árvore com formato aleatório e valores 0..n-1.
ArvoreBinaria<int> arvore_aleatoria(std::mt19937& sorteio, int quantidade) {
    using No = ArvoreBinaria<int>::No;
    if (quantidade == 0) return ArvoreBinaria<int>();
    Vetor valores(quantidade);
    for (int i = 0; i < quantidade; i++) valores[i] = i;
    std::shuffle(valores.begin(), valores.end(), sorteio);
    std::vector<No*> nos{new No{valores[0]}};
    for (int i = 1; i < quantidade; i++) {
        while (true) {
            No* pai = nos[sorteio() % nos.size()];
            No*& lado = sorteio() % 2 ? pai->esquerda : pai->direita;
            if (lado == nullptr) {
                lado = new No{valores[i]};
                nos.push_back(lado);
                break;
            }
        }
    }
    return ArvoreBinaria<int>(nos[0]);
}

TESTE(binaria_percursos) {
    auto arvore = exemplo();
    VERIFICAR_IGUAL(arvore.pre_ordem(), (Vetor{1, 2, 4, 5, 3, 7}));
    VERIFICAR_IGUAL(arvore.em_ordem(), (Vetor{4, 2, 5, 1, 3, 7}));
    VERIFICAR_IGUAL(arvore.pos_ordem(), (Vetor{4, 5, 2, 7, 3, 1}));
    VERIFICAR_IGUAL(arvore.em_nivel(), (Vetor{1, 2, 3, 4, 5, 7}));
    VERIFICAR_IGUAL(arvore.por_niveis(), (std::vector<Vetor>{{1}, {2, 3}, {4, 5, 7}}));
}

TESTE(binaria_medidas) {
    auto arvore = exemplo();
    VERIFICAR_IGUAL(arvore.tamanho(), 6);
    VERIFICAR_IGUAL(arvore.altura(), 2);
    VERIFICAR_IGUAL(arvore.contar_folhas(), 3);
    VERIFICAR_IGUAL(arvore.contar_nos_internos(), 3);
    VERIFICAR_IGUAL(arvore.nivel(1), 0);
    VERIFICAR_IGUAL(arvore.nivel(5), 2);
    VERIFICAR_IGUAL(arvore.nivel(99), -1);
    VERIFICAR(arvore.contem(7));
    VERIFICAR(!arvore.contem(6));
}

TESTE(binaria_vazia) {
    ArvoreBinaria<int> vazia;
    VERIFICAR_IGUAL(vazia.tamanho(), 0);
    VERIFICAR_IGUAL(vazia.altura(), -1);
    VERIFICAR(vazia.pre_ordem().empty());
    VERIFICAR(vazia.em_ordem_iterativa().empty());
    VERIFICAR(vazia.eh_completa());
    VERIFICAR(!vazia.remover(1));
}

TESTE(binaria_classificacoes) {
    auto arvore = exemplo();
    VERIFICAR(!arvore.eh_estritamente_binaria());
    VERIFICAR(!arvore.eh_completa());
    VERIFICAR(!arvore.eh_cheia());
    auto cheia = ArvoreBinaria<int>::de_lista_por_nivel({1, 2, 3, 4, 5, 6, 7});
    VERIFICAR(cheia.eh_estritamente_binaria() && cheia.eh_completa() && cheia.eh_cheia());
    auto completa = ArvoreBinaria<int>::de_lista_por_nivel({1, 2, 3, 4});
    VERIFICAR(completa.eh_completa() && !completa.eh_cheia() && !completa.eh_estritamente_binaria());
}

TESTE(binaria_percursos_iterativos_batem_com_os_recursivos) {
    std::mt19937 sorteio(2);
    for (int rodada = 0; rodada < 200; rodada++) {
        auto arvore = arvore_aleatoria(sorteio, sorteio() % 30);
        VERIFICAR_IGUAL(arvore.pre_ordem_iterativa(), arvore.pre_ordem());
        VERIFICAR_IGUAL(arvore.em_ordem_iterativa(), arvore.em_ordem());
        VERIFICAR_IGUAL(arvore.pos_ordem_iterativa(), arvore.pos_ordem());
    }
}

TESTE(binaria_espelhar_inverte_o_em_ordem) {
    std::mt19937 sorteio(3);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto arvore = arvore_aleatoria(sorteio, sorteio() % 20);
        auto antes = arvore.em_ordem();
        arvore.espelhar();
        std::reverse(antes.begin(), antes.end());
        VERIFICAR_IGUAL(arvore.em_ordem(), antes);
    }
}

TESTE(binaria_reconstruir_a_partir_dos_percursos) {
    std::mt19937 sorteio(4);
    for (int rodada = 0; rodada < 200; rodada++) {
        auto arvore = arvore_aleatoria(sorteio, sorteio() % 25);
        auto copia = ArvoreBinaria<int>::de_percursos(arvore.pre_ordem(), arvore.em_ordem());
        VERIFICAR(copia.igual_a(arvore));
    }
    VERIFICAR_LANCA(ArvoreBinaria<int>::de_percursos({1, 2}, {1, 3}), std::invalid_argument);
    VERIFICAR_LANCA(ArvoreBinaria<int>::de_percursos({1, 1}, {1, 1}), std::invalid_argument);
}

TESTE(binaria_inserir_por_nivel_mantem_completa) {
    ArvoreBinaria<int> arvore;
    for (int valor = 1; valor <= 20; valor++) {
        arvore.inserir_por_nivel(valor);
        VERIFICAR(arvore.eh_completa());
    }
    Vetor esperado;
    for (int valor = 1; valor <= 20; valor++) esperado.push_back(valor);
    VERIFICAR_IGUAL(arvore.em_nivel(), esperado);
}

TESTE(binaria_remover) {
    std::mt19937 sorteio(5);
    for (int rodada = 0; rodada < 100; rodada++) {
        int n = sorteio() % 20 + 1;
        auto arvore = arvore_aleatoria(sorteio, n);
        std::multiset<int> modelo;
        for (int v : arvore.em_ordem()) modelo.insert(v);
        for (int passo = 0; passo < n; passo++) {
            int valor = sorteio() % (n + 3);
            bool existe = modelo.count(valor) > 0;
            VERIFICAR_IGUAL(arvore.remover(valor), existe);
            if (existe) modelo.erase(modelo.find(valor));
            auto restantes = arvore.em_ordem();
            VERIFICAR_IGUAL(std::multiset<int>(restantes.begin(), restantes.end()), modelo);
        }
    }
}

// ================================================================ árvores de busca
// ABB, AVL e rubro-negra passam pelo mesmo contrato.
template <typename Arvore>
void contrato_arvore_de_busca(unsigned semente) {
    Arvore vazia;
    VERIFICAR(vazia.vazia());
    VERIFICAR_LANCA(vazia.minimo(), std::invalid_argument);
    VERIFICAR_LANCA(vazia.maximo(), std::invalid_argument);
    VERIFICAR(!vazia.remover(1));

    Arvore arvore{50, 30, 70, 20, 40, 60, 80};
    VERIFICAR(!arvore.inserir(40));  // repetido
    VERIFICAR_IGUAL(arvore.em_ordem(), (Vetor{20, 30, 40, 50, 60, 70, 80}));
    VERIFICAR(arvore.contem(60) && !arvore.contem(65));
    VERIFICAR_IGUAL(arvore.minimo(), 20);
    VERIFICAR_IGUAL(arvore.maximo(), 80);
    VERIFICAR(arvore.remover(20));  // folha
    VERIFICAR(arvore.remover(30));  // um filho
    VERIFICAR(arvore.remover(70));  // dois filhos
    VERIFICAR_IGUAL(arvore.em_ordem(), (Vetor{40, 50, 60, 80}));
    VERIFICAR(arvore.eh_valida());

    std::mt19937 sorteio(semente);
    Arvore aleatoria;
    std::set<int> modelo;
    for (int passo = 0; passo < 3000; passo++) {
        int valor = sorteio() % 200;
        if (sorteio() % 3 != 0) {
            VERIFICAR_IGUAL(aleatoria.inserir(valor), modelo.insert(valor).second);
        } else {
            VERIFICAR_IGUAL(aleatoria.remover(valor), modelo.erase(valor) == 1);
        }
        VERIFICAR_IGUAL(aleatoria.tamanho(), static_cast<int>(modelo.size()));
        if (passo % 10 == 0) {
            VERIFICAR(aleatoria.eh_valida());
            VERIFICAR_IGUAL(aleatoria.em_ordem(), Vetor(modelo.begin(), modelo.end()));
        }
    }
}

TESTE(abb_contrato) { contrato_arvore_de_busca<ArvoreBinariaBusca<int>>(10); }
TESTE(avl_contrato) { contrato_arvore_de_busca<ArvoreAVL<int>>(11); }
TESTE(rubro_negra_contrato) { contrato_arvore_de_busca<ArvoreRubroNegra<int>>(12); }

TESTE(abb_insercao_recursiva_gera_a_mesma_arvore) {
    ArvoreBinariaBusca<int> iterativa, recursiva;
    for (int valor : {50, 30, 70, 20, 40, 60, 80, 35, 45, 30}) {
        VERIFICAR_IGUAL(iterativa.inserir(valor), recursiva.inserir_recursivo(valor));
    }
    VERIFICAR_IGUAL(iterativa.pre_ordem(), recursiva.pre_ordem());
    VERIFICAR(recursiva.buscar(35) && !recursiva.buscar(36));
}

TESTE(abb_sucessor_antecessor_k_esimo_e_intervalo) {
    ArvoreBinariaBusca<int> abb{50, 30, 70, 20, 40, 60, 80, 35, 45};
    VERIFICAR(abb.sucessor(45) == 50);
    VERIFICAR(abb.sucessor(80) == nullopt);
    VERIFICAR(abb.antecessor(50) == 45);
    VERIFICAR(abb.antecessor(20) == nullopt);
    VERIFICAR(abb.sucessor(36) == 40);  // funciona mesmo para valores fora da árvore
    VERIFICAR_IGUAL(abb.k_esimo_menor(1), 20);
    VERIFICAR_IGUAL(abb.k_esimo_menor(9), 80);
    VERIFICAR_LANCA(abb.k_esimo_menor(10), std::out_of_range);
    VERIFICAR_IGUAL(abb.intervalo(33, 65), (Vetor{35, 40, 45, 50, 60}));
    VERIFICAR_IGUAL(abb.ancestral_comum(35, 45), 40);
    VERIFICAR_IGUAL(abb.ancestral_comum(20, 80), 50);
    VERIFICAR_IGUAL(abb.ancestral_comum(30, 35), 30);
    VERIFICAR_LANCA(abb.ancestral_comum(35, 99), std::invalid_argument);
    VERIFICAR_IGUAL(abb.nivel(45), 3);
    VERIFICAR_IGUAL(abb.contar_folhas(), 5);
}

TESTE(abb_pior_caso_vira_lista) {
    ArvoreBinariaBusca<int> torta;
    for (int valor = 1; valor <= 50; valor++) torta.inserir(valor);
    VERIFICAR_IGUAL(torta.altura(), 49);
}

TESTE(avl_rotacoes_dos_quatro_casos) {
    struct Caso {
        Vetor valores;
        std::vector<std::string> rotacoes;
    };
    for (const Caso& caso : {Caso{{30, 20, 10}, {"direita em 30"}},                    // LL
                             Caso{{10, 20, 30}, {"esquerda em 10"}},                   // RR
                             Caso{{30, 10, 20}, {"esquerda em 10", "direita em 30"}},  // LR
                             Caso{{10, 30, 20}, {"direita em 30", "esquerda em 10"}}}) {  // RL
        ArvoreAVL<int> avl;
        for (int valor : caso.valores) avl.inserir(valor);
        VERIFICAR_IGUAL(avl.rotacoes, caso.rotacoes);
        VERIFICAR_IGUAL(avl.pre_ordem(), (Vetor{20, 10, 30}));
    }
}

TESTE(avl_altura_fica_logaritmica) {
    ArvoreAVL<int> avl;
    for (int valor = 1; valor <= 1000; valor++) avl.inserir(valor);
    VERIFICAR(avl.altura() <= 1.45 * std::log2(1002));
    VERIFICAR(avl.eh_valida());
}

TESTE(avl_fator_de_balanceamento) {
    ArvoreAVL<int> avl{20, 10, 30, 5};
    VERIFICAR_IGUAL(avl.fator_balanceamento(20), 1);
    VERIFICAR_IGUAL(avl.fator_balanceamento(10), 1);
    VERIFICAR_IGUAL(avl.fator_balanceamento(5), 0);
    VERIFICAR_LANCA(avl.fator_balanceamento(99), std::invalid_argument);
}

TESTE(rubro_negra_raiz_preta_e_altura_limitada) {
    ArvoreRubroNegra<int> arvore;
    for (int valor = 1; valor <= 1000; valor++) arvore.inserir(valor);
    VERIFICAR(arvore.cor_de(arvore.em_nivel()[0]) == ArvoreRubroNegra<int>::PRETO);
    VERIFICAR(arvore.altura() + 1 <= 2 * std::log2(1001));
    VERIFICAR(arvore.eh_valida());
}

int main() { return rodar_testes(); }
