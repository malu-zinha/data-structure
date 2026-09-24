#include <iomanip>
#include <iostream>
#include <string>

#include "../util.hpp"
#include "arvore_geradora_minima.hpp"
#include "buscas.hpp"
#include "caminhos_minimos.hpp"
#include "grafo_lista.hpp"
#include "grafo_matriz.hpp"
#include "propriedades.hpp"

template <typename V>
std::string grupos(const std::vector<std::vector<V>>& componentes) {
    std::string saida;
    for (const auto& componente : componentes) saida += em_colchetes(componente) + " ";
    return saida;
}

template <typename V>
std::string lista_de_arestas(const std::vector<std::tuple<V, V, double>>& arestas) {
    std::ostringstream saida;
    for (const auto& [origem, destino, peso] : arestas) saida << origem << "-" << destino << "(" << peso << ") ";
    return saida.str();
}

int main() {
    titulo("Lista de adjacência");
    GrafoLista<std::string> grafo;
    grafo.adicionar_aresta("A", "B", 4);
    grafo.adicionar_aresta("A", "C", 2);
    grafo.adicionar_aresta("B", "C", 1);
    grafo.adicionar_aresta("C", "D", 7);
    grafo.adicionar_vertice("E");  // vértice isolado
    std::cout << grafo.desenho();
    std::cout << "Arestas: " << lista_de_arestas(grafo.arestas()) << "\n";
    std::cout << "Grau de C: " << grafo.grau("C") << " | existe A-D? " << (grafo.existe_aresta("A", "D") ? "sim" : "não")
              << " | peso A-B: " << grafo.peso("A", "B") << "\n";

    GrafoLista<std::string> digrafo(true);
    for (auto [origem, destino] : {std::pair{"A", "B"}, {"B", "C"}, {"C", "A"}, {"C", "D"}})
        digrafo.adicionar_aresta(origem, destino);
    std::cout << "\nDígrafo:\n" << digrafo.desenho();
    std::cout << "Grau de entrada de A: " << digrafo.grau_entrada("A") << " | de saída: " << digrafo.grau_saida("A")
              << "\n";
    std::cout << "Transposto:\n" << digrafo.transposto().desenho();

    titulo("Matriz de adjacência");
    GrafoMatriz matriz(4);
    matriz.adicionar_aresta(0, 1, 4);
    matriz.adicionar_aresta(0, 2, 2);
    matriz.adicionar_aresta(1, 2, 1);
    matriz.adicionar_aresta(2, 3, 7);
    std::cout << matriz.desenho();
    std::cout << "Grau de 2: " << matriz.grau(2) << " | arestas: " << lista_de_arestas(matriz.arestas()) << "\n";
    int novo = matriz.adicionar_vertice();
    std::cout << "Vértice " << novo << " adicionado -> agora " << matriz.numero_vertices() << " vértices\n";

    titulo("BFS e DFS");
    GrafoLista<std::string> ruas;
    for (auto [origem, destino] :
         {std::pair{"A", "B"}, {"A", "C"}, {"B", "D"}, {"C", "D"}, {"D", "E"}, {"E", "F"}, {"C", "F"}})
        ruas.adicionar_aresta(origem, destino);
    std::cout << "BFS a partir de A: " << em_colchetes(bfs(ruas, "A")) << "\n";
    std::cout << "DFS a partir de A: " << em_colchetes(dfs(ruas, "A")) << "\n";
    std::cout << "DFS iterativa:     " << em_colchetes(dfs_iterativa(ruas, "A")) << "\n";
    std::cout << "Menor caminho A -> F: " << juntar(menor_caminho_bfs(ruas, "A", "F"), " -> ")
              << "\n";
    std::cout << "Todos os caminhos A -> F: " << grupos(todos_os_caminhos(ruas, "A", "F"))
              << "\n";

    titulo("Propriedades");
    GrafoLista<std::string> pedacos;
    for (auto [origem, destino] : {std::pair{"A", "B"}, {"B", "C"}, {"C", "A"}, {"C", "D"}, {"E", "F"}})
        pedacos.adicionar_aresta(origem, destino);
    pedacos.adicionar_vertice("G");
    std::cout << "Componentes conexas: " << grupos(componentes_conexas(pedacos))
              << "| conexo? " << (eh_conexo(pedacos) ? "sim" : "não") << "\n";
    std::cout << "Tem ciclo? " << (tem_ciclo(pedacos) ? "sim" : "não") << "\n";
    std::cout << "Pontes:";
    for (const auto& [u, v] : pontes(pedacos)) std::cout << " " << u << "-" << v;
    std::cout << " | articulações: " << em_colchetes(pontos_de_articulacao(pedacos)) << "\n";

    GrafoLista<int> ciclo_par;
    for (auto [origem, destino] : {std::pair{1, 2}, {2, 3}, {3, 4}, {4, 1}}) ciclo_par.adicionar_aresta(origem, destino);
    std::cout << "Ciclo de 4 é bipartido? " << (eh_bipartido(ciclo_par) ? "sim" : "não") << "\n";
    ciclo_par.adicionar_aresta(4, 2);
    std::cout << "Depois de criar um ciclo ímpar: " << (eh_bipartido(ciclo_par) ? "sim" : "não") << "\n";

    GrafoLista<std::string> materias(true);
    for (auto [pre, pos] : {std::pair{"Algoritmos", "Estrutura de Dados"}, {"Cálculo 1", "Cálculo 2"},
                            {"Estrutura de Dados", "Banco de Dados"}, {"Estrutura de Dados", "Compiladores"},
                            {"Cálculo 2", "Física"}})
        materias.adicionar_aresta(pre, pos);
    std::cout << "Ordem para cursar (Kahn): " << em_colchetes(ordenacao_topologica(materias)) << "\n";
    std::cout << "Ordem para cursar (DFS):  " << em_colchetes(ordenacao_topologica_dfs(materias)) << "\n";

    GrafoLista<std::string> fortes(true);
    for (auto [origem, destino] :
         {std::pair{"A", "B"}, {"B", "C"}, {"C", "A"}, {"B", "D"}, {"D", "E"}, {"E", "D"}, {"F", "E"}})
        fortes.adicionar_aresta(origem, destino);
    std::cout << "Componentes fortemente conexas: " << grupos(componentes_fortemente_conexas(fortes)) << "\n";

    titulo("Caminhos mínimos");
    GrafoLista<std::string> mapa;
    for (auto [origem, destino, km] : {std::tuple{"A", "B", 4}, {"A", "C", 2}, {"B", "C", 1}, {"B", "D", 5},
                                       {"C", "D", 8}, {"C", "E", 10}, {"D", "E", 2}, {"D", "F", 6}, {"E", "F", 3}})
        mapa.adicionar_aresta(origem, destino, km);
    auto [distancia, pai] = dijkstra(mapa, "A");
    std::cout << "Dijkstra a partir de A:";
    for (const auto& [vertice, custo] : distancia) std::cout << " " << vertice << "=" << custo;
    std::cout << "\n";
    auto [caminho, custo] = caminho_minimo(mapa, "A", "F");
    std::cout << "Menor caminho A -> F: " << juntar(caminho, " -> ") << " (custo " << custo << ")\n";
    std::cout << "Bellman-Ford dá o mesmo resultado? "
              << (bellman_ford(mapa, "A").distancia == distancia ? "sim" : "não") << "\n";

    auto todos = floyd_warshall(mapa);
    std::cout << "Floyd-Warshall (distância de cada par):\n     ";
    for (const auto& v : mapa.vertices()) std::cout << std::setw(4) << v;
    std::cout << "\n";
    for (const auto& u : mapa.vertices()) {
        std::cout << "  " << u << "  ";
        for (const auto& v : mapa.vertices()) std::cout << std::setw(4) << todos.distancia[u][v];
        std::cout << "\n";
    }
    std::cout << "Caminho C -> F pela matriz: "
              << juntar(caminho_floyd(todos.proximo, std::string("C"), std::string("F")), " -> ") << "\n";

    GrafoLista<std::string> negativo(true);
    for (auto [origem, destino, peso] : {std::tuple{"S", "A", 4}, {"S", "B", 5}, {"A", "B", -3}, {"B", "C", 2}})
        negativo.adicionar_aresta(origem, destino, peso);
    std::cout << "Com peso negativo, Bellman-Ford:";
    for (const auto& [vertice, d] : bellman_ford(negativo, "S").distancia)
        std::cout << " " << vertice << "=" << d;
    std::cout << "\n";
    try {
        dijkstra(negativo, "S");
    } catch (const std::invalid_argument& erro) {
        std::cout << "Dijkstra recusa: " << erro.what() << "\n";
    }

    titulo("Árvore geradora mínima");
    GrafoLista<std::string> rede;
    for (auto [origem, destino, preco] : {std::tuple{"A", "B", 4}, {"A", "C", 3}, {"B", "C", 1}, {"B", "D", 2},
                                          {"C", "D", 4}, {"D", "E", 2}, {"C", "E", 6}, {"E", "F", 3}, {"D", "F", 5}})
        rede.adicionar_aresta(origem, destino, preco);
    auto agm_kruskal = kruskal(rede);
    std::cout << "Kruskal: " << lista_de_arestas(agm_kruskal.arestas) << "| custo " << agm_kruskal.custo << "\n";
    auto agm_prim = prim(rede, "A");
    std::cout << "Prim:    " << lista_de_arestas(agm_prim.arestas) << "| custo " << agm_prim.custo << "\n";

    UniaoBusca<std::string> conjuntos({"A", "B", "C", "D"});
    conjuntos.unir("A", "B");
    conjuntos.unir("C", "D");
    std::cout << "Union-Find: A e B juntos? " << (conjuntos.mesmo_conjunto("A", "B") ? "sim" : "não")
              << " | A e C juntos? " << (conjuntos.mesmo_conjunto("A", "C") ? "sim" : "não");
    conjuntos.unir("B", "C");
    std::cout << " | depois de unir B e C, grupos: " << conjuntos.quantidade_de_grupos() << "\n";
}
