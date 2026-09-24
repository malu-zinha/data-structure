// ÁRVORE GERADORA MÍNIMA (AGM / MST)
//
// Problema: ligar TODOS os vértices de um grafo não dirigido com peso, usando o
// MENOR custo total e SEM formar ciclo. O resultado tem V-1 arestas (se o grafo
// for conexo) e é uma árvore.
// Ex.: puxar cabo de rede entre prédios gastando o mínimo possível.
//
// KRUSKAL — pensa nas ARESTAS
//   1. Ordena todas as arestas da mais barata para a mais cara.
//   2. Pega a aresta se ela LIGAR DOIS GRUPOS DIFERENTES (senão formaria ciclo).
//   3. Para quando tiver V-1 arestas.
//   Complexidade: O(E log E), dominada pela ordenação.
//   Precisa da estrutura UNION-FIND (conjuntos disjuntos) para saber, em tempo
//   quase constante, se dois vértices já estão no mesmo grupo.
//
// PRIM — pensa nos VÉRTICES
//   1. Começa de um vértice qualquer.
//   2. Sempre adiciona a aresta mais barata que liga a árvore a um vértice de FORA.
//   3. Para quando todos os vértices entrarem.
//   Complexidade: O(E log V) com fila de prioridade (heap).
//
// Qual usar? Kruskal vai bem em grafo esparso; Prim, em grafo denso.
// Se o grafo for desconexo, os dois devolvem uma FLORESTA (Prim só a componente
// do vértice inicial; Kruskal, a de todas as componentes).
#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <queue>
#include <set>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <vector>

// UNION-FIND / Conjuntos Disjuntos (DSU).
// Responde "esses dois estão no mesmo grupo?" e junta grupos.
// Otimizações: união por TAMANHO + compressão de caminho -> quase O(1).
template <typename V>
class UniaoBusca {
public:
    UniaoBusca() = default;
    explicit UniaoBusca(const std::vector<V>& elementos) {
        for (const V& elemento : elementos) adicionar(elemento);
    }

    void adicionar(const V& elemento) {
        if (pai_.count(elemento)) return;
        pai_[elemento] = elemento;  // começa sozinho, chefe de si mesmo
        tamanho_[elemento] = 1;
    }

    // Devolve o representante do grupo, encurtando o caminho na volta.
    V encontrar(V elemento) {
        if (!pai_.count(elemento)) throw std::out_of_range("Elemento não existe");
        V raiz = elemento;
        while (pai_[raiz] != raiz) raiz = pai_[raiz];
        while (pai_[elemento] != raiz) {  // compressão de caminho
            V proximo = pai_[elemento];
            pai_[elemento] = raiz;
            elemento = proximo;
        }
        return raiz;
    }

    // Junta os grupos de a e b. Devolve false se já eram o mesmo grupo.
    bool unir(const V& a, const V& b) {
        V raiz_a = encontrar(a), raiz_b = encontrar(b);
        if (raiz_a == raiz_b) return false;
        if (tamanho_[raiz_a] < tamanho_[raiz_b]) std::swap(raiz_a, raiz_b);  // o menor entra no maior
        pai_[raiz_b] = raiz_a;
        tamanho_[raiz_a] += tamanho_[raiz_b];
        return true;
    }

    bool mesmo_conjunto(const V& a, const V& b) { return encontrar(a) == encontrar(b); }

    int quantidade_de_grupos() const {
        int grupos = 0;
        for (const auto& [elemento, pai] : pai_)
            if (elemento == pai) grupos++;
        return grupos;
    }

private:
    std::map<V, V> pai_;  // cada elemento aponta para o "chefe" do grupo
    std::map<V, int> tamanho_;
};

template <typename V>
struct ArvoreGeradora {
    std::vector<std::tuple<V, V, double>> arestas;
    double custo = 0;
};

template <typename Grafo>
ArvoreGeradora<typename Grafo::Vertice> kruskal(const Grafo& grafo) {
    using V = typename Grafo::Vertice;
    if (grafo.dirigido()) throw std::invalid_argument("AGM é definida para grafos não dirigidos");
    std::vector<V> vertices(grafo.vertices().begin(), grafo.vertices().end());
    UniaoBusca<V> conjuntos(vertices);
    auto arestas = grafo.arestas();
    std::stable_sort(arestas.begin(), arestas.end(),
                     [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

    ArvoreGeradora<V> resultado;
    for (const auto& [origem, destino, peso] : arestas) {
        if (conjuntos.unir(origem, destino)) {  // false = formaria ciclo
            resultado.arestas.emplace_back(origem, destino, peso);
            resultado.custo += peso;
            if (static_cast<int>(resultado.arestas.size()) == grafo.numero_vertices() - 1) break;
        }
    }
    return resultado;
}

// Árvore da componente de `inicio` (sem `inicio`, começa pelo primeiro vértice).
template <typename Grafo>
ArvoreGeradora<typename Grafo::Vertice> prim(const Grafo& grafo,
                                             std::optional<typename Grafo::Vertice> inicio = std::nullopt) {
    using V = typename Grafo::Vertice;
    if (grafo.dirigido()) throw std::invalid_argument("AGM é definida para grafos não dirigidos");
    ArvoreGeradora<V> resultado;
    if (grafo.numero_vertices() == 0) return resultado;
    if (!inicio)
        inicio = grafo.vertices()[0];
    else if (!grafo.contem(*inicio))
        throw std::out_of_range("Vértice não existe");

    // heap MÍNIMO com as arestas que saem da árvore: (peso, desempate, origem, destino).
    // O contador de desempate faz arestas de mesmo peso saírem na ordem de chegada.
    using Item = std::tuple<double, long, V, V>;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> heap;
    long contador = 0;
    std::set<V> na_arvore{*inicio};
    for (const auto& [vizinho, peso] : grafo.vizinhos(*inicio)) heap.push({peso, contador++, *inicio, vizinho});

    while (!heap.empty() && static_cast<int>(na_arvore.size()) < grafo.numero_vertices()) {
        auto [peso, _, origem, destino] = heap.top();
        heap.pop();
        if (na_arvore.count(destino)) continue;  // essa aresta fecharia ciclo
        na_arvore.insert(destino);
        resultado.arestas.emplace_back(origem, destino, peso);
        resultado.custo += peso;
        for (const auto& [vizinho, peso_vizinho] : grafo.vizinhos(destino))
            if (!na_arvore.count(vizinho)) heap.push({peso_vizinho, contador++, destino, vizinho});
    }
    return resultado;
}
