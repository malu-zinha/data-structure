// BUSCAS EM GRAFOS — BFS e DFS
//
// Funcionam com QUALQUER das duas representações (`GrafoLista` ou `GrafoMatriz`),
// porque só usam `grafo.vertices()`, `grafo.vizinhos(v)` e `grafo.contem(v)`.
// Por isso são templates: `Grafo` é o tipo do grafo e `Grafo::Vertice` o do vértice.
//
// BFS (Busca em LARGURA) — usa FILA
//   Visita por "ondas": primeiro os vizinhos, depois os vizinhos dos vizinhos.
//   Acha o MENOR caminho em número de arestas (grafo sem peso).
//
// DFS (Busca em PROFUNDIDADE) — usa PILHA (ou recursão)
//   Vai fundo por um caminho até não dar mais e então volta (backtracking).
//   Base para: detectar ciclo, ordenação topológica, componentes fortemente conexas.
//
// Complexidade das duas: O(V + E) com lista de adjacência; O(V²) com matriz.
//
// Pega-ratão
// - MARQUE o vértice como visitado ao ENFILEIRAR na BFS (não ao desenfileirar),
//   senão ele entra várias vezes na fila.
// - Sem a marcação de visitados, um ciclo faz o algoritmo rodar para sempre.
// - A DFS recursiva pode estourar a pilha em grafos enormes; use a iterativa.
#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <vector>

template <typename Grafo>
void validar_vertice(const Grafo& grafo, const typename Grafo::Vertice& vertice) {
    if (!grafo.contem(vertice)) throw std::out_of_range("Vértice não existe");
}

// Pai de cada vértice na árvore de busca (nullopt = é a raiz da busca).
template <typename V>
using MapaDePais = std::map<V, std::optional<V>>;

// ======================================================================
// BFS — Busca em largura
// ======================================================================
// Devolve a ordem de visita a partir de `origem`.
template <typename Grafo>
std::vector<typename Grafo::Vertice> bfs(const Grafo& grafo, const typename Grafo::Vertice& origem) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    std::set<V> visitados{origem};
    std::queue<V> fila;
    fila.push(origem);
    std::vector<V> ordem;
    while (!fila.empty()) {
        V atual = fila.front();  // sai o mais antigo (FIFO)
        fila.pop();
        ordem.push_back(atual);
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (!visitados.count(vizinho)) {
                visitados.insert(vizinho);  // marca AO ENFILEIRAR
                fila.push(vizinho);
            }
        }
    }
    return ordem;
}

// Devolve {distancia, pai} em NÚMERO DE ARESTAS (ignora os pesos).
// Só aparecem os vértices alcançáveis a partir da origem.
template <typename Grafo>
std::pair<std::map<typename Grafo::Vertice, int>, MapaDePais<typename Grafo::Vertice>> bfs_distancias(
    const Grafo& grafo, const typename Grafo::Vertice& origem) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    std::map<V, int> distancia{{origem, 0}};
    MapaDePais<V> pai{{origem, std::nullopt}};
    std::queue<V> fila;
    fila.push(origem);
    while (!fila.empty()) {
        V atual = fila.front();
        fila.pop();
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (!distancia.count(vizinho)) {
                distancia[vizinho] = distancia[atual] + 1;
                pai[vizinho] = atual;
                fila.push(vizinho);
            }
        }
    }
    return {distancia, pai};
}

// Monta o caminho origem -> destino andando de trás para frente pelos pais.
// Vetor vazio = não existe caminho.
template <typename V>
std::vector<V> reconstruir_caminho(const MapaDePais<V>& pai, const V& origem, const V& destino) {
    if (!pai.count(destino)) return {};
    std::vector<V> caminho;
    std::optional<V> atual = destino;
    while (atual) {
        caminho.push_back(*atual);
        if (*atual == origem) break;
        auto achou = pai.find(*atual);
        atual = achou != pai.end() ? achou->second : std::nullopt;
    }
    std::reverse(caminho.begin(), caminho.end());
    if (caminho.empty() || caminho.front() != origem) return {};
    return caminho;
}

// Caminho com MENOS ARESTAS entre dois vértices (vazio se não existir).
template <typename Grafo>
std::vector<typename Grafo::Vertice> menor_caminho_bfs(const Grafo& grafo, const typename Grafo::Vertice& origem,
                                                        const typename Grafo::Vertice& destino) {
    return reconstruir_caminho(bfs_distancias(grafo, origem).second, origem, destino);
}

// ======================================================================
// DFS — Busca em profundidade
// ======================================================================
// Versão RECURSIVA. Devolve a ordem de visita.
template <typename Grafo>
std::vector<typename Grafo::Vertice> dfs(const Grafo& grafo, const typename Grafo::Vertice& origem) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    std::set<V> visitados;
    std::vector<V> ordem;
    std::function<void(const V&)> visitar = [&](const V& atual) {
        visitados.insert(atual);
        ordem.push_back(atual);
        for (const auto& [vizinho, _] : grafo.vizinhos(atual))
            if (!visitados.count(vizinho)) visitar(vizinho);
    };
    visitar(origem);
    return ordem;
}

// Mesma ordem da recursiva, trocando a recursão por uma PILHA explícita.
template <typename Grafo>
std::vector<typename Grafo::Vertice> dfs_iterativa(const Grafo& grafo, const typename Grafo::Vertice& origem) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    std::set<V> visitados;
    std::vector<V> ordem;
    std::stack<V> pilha;
    pilha.push(origem);
    while (!pilha.empty()) {
        V atual = pilha.top();
        pilha.pop();
        if (visitados.count(atual)) continue;
        visitados.insert(atual);
        ordem.push_back(atual);
        // empilha ao contrário para visitar os vizinhos na ordem original
        auto vizinhos = grafo.vizinhos(atual);
        for (auto it = vizinhos.rbegin(); it != vizinhos.rend(); ++it)
            if (!visitados.count(it->first)) pilha.push(it->first);
    }
    return ordem;
}

// Tempos do algoritmo do CLRS: descoberta = quando o vértice ficou cinza;
// finalização = quando ficou preto.
template <typename V>
struct TemposDFS {
    std::map<V, int> descoberta;
    std::map<V, int> finalizacao;
    MapaDePais<V> pai;
};

// DFS em TODOS os vértices (pega também as partes desconexas).
template <typename Grafo>
TemposDFS<typename Grafo::Vertice> dfs_completa(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    TemposDFS<V> tempos;
    int tempo = 0;
    std::function<void(const V&)> visitar = [&](const V& atual) {
        tempos.descoberta[atual] = tempo++;
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (!tempos.descoberta.count(vizinho)) {
                tempos.pai[vizinho] = atual;
                visitar(vizinho);
            }
        }
        tempos.finalizacao[atual] = tempo++;
    };
    for (const V& vertice : grafo.vertices()) {
        if (!tempos.descoberta.count(vertice)) {
            tempos.pai[vertice] = std::nullopt;
            visitar(vertice);
        }
    }
    return tempos;
}

template <typename Grafo>
bool existe_caminho(const Grafo& grafo, const typename Grafo::Vertice& origem, const typename Grafo::Vertice& destino) {
    validar_vertice(grafo, destino);
    auto alcancados = bfs(grafo, origem);
    return std::find(alcancados.begin(), alcancados.end(), destino) != alcancados.end();
}

// Todos os caminhos SIMPLES (sem repetir vértice) entre origem e destino.
// Usa backtracking: entra no vizinho, e ao voltar DESMARCA para que ele possa
// ser usado em outro caminho. Cuidado: a quantidade de caminhos pode explodir.
template <typename Grafo>
std::vector<std::vector<typename Grafo::Vertice>> todos_os_caminhos(const Grafo& grafo,
                                                                    const typename Grafo::Vertice& origem,
                                                                    const typename Grafo::Vertice& destino) {
    using V = typename Grafo::Vertice;
    validar_vertice(grafo, origem);
    validar_vertice(grafo, destino);
    std::vector<std::vector<V>> caminhos;
    std::vector<V> caminho_atual{origem};
    std::set<V> no_caminho{origem};
    std::function<void(const V&)> explorar = [&](const V& atual) {
        if (atual == destino) {
            caminhos.push_back(caminho_atual);
            return;
        }
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (!no_caminho.count(vizinho)) {
                no_caminho.insert(vizinho);
                caminho_atual.push_back(vizinho);
                explorar(vizinho);
                caminho_atual.pop_back();  // desfaz (backtracking)
                no_caminho.erase(vizinho);
            }
        }
    };
    explorar(origem);
    return caminhos;
}
