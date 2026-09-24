// Testes dos grafos.
//
// Além de exemplos conhecidos, geramos grafos aleatórios e comparamos cada
// algoritmo com outro jeito de chegar à mesma resposta (força bruta, ou outro
// algoritmo que resolve o mesmo problema).
#include <algorithm>
#include <random>
#include <set>
#include <string>

#include "../graphs/arvore_geradora_minima.hpp"
#include "../graphs/buscas.hpp"
#include "../graphs/caminhos_minimos.hpp"
#include "../graphs/grafo_lista.hpp"
#include "../graphs/grafo_matriz.hpp"
#include "../graphs/propriedades.hpp"
#include "teste.hpp"

using Texto = std::vector<std::string>;

// Mesmo grafo aleatório nas duas representações, com vértices 0..n-1.
std::pair<GrafoLista<int>, GrafoMatriz> grafo_aleatorio(std::mt19937& sorteio, int n, int chance_em_10, bool dirigido,
                                                         int peso_minimo = 1, int peso_maximo = 9) {
    GrafoLista<int> lista(dirigido);
    GrafoMatriz matriz(n, dirigido);
    for (int v = 0; v < n; v++) lista.adicionar_vertice(v);
    for (int u = 0; u < n; u++) {
        for (int v = dirigido ? 0 : u + 1; v < n; v++) {
            if (u == v || static_cast<int>(sorteio() % 10) >= chance_em_10) continue;
            int peso = peso_minimo + static_cast<int>(sorteio() % (peso_maximo - peso_minimo + 1));
            lista.adicionar_aresta(u, v, peso);
            matriz.adicionar_aresta(u, v, peso);
        }
    }
    return {std::move(lista), matriz};
}

// Quais vértices `origem` alcança (força bruta simples, sem BFS/DFS da biblioteca).
std::set<int> alcancaveis(const GrafoMatriz& grafo, int origem) {
    std::set<int> vistos{origem};
    bool mudou = true;
    while (mudou) {
        mudou = false;
        for (int u : std::set<int>(vistos))
            for (const auto& [v, _] : grafo.vizinhos(u)) mudou |= vistos.insert(v).second;
    }
    return vistos;
}

// ================================================================ representações
TESTE(lista_de_adjacencia_basica) {
    GrafoLista<std::string> grafo;
    grafo.adicionar_aresta("A", "B", 4);
    grafo.adicionar_aresta("A", "C", 2);
    grafo.adicionar_aresta("B", "C", 1);
    grafo.adicionar_aresta("C", "C", 5);  // laço
    grafo.adicionar_vertice("D");
    VERIFICAR_IGUAL(grafo.vertices(), (Texto{"A", "B", "C", "D"}));
    VERIFICAR(grafo.existe_aresta("B", "A"));  // não dirigido: os dois sentidos
    VERIFICAR_IGUAL(grafo.peso("C", "A"), 2.0);
    VERIFICAR_IGUAL(grafo.numero_arestas(), 4);
    VERIFICAR_IGUAL(grafo.grau("C"), 4);  // A, B e o laço (que conta 2)
    VERIFICAR_IGUAL(grafo.grau("D"), 0);
    grafo.adicionar_aresta("A", "B", 9);  // atualiza o peso, não duplica
    VERIFICAR_IGUAL(grafo.peso("B", "A"), 9.0);
    VERIFICAR_IGUAL(grafo.numero_arestas(), 4);
    grafo.remover_aresta("A", "B");
    VERIFICAR(!grafo.existe_aresta("B", "A"));
    grafo.remover_vertice("C");
    VERIFICAR_IGUAL(grafo.numero_arestas(), 0);
    VERIFICAR_LANCA(grafo.vizinhos("C"), std::out_of_range);
    VERIFICAR_LANCA(grafo.peso("A", "B"), std::out_of_range);
}

TESTE(digrafo_graus_e_transposto) {
    GrafoLista<std::string> grafo(true);
    grafo.adicionar_aresta("A", "B");
    grafo.adicionar_aresta("C", "A");
    grafo.adicionar_aresta("A", "D");
    VERIFICAR(!grafo.existe_aresta("B", "A"));
    VERIFICAR_IGUAL(grafo.grau_saida("A"), 2);
    VERIFICAR_IGUAL(grafo.grau_entrada("A"), 1);
    VERIFICAR_IGUAL(grafo.grau("A"), 3);
    auto transposto = grafo.transposto();
    VERIFICAR(transposto.existe_aresta("B", "A") && transposto.existe_aresta("A", "C"));
    VERIFICAR(!transposto.existe_aresta("A", "B"));
}

TESTE(matriz_de_adjacencia_basica) {
    GrafoMatriz grafo(3);
    grafo.adicionar_aresta(0, 1, 0);  // peso 0 é uma aresta válida
    VERIFICAR(grafo.existe_aresta(1, 0));
    VERIFICAR_IGUAL(grafo.peso(0, 1), 0.0);
    VERIFICAR_IGUAL(grafo.adicionar_vertice(), 3);
    grafo.adicionar_aresta(3, 2, 7);
    VERIFICAR_IGUAL(grafo.grau(2), 1);
    VERIFICAR_IGUAL(grafo.numero_arestas(), 2);
    VERIFICAR_LANCA(grafo.adicionar_aresta(0, 9), std::out_of_range);
    grafo.remover_aresta(0, 1);
    VERIFICAR_LANCA(grafo.remover_aresta(0, 1), std::out_of_range);
}

TESTE(as_duas_representacoes_concordam) {
    std::mt19937 sorteio(1);
    for (int rodada = 0; rodada < 50; rodada++) {
        bool dirigido = rodada % 2;
        auto [lista, matriz] = grafo_aleatorio(sorteio, 8, 3, dirigido);
        VERIFICAR_IGUAL(lista.numero_arestas(), matriz.numero_arestas());
        for (int v = 0; v < 8; v++) {
            VERIFICAR_IGUAL(lista.grau(v), matriz.grau(v));
            VERIFICAR_IGUAL(bfs(lista, v).size(), bfs(matriz, v).size());
            VERIFICAR_IGUAL(bfs_distancias(lista, v).first, bfs_distancias(matriz, v).first);
        }
    }
}

// ================================================================ buscas
TESTE(bfs_e_dfs_no_exemplo) {
    GrafoLista<std::string> grafo;
    for (auto [u, v] : {std::pair{"A", "B"}, {"A", "C"}, {"B", "D"}, {"C", "D"}, {"D", "E"}, {"E", "F"}, {"C", "F"}})
        grafo.adicionar_aresta(u, v);
    VERIFICAR_IGUAL(bfs(grafo, "A"), (Texto{"A", "B", "C", "D", "F", "E"}));
    VERIFICAR_IGUAL(dfs(grafo, "A"), (Texto{"A", "B", "D", "C", "F", "E"}));
    VERIFICAR_IGUAL(menor_caminho_bfs(grafo, "A", "F"), (Texto{"A", "C", "F"}));
    VERIFICAR_IGUAL(todos_os_caminhos(grafo, "A", "F").size(), 4u);
    VERIFICAR_LANCA(bfs(grafo, "Z"), std::out_of_range);
}

TESTE(dfs_iterativa_igual_a_recursiva_e_alcance_certo) {
    std::mt19937 sorteio(2);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto [lista, matriz] = grafo_aleatorio(sorteio, 9, 2, rodada % 2);
        for (int v = 0; v < 9; v++) {
            VERIFICAR_IGUAL(dfs_iterativa(lista, v), dfs(lista, v));
            auto visitados = bfs(lista, v);
            VERIFICAR_IGUAL(std::set<int>(visitados.begin(), visitados.end()), alcancaveis(matriz, v));
            VERIFICAR_IGUAL(existe_caminho(lista, v, 0), alcancaveis(matriz, v).count(0) == 1);
        }
    }
}

TESTE(tempos_da_dfs_formam_parenteses) {
    std::mt19937 sorteio(3);
    auto [lista, _] = grafo_aleatorio(sorteio, 12, 2, true);
    auto tempos = dfs_completa(lista);
    VERIFICAR_IGUAL(tempos.descoberta.size(), 12u);
    for (int u = 0; u < 12; u++) {
        VERIFICAR(tempos.descoberta[u] < tempos.finalizacao[u]);
        for (int v = 0; v < 12; v++) {  // intervalos nunca se cruzam: ou um contém o outro, ou são disjuntos
            int du = tempos.descoberta[u], fu = tempos.finalizacao[u];
            int dv = tempos.descoberta[v], fv = tempos.finalizacao[v];
            VERIFICAR(!(du < dv && dv < fu && fu < fv));
        }
    }
}

// ================================================================ propriedades
TESTE(componentes_e_ciclos) {
    GrafoLista<std::string> grafo;
    for (auto [u, v] : {std::pair{"A", "B"}, {"B", "C"}, {"E", "F"}}) grafo.adicionar_aresta(u, v);
    grafo.adicionar_vertice("G");
    VERIFICAR_IGUAL(componentes_conexas(grafo).size(), 3u);
    VERIFICAR(!eh_conexo(grafo));
    VERIFICAR(!tem_ciclo(grafo));
    grafo.adicionar_aresta("C", "A");
    VERIFICAR(tem_ciclo(grafo));

    GrafoLista<int> dag(true);
    dag.adicionar_aresta(1, 2);
    dag.adicionar_aresta(1, 3);
    dag.adicionar_aresta(2, 3);
    VERIFICAR(!tem_ciclo(dag));  // 1->2->3 e 1->3 não é ciclo em dígrafo
    dag.adicionar_aresta(3, 1);
    VERIFICAR(tem_ciclo(dag));
    VERIFICAR_LANCA(componentes_conexas(dag), std::invalid_argument);
}

TESTE(ordenacao_topologica_respeita_as_arestas) {
    std::mt19937 sorteio(4);
    for (int rodada = 0; rodada < 100; rodada++) {
        // aresta só de menor para maior -> nunca tem ciclo
        GrafoLista<int> dag(true);
        std::vector<int> rotulos(10);
        for (int i = 0; i < 10; i++) rotulos[i] = i;
        std::shuffle(rotulos.begin(), rotulos.end(), sorteio);
        for (int v : rotulos) dag.adicionar_vertice(v);
        for (int u = 0; u < 10; u++)
            for (int v = u + 1; v < 10; v++)
                if (sorteio() % 4 == 0) dag.adicionar_aresta(u, v);
        for (const auto& ordem : {ordenacao_topologica(dag), ordenacao_topologica_dfs(dag)}) {
            VERIFICAR_IGUAL(ordem.size(), 10u);
            std::map<int, int> posicao;
            for (int i = 0; i < 10; i++) posicao[ordem[i]] = i;
            for (const auto& [u, v, peso] : dag.arestas()) VERIFICAR(posicao[u] < posicao[v]);
        }
        dag.adicionar_aresta(9, 0);
        if (existe_caminho(dag, 0, 9)) {
            VERIFICAR_LANCA(ordenacao_topologica(dag), std::invalid_argument);
            VERIFICAR_LANCA(ordenacao_topologica_dfs(dag), std::invalid_argument);
        }
    }
}

TESTE(bipartido_contra_forca_bruta) {
    std::mt19937 sorteio(5);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto [lista, matriz] = grafo_aleatorio(sorteio, 7, 3, false);
        bool existe_coloracao = false;
        for (int mascara = 0; mascara < (1 << 7) && !existe_coloracao; mascara++) {
            bool ok = true;
            for (const auto& [u, v, peso] : matriz.arestas())
                if (((mascara >> u) & 1) == ((mascara >> v) & 1)) ok = false;
            existe_coloracao = ok;
        }
        auto cores = eh_bipartido(lista);
        VERIFICAR_IGUAL(cores.has_value(), existe_coloracao);
        if (cores)
            for (const auto& [u, v, peso] : lista.arestas()) VERIFICAR((*cores)[u] != (*cores)[v]);
    }
}

TESTE(kosaraju_contra_alcance_mutuo) {
    std::mt19937 sorteio(6);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto [lista, matriz] = grafo_aleatorio(sorteio, 8, 2, true);
        std::map<int, int> grupo;
        auto componentes = componentes_fortemente_conexas(lista);
        for (std::size_t i = 0; i < componentes.size(); i++)
            for (int v : componentes[i]) grupo[v] = static_cast<int>(i);
        VERIFICAR_IGUAL(grupo.size(), 8u);
        for (int u = 0; u < 8; u++)
            for (int v = 0; v < 8; v++) {
                bool mutuo = alcancaveis(matriz, u).count(v) && alcancaveis(matriz, v).count(u);
                VERIFICAR_IGUAL(grupo[u] == grupo[v], mutuo);
            }
    }
}

TESTE(pontes_e_articulacoes_contra_forca_bruta) {
    std::mt19937 sorteio(7);
    for (int rodada = 0; rodada < 100; rodada++) {
        auto [lista, _] = grafo_aleatorio(sorteio, 8, 3, false);
        int componentes_antes = static_cast<int>(componentes_conexas(lista).size());

        std::set<std::pair<int, int>> pontes_esperadas;
        for (const auto& [u, v, peso] : lista.arestas()) {
            GrafoLista<int> sem_aresta(false);
            for (int w : lista.vertices()) sem_aresta.adicionar_vertice(w);
            for (const auto& [a, b, p] : lista.arestas())
                if (!(a == u && b == v)) sem_aresta.adicionar_aresta(a, b, p);
            if (static_cast<int>(componentes_conexas(sem_aresta).size()) > componentes_antes)
                pontes_esperadas.insert({std::min(u, v), std::max(u, v)});
        }
        std::set<std::pair<int, int>> pontes_obtidas;
        for (const auto& [u, v] : pontes(lista)) pontes_obtidas.insert({std::min(u, v), std::max(u, v)});
        VERIFICAR_IGUAL(pontes_obtidas, pontes_esperadas);

        std::vector<int> articulacoes_esperadas;
        for (int x : lista.vertices()) {
            GrafoLista<int> sem_vertice(false);
            for (int w : lista.vertices())
                if (w != x) sem_vertice.adicionar_vertice(w);
            for (const auto& [a, b, p] : lista.arestas())
                if (a != x && b != x) sem_vertice.adicionar_aresta(a, b, p);
            if (static_cast<int>(componentes_conexas(sem_vertice).size()) > componentes_antes)
                articulacoes_esperadas.push_back(x);
        }
        VERIFICAR_IGUAL(pontos_de_articulacao(lista), articulacoes_esperadas);
    }
}

// ================================================================ caminhos mínimos
TESTE(dijkstra_bellman_ford_e_floyd_concordam) {
    std::mt19937 sorteio(8);
    for (int rodada = 0; rodada < 60; rodada++) {
        auto [lista, matriz] = grafo_aleatorio(sorteio, 8, 3, rodada % 2);
        auto todos = floyd_warshall(lista);
        for (int origem = 0; origem < 8; origem++) {
            auto por_dijkstra = dijkstra(lista, origem);
            VERIFICAR_IGUAL(por_dijkstra.distancia, bellman_ford(lista, origem).distancia);
            VERIFICAR_IGUAL(por_dijkstra.distancia, dijkstra(matriz, origem).distancia);
            VERIFICAR_IGUAL(por_dijkstra.distancia, todos.distancia[origem]);
            for (int destino = 0; destino < 8; destino++) {
                auto [caminho, custo] = caminho_minimo(lista, origem, destino);
                if (custo == INFINITO) {
                    VERIFICAR(caminho.empty());
                    VERIFICAR(caminho_floyd(todos.proximo, origem, destino).empty());
                    continue;
                }
                double soma = 0;  // o custo do caminho devolvido bate com a distância?
                for (std::size_t i = 0; i + 1 < caminho.size(); i++) soma += lista.peso(caminho[i], caminho[i + 1]);
                VERIFICAR_IGUAL(soma, custo);
                auto pela_matriz = caminho_floyd(todos.proximo, origem, destino);
                double soma_floyd = 0;
                for (std::size_t i = 0; i + 1 < pela_matriz.size(); i++)
                    soma_floyd += lista.peso(pela_matriz[i], pela_matriz[i + 1]);
                VERIFICAR_IGUAL(soma_floyd, custo);
            }
        }
    }
}

TESTE(pesos_negativos) {
    GrafoLista<std::string> grafo(true);
    for (auto [u, v, p] : {std::tuple{"S", "A", 4}, {"S", "B", 5}, {"A", "B", -3}, {"B", "C", 2}})
        grafo.adicionar_aresta(u, v, p);
    VERIFICAR_IGUAL(bellman_ford(grafo, "S").distancia.at("C"), 3.0);
    VERIFICAR_IGUAL(floyd_warshall(grafo).distancia["S"]["C"], 3.0);
    VERIFICAR_LANCA(dijkstra(grafo, "S"), std::invalid_argument);
    grafo.adicionar_aresta("C", "A", -1);  // A -> B -> C -> A custa -2: ciclo negativo
    VERIFICAR_LANCA(bellman_ford(grafo, "S"), std::invalid_argument);
    VERIFICAR_LANCA(floyd_warshall(grafo), std::invalid_argument);
}

// ================================================================ árvore geradora mínima
TESTE(union_find) {
    UniaoBusca<int> conjuntos({1, 2, 3, 4, 5});
    VERIFICAR_IGUAL(conjuntos.quantidade_de_grupos(), 5);
    VERIFICAR(conjuntos.unir(1, 2));
    VERIFICAR(conjuntos.unir(3, 4));
    VERIFICAR(!conjuntos.unir(2, 1));
    VERIFICAR(!conjuntos.mesmo_conjunto(1, 3));
    conjuntos.unir(2, 4);
    VERIFICAR(conjuntos.mesmo_conjunto(1, 3));
    VERIFICAR_IGUAL(conjuntos.quantidade_de_grupos(), 2);
    VERIFICAR_LANCA(conjuntos.encontrar(9), std::out_of_range);
}

TESTE(kruskal_e_prim_contra_forca_bruta) {
    std::mt19937 sorteio(9);
    for (int rodada = 0; rodada < 60; rodada++) {
        auto [lista, _] = grafo_aleatorio(sorteio, 6, 5, false);
        if (!eh_conexo(lista)) continue;
        auto arestas = lista.arestas();

        // Testa todos os subconjuntos de V-1 arestas e fica com a árvore mais barata.
        double melhor = INFINITO;
        int m = static_cast<int>(arestas.size());
        for (int mascara = 0; mascara < (1 << m); mascara++) {
            if (__builtin_popcount(mascara) != 5) continue;
            UniaoBusca<int> conjuntos(lista.vertices());
            double custo = 0;
            bool arvore = true;
            for (int i = 0; i < m && arvore; i++) {
                if (!((mascara >> i) & 1)) continue;
                auto [u, v, peso] = arestas[i];
                arvore = conjuntos.unir(u, v);
                custo += peso;
            }
            if (arvore) melhor = std::min(melhor, custo);
        }
        auto por_kruskal = kruskal(lista);
        auto por_prim = prim(lista);
        VERIFICAR_IGUAL(por_kruskal.custo, melhor);
        VERIFICAR_IGUAL(por_prim.custo, melhor);
        VERIFICAR_IGUAL(por_kruskal.arestas.size(), 5u);
        VERIFICAR_IGUAL(por_prim.arestas.size(), 5u);
    }
}

TESTE(agm_so_para_nao_dirigido) {
    GrafoLista<int> digrafo(true);
    digrafo.adicionar_aresta(1, 2);
    VERIFICAR_LANCA(kruskal(digrafo), std::invalid_argument);
    VERIFICAR_LANCA(prim(digrafo), std::invalid_argument);
    GrafoLista<int> vazio;
    VERIFICAR_IGUAL(prim(vazio).custo, 0.0);
}

int main() { return rodar_testes(); }
