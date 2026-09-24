// CAMINHOS MÍNIMOS (grafos com PESO)
//
// | algoritmo      | resolve                  | pesos negativos | complexidade      |
// |----------------|--------------------------|-----------------|-------------------|
// | BFS            | 1 origem, SEM peso       | -               | O(V + E)          |
// | Dijkstra       | 1 origem -> todos        | NÃO aceita      | O((V + E) log V)  |
// | Bellman-Ford   | 1 origem -> todos        | aceita (e       | O(V * E)          |
// |                |                          | detecta ciclo   |                   |
// |                |                          | negativo)       |                   |
// | Floyd-Warshall | TODOS -> todos           | aceita          | O(V³)             |
//
// Ideia de cada um
// - Dijkstra: sempre fecha o vértice não visitado de MENOR distância conhecida
//   (guloso) e RELAXA as arestas dele. Usa fila de prioridade (heap).
// - Bellman-Ford: relaxa TODAS as arestas V-1 vezes. Se ainda melhorar na
//   V-ésima passada, existe ciclo negativo.
// - Floyd-Warshall: programação dinâmica. Para cada vértice k, testa se passar
//   por k encurta o caminho entre todos os pares i -> j.
//
// Relaxar uma aresta (u, v):
//     se distancia[u] + peso(u, v) < distancia[v]:
//         distancia[v] = distancia[u] + peso(u, v);  pai[v] = u
//
// Pega-ratão
// - Dijkstra com peso negativo dá resposta ERRADA (ele fecha um vértice cedo
//   demais e nunca revisa). Para pesos negativos use Bellman-Ford.
// - Vértice inalcançável fica com distância infinita (INFINITO).
#pragma once

#include <functional>
#include <limits>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

#include "buscas.hpp"

const double INFINITO = std::numeric_limits<double>::infinity();

template <typename V>
struct CaminhosDeUmaOrigem {
    std::map<V, double> distancia;
    MapaDePais<V> pai;
};

// Menor custo de `origem` até cada vértice.
template <typename Grafo>
CaminhosDeUmaOrigem<typename Grafo::Vertice> dijkstra(const Grafo& grafo, const typename Grafo::Vertice& origem) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    CaminhosDeUmaOrigem<V> resultado;
    for (const V& v : grafo.vertices()) {
        resultado.distancia[v] = INFINITO;
        resultado.pai[v] = std::nullopt;
    }
    resultado.distancia[origem] = 0;

    // heap MÍNIMO de (custo, vértice): std::greater inverte a priority_queue,
    // que por padrão é heap máximo.
    using Item = std::pair<double, V>;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> heap;
    heap.push({0, origem});
    std::set<V> fechados;
    while (!heap.empty()) {
        auto [custo, atual] = heap.top();
        heap.pop();
        if (fechados.count(atual)) continue;  // entrada velha, já melhorada depois
        fechados.insert(atual);
        for (const auto& [vizinho, peso] : grafo.vizinhos(atual)) {
            if (peso < 0) throw std::invalid_argument("Dijkstra não aceita pesos negativos");
            if (custo + peso < resultado.distancia[vizinho]) {  // relaxamento
                resultado.distancia[vizinho] = custo + peso;
                resultado.pai[vizinho] = atual;
                heap.push({resultado.distancia[vizinho], vizinho});
            }
        }
    }
    return resultado;
}

// Aceita peso negativo. Lança std::invalid_argument se houver ciclo negativo.
template <typename Grafo>
CaminhosDeUmaOrigem<typename Grafo::Vertice> bellman_ford(const Grafo& grafo,
                                                           const typename Grafo::Vertice& origem) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    CaminhosDeUmaOrigem<V> resultado;
    auto& distancia = resultado.distancia;
    for (const V& v : grafo.vertices()) {
        distancia[v] = INFINITO;
        resultado.pai[v] = std::nullopt;
    }
    distancia[origem] = 0;

    // Em grafo não dirigido cada aresta vale nos dois sentidos.
    std::vector<std::tuple<V, V, double>> arestas;
    for (const V& u : grafo.vertices())
        for (const auto& [v, peso] : grafo.vizinhos(u)) arestas.emplace_back(u, v, peso);

    for (int passada = 0; passada < grafo.numero_vertices() - 1; passada++) {
        bool mudou = false;
        for (const auto& [u, v, peso] : arestas) {
            if (distancia[u] + peso < distancia[v]) {
                distancia[v] = distancia[u] + peso;
                resultado.pai[v] = u;
                mudou = true;
            }
        }
        if (!mudou) break;  // nada melhorou: já convergiu
    }
    // V-ésima passada: se ainda melhora, é porque existe ciclo negativo
    for (const auto& [u, v, peso] : arestas)
        if (distancia[u] + peso < distancia[v]) throw std::invalid_argument("O grafo tem ciclo de peso negativo");
    return resultado;
}

template <typename V>
struct CaminhosDeTodosOsPares {
    std::map<V, std::map<V, double>> distancia;
    std::map<V, std::map<V, std::optional<V>>> proximo;  // próximo vértice do caminho de u até v
};

// Menor distância entre TODOS os pares.
template <typename Grafo>
CaminhosDeTodosOsPares<typename Grafo::Vertice> floyd_warshall(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    const auto& vertices = grafo.vertices();
    CaminhosDeTodosOsPares<V> resultado;
    auto& distancia = resultado.distancia;
    auto& proximo = resultado.proximo;
    for (const V& u : vertices) {
        for (const V& v : vertices) {
            distancia[u][v] = INFINITO;
            proximo[u][v] = std::nullopt;
        }
    }
    for (const V& u : vertices) {
        distancia[u][u] = 0;
        for (const auto& [v, peso] : grafo.vizinhos(u)) {
            if (peso < distancia[u][v]) {  // guarda a aresta mais barata
                distancia[u][v] = peso;
                proximo[u][v] = v;
            }
        }
    }
    // k = vértice intermediário permitido; i -> k -> j encurta o caminho?
    for (const V& k : vertices) {
        for (const V& i : vertices) {
            if (distancia[i][k] == INFINITO) continue;
            for (const V& j : vertices) {
                if (distancia[i][k] + distancia[k][j] < distancia[i][j]) {
                    distancia[i][j] = distancia[i][k] + distancia[k][j];
                    proximo[i][j] = proximo[i][k];
                }
            }
        }
    }
    for (const V& v : vertices)
        if (distancia[v][v] < 0) throw std::invalid_argument("O grafo tem ciclo de peso negativo");
    return resultado;
}

// Monta o caminho origem -> destino usando a matriz `proximo` (vazio se não existir).
template <typename V>
std::vector<V> caminho_floyd(const std::map<V, std::map<V, std::optional<V>>>& proximo, const V& origem,
                             const V& destino) {
    if (!proximo.at(origem).at(destino) && origem != destino) return {};
    std::vector<V> caminho{origem};
    V atual = origem;
    while (atual != destino) {
        atual = *proximo.at(atual).at(destino);
        caminho.push_back(atual);
    }
    return caminho;
}

// Atalho: roda Dijkstra e devolve {caminho, custo}.
template <typename Grafo>
std::pair<std::vector<typename Grafo::Vertice>, double> caminho_minimo(const Grafo& grafo,
                                                                        const typename Grafo::Vertice& origem,
                                                                        const typename Grafo::Vertice& destino) {
    auto [distancia, pai] = dijkstra(grafo, origem);
    if (distancia.at(destino) == INFINITO) return {{}, INFINITO};
    return {reconstruir_caminho(pai, origem, destino), distancia.at(destino)};
}
