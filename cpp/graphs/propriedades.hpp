// PROPRIEDADES DE GRAFOS (tudo a partir de BFS/DFS)
//
// - componentes_conexas            -> "pedaços" separados de um grafo não dirigido
// - eh_conexo                      -> o grafo é um pedaço só?
// - tem_ciclo                      -> existe ciclo? (trata dirigido e não dirigido)
// - ordenacao_topologica           -> ordem válida de tarefas num DAG (algoritmo de Kahn)
// - ordenacao_topologica_dfs       -> a mesma coisa, via DFS (pós-ordem invertida)
// - eh_bipartido                   -> dá para colorir com 2 cores sem vizinhos iguais?
// - componentes_fortemente_conexas -> algoritmo de Kosaraju (dígrafos)
// - pontes / pontos_de_articulacao -> algoritmo de Tarjan (o que "quebra" o grafo)
//
// Complexidade: todos O(V + E).
#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stack>
#include <stdexcept>
#include <utility>
#include <vector>

// ======================================================================
// Conectividade (grafos NÃO dirigidos)
// ======================================================================
// Cada componente é um grupo de vértices ligados entre si.
template <typename Grafo>
std::vector<std::vector<typename Grafo::Vertice>> componentes_conexas(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (grafo.dirigido()) throw std::invalid_argument("Use componentes_fortemente_conexas para dígrafos");
    std::set<V> visitados;
    std::vector<std::vector<V>> componentes;
    for (const V& inicio : grafo.vertices()) {
        if (visitados.count(inicio)) continue;
        // BFS a partir de um vértice ainda não visitado = uma componente nova
        std::vector<V> componente;
        visitados.insert(inicio);
        std::queue<V> fila;
        fila.push(inicio);
        while (!fila.empty()) {
            V atual = fila.front();
            fila.pop();
            componente.push_back(atual);
            for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
                if (!visitados.count(vizinho)) {
                    visitados.insert(vizinho);
                    fila.push(vizinho);
                }
            }
        }
        componentes.push_back(componente);
    }
    return componentes;
}

template <typename Grafo>
bool eh_conexo(const Grafo& grafo) {
    return componentes_conexas(grafo).size() <= 1;
}

// ======================================================================
// Ciclos
// ======================================================================
enum class CorDFS { BRANCO, CINZA, PRETO };  // não visitado / em processamento / finalizado

// Dígrafo: procura ARESTA DE RETORNO para um vértice ainda "em aberto" (cinza).
// Não dirigido: achar um visitado que não seja o pai já é ciclo.
template <typename Grafo>
bool tem_ciclo(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (grafo.dirigido()) {
        std::map<V, CorDFS> cor;
        for (const V& v : grafo.vertices()) cor[v] = CorDFS::BRANCO;
        std::function<bool(const V&)> visitar = [&](const V& atual) {
            cor[atual] = CorDFS::CINZA;
            for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
                if (cor[vizinho] == CorDFS::CINZA) return true;  // voltou para alguém do caminho atual
                if (cor[vizinho] == CorDFS::BRANCO && visitar(vizinho)) return true;
            }
            cor[atual] = CorDFS::PRETO;
            return false;
        };
        for (const V& v : grafo.vertices())
            if (cor[v] == CorDFS::BRANCO && visitar(v)) return true;
        return false;
    }

    std::set<V> visitados;
    std::function<bool(const V&, const std::optional<V>&)> visitar = [&](const V& atual, const std::optional<V>& pai) {
        visitados.insert(atual);
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (!visitados.count(vizinho)) {
                if (visitar(vizinho, atual)) return true;
            } else if (!pai || vizinho != *pai) {  // visitado e não é de onde eu vim -> ciclo
                return true;
            }
        }
        return false;
    };
    for (const V& v : grafo.vertices())
        if (!visitados.count(v) && visitar(v, std::nullopt)) return true;
    return false;
}

// ======================================================================
// Ordenação topológica (só para DAG: dígrafo acíclico)
// ======================================================================
// Algoritmo de KAHN (usa FILA + grau de entrada).
// Ideia: quem não depende de ninguém (grau de entrada 0) pode ir primeiro.
// Ao "remover" um vértice, os dependentes dele perdem uma dependência.
// Ex.: ordem de matérias respeitando pré-requisitos.
template <typename Grafo>
std::vector<typename Grafo::Vertice> ordenacao_topologica(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (!grafo.dirigido()) throw std::invalid_argument("Ordenação topológica só existe em dígrafos");
    std::map<V, int> grau_entrada;
    for (const V& v : grafo.vertices()) grau_entrada[v] += 0;
    for (const V& v : grafo.vertices())
        for (const auto& [vizinho, _] : grafo.vizinhos(v)) grau_entrada[vizinho]++;

    std::queue<V> fila;
    for (const V& v : grafo.vertices())
        if (grau_entrada[v] == 0) fila.push(v);

    std::vector<V> ordem;
    while (!fila.empty()) {
        V atual = fila.front();
        fila.pop();
        ordem.push_back(atual);
        for (const auto& [vizinho, _] : grafo.vizinhos(atual))
            if (--grau_entrada[vizinho] == 0) fila.push(vizinho);
    }
    if (static_cast<int>(ordem.size()) != grafo.numero_vertices())
        throw std::invalid_argument("O grafo tem ciclo: não existe ordenação topológica");
    return ordem;
}

// Mesma ideia via DFS: guarda na PÓS-ordem e inverte no final.
template <typename Grafo>
std::vector<typename Grafo::Vertice> ordenacao_topologica_dfs(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (!grafo.dirigido()) throw std::invalid_argument("Ordenação topológica só existe em dígrafos");
    std::map<V, CorDFS> cor;
    for (const V& v : grafo.vertices()) cor[v] = CorDFS::BRANCO;
    std::vector<V> ordem;
    std::function<void(const V&)> visitar = [&](const V& atual) {
        cor[atual] = CorDFS::CINZA;
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (cor[vizinho] == CorDFS::CINZA)
                throw std::invalid_argument("O grafo tem ciclo: não existe ordenação topológica");
            if (cor[vizinho] == CorDFS::BRANCO) visitar(vizinho);
        }
        cor[atual] = CorDFS::PRETO;
        ordem.push_back(atual);  // entra depois de todos os seus dependentes
    };
    for (const V& v : grafo.vertices())
        if (cor[v] == CorDFS::BRANCO) visitar(v);
    std::reverse(ordem.begin(), ordem.end());
    return ordem;
}

// ======================================================================
// Grafo bipartido (2-coloração)
// ======================================================================
// Devolve a cor (0 ou 1) de cada vértice, ou nullopt se não for bipartido.
// BFS pintando cada vizinho com a cor oposta. Se dois vizinhos ficarem com a
// mesma cor, existe ciclo ÍMPAR e o grafo não é bipartido.
template <typename Grafo>
std::optional<std::map<typename Grafo::Vertice, int>> eh_bipartido(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (grafo.dirigido()) throw std::invalid_argument("Bipartição é definida para grafos não dirigidos");
    std::map<V, int> cor;
    for (const V& inicio : grafo.vertices()) {
        if (cor.count(inicio)) continue;
        cor[inicio] = 0;
        std::queue<V> fila;
        fila.push(inicio);
        while (!fila.empty()) {
            V atual = fila.front();
            fila.pop();
            for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
                if (!cor.count(vizinho)) {
                    cor[vizinho] = 1 - cor[atual];  // cor oposta
                    fila.push(vizinho);
                } else if (cor[vizinho] == cor[atual]) {
                    return std::nullopt;
                }
            }
        }
    }
    return cor;
}

// ======================================================================
// Componentes fortemente conexas (Kosaraju) — só em dígrafos
// ======================================================================
// Grupos em que TODO vértice alcança todos os outros (ida e volta).
// Kosaraju, em 2 DFS:
//   1. DFS no grafo original guardando a ordem de FINALIZAÇÃO.
//   2. DFS no grafo TRANSPOSTO (arestas invertidas), pegando os vértices na
//      ordem inversa de finalização. Cada árvore gerada é uma componente.
template <typename Grafo>
std::vector<std::vector<typename Grafo::Vertice>> componentes_fortemente_conexas(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (!grafo.dirigido()) throw std::invalid_argument("Use componentes_conexas para grafos não dirigidos");

    // ---- passo 1: ordem de finalização (DFS iterativa, evita estourar a pilha)
    // Cada item da pilha guarda o vértice e o índice do próximo vizinho a olhar.
    std::set<V> visitados;
    std::vector<V> ordem_finalizacao;
    for (const V& inicio : grafo.vertices()) {
        if (visitados.count(inicio)) continue;
        visitados.insert(inicio);
        std::vector<std::pair<V, std::size_t>> pilha{{inicio, 0}};
        while (!pilha.empty()) {
            auto& [atual, proximo] = pilha.back();
            const auto& vizinhos = grafo.vizinhos(atual);
            if (proximo < vizinhos.size()) {
                V vizinho = vizinhos[proximo++].first;
                if (!visitados.count(vizinho)) {
                    visitados.insert(vizinho);
                    pilha.push_back({vizinho, 0});
                }
            } else {
                ordem_finalizacao.push_back(atual);
                pilha.pop_back();
            }
        }
    }

    // ---- passo 2: DFS no transposto, na ordem inversa de finalização
    std::map<V, std::vector<V>> transposto;
    for (const V& v : grafo.vertices())
        for (const auto& [vizinho, _] : grafo.vizinhos(v)) transposto[vizinho].push_back(v);

    visitados.clear();
    std::vector<std::vector<V>> componentes;
    for (auto it = ordem_finalizacao.rbegin(); it != ordem_finalizacao.rend(); ++it) {
        if (visitados.count(*it)) continue;
        std::vector<V> componente;
        std::stack<V> pilha;
        pilha.push(*it);
        visitados.insert(*it);
        while (!pilha.empty()) {
            V atual = pilha.top();
            pilha.pop();
            componente.push_back(atual);
            for (const V& vizinho : transposto[atual]) {
                if (!visitados.count(vizinho)) {
                    visitados.insert(vizinho);
                    pilha.push(vizinho);
                }
            }
        }
        componentes.push_back(componente);
    }
    return componentes;
}

// ======================================================================
// Pontes e pontos de articulação (Tarjan) — grafos não dirigidos
// ======================================================================
// Calcula, para cada vértice, o menor tempo de descoberta alcançável (low).
// - PONTE: aresta (u, v) em que low[v] > descoberta[u] -> tirar a aresta
//   desconecta o grafo.
// - PONTO DE ARTICULAÇÃO: vértice que, se removido, desconecta o grafo.
template <typename Grafo>
std::pair<std::vector<std::pair<typename Grafo::Vertice, typename Grafo::Vertice>>, std::set<typename Grafo::Vertice>>
tarjan(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (grafo.dirigido()) throw std::invalid_argument("Pontes e articulações valem para grafos não dirigidos");
    std::map<V, int> descoberta, low;
    std::vector<std::pair<V, V>> pontes;
    std::set<V> articulacoes;
    int tempo = 0;

    std::function<void(const V&, const std::optional<V>&)> visitar = [&](const V& atual, const std::optional<V>& pai) {
        descoberta[atual] = low[atual] = tempo++;
        int filhos = 0;
        for (const auto& [vizinho, _] : grafo.vizinhos(atual)) {
            if (!descoberta.count(vizinho)) {
                filhos++;
                visitar(vizinho, atual);
                low[atual] = std::min(low[atual], low[vizinho]);
                if (low[vizinho] > descoberta[atual]) pontes.emplace_back(atual, vizinho);
                if (pai && low[vizinho] >= descoberta[atual]) articulacoes.insert(atual);
            } else if (!pai || vizinho != *pai) {  // aresta de retorno
                low[atual] = std::min(low[atual], descoberta[vizinho]);
            }
        }
        if (!pai && filhos > 1) articulacoes.insert(atual);  // raiz com 2+ filhos na árvore DFS
    };

    for (const V& v : grafo.vertices())
        if (!descoberta.count(v)) visitar(v, std::nullopt);
    return {pontes, articulacoes};
}

template <typename Grafo>
std::vector<std::pair<typename Grafo::Vertice, typename Grafo::Vertice>> pontes(const Grafo& grafo) {
    return tarjan(grafo).first;
}

template <typename Grafo>
std::vector<typename Grafo::Vertice> pontos_de_articulacao(const Grafo& grafo) {
    auto articulacoes = tarjan(grafo).second;
    return {articulacoes.begin(), articulacoes.end()};  // std::set já sai ordenado
}
