// GRAFO — LISTA DE ADJACÊNCIA
//
// - Cada vértice guarda a LISTA dos seus vizinhos (com o peso da aresta).
// - Aqui: um std::map de vértice -> vetor de (vizinho, peso). O rótulo do
//   vértice pode ser qualquer tipo comparável ("A", 0, "Recife"...).
// - Funciona para grafo DIRIGIDO (dígrafo) ou NÃO DIRIGIDO, com ou sem peso
//   (sem peso = todo mundo com peso 1).
//
//     A --4-- B          A: B(4), C(2)
//     |      /           B: A(4), C(1)
//     2     1            C: A(2), B(1)
//     |    /
//     C ---
//
// Lista x Matriz de adjacência
// | pergunta                       | lista       | matriz |
// |--------------------------------|-------------|--------|
// | memória                        | O(V + E)    | O(V²)  |
// | existe aresta (u, v)?          | O(grau(u))  | O(1)   |
// | percorrer vizinhos de v        | O(grau(v))  | O(V)   |
// | adicionar vértice              | O(log V)    | O(V²)  |
//
// Use LISTA quando o grafo é ESPARSO (poucas arestas) — o caso mais comum.
//
// Pega-ratão
// - Em grafo NÃO dirigido, toda aresta precisa ser inserida nos DOIS sentidos.
// - Grau em dígrafo se divide em grau de ENTRADA e de SAÍDA.
#pragma once

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

template <typename V = std::string>
class GrafoLista {
public:
    using Vertice = V;
    using Vizinho = std::pair<V, double>;       // (vizinho, peso)
    using Aresta = std::tuple<V, V, double>;    // (origem, destino, peso)

    explicit GrafoLista(bool dirigido = false) : dirigido_(dirigido) {}

    bool dirigido() const { return dirigido_; }

    // ---------------------------------------------------------------- vértices
    void adicionar_vertice(const V& vertice) {
        if (contem(vertice)) return;
        ordem_.push_back(vertice);
        adjacencia_[vertice];  // cria a lista vazia
    }

    void remover_vertice(const V& vertice) {
        validar(vertice);
        adjacencia_.erase(vertice);
        ordem_.erase(std::find(ordem_.begin(), ordem_.end(), vertice));
        for (auto& [_, vizinhos] : adjacencia_) {  // apaga as arestas que chegavam nele
            vizinhos.erase(std::remove_if(vizinhos.begin(), vizinhos.end(),
                                          [&](const Vizinho& v) { return v.first == vertice; }),
                           vizinhos.end());
        }
    }

    // Na ordem em que foram adicionados.
    const std::vector<V>& vertices() const { return ordem_; }
    int numero_vertices() const { return static_cast<int>(ordem_.size()); }
    bool contem(const V& vertice) const { return adjacencia_.count(vertice) > 0; }

    // ---------------------------------------------------------------- arestas
    // Se a aresta já existe, só troca o peso.
    void adicionar_aresta(const V& origem, const V& destino, double peso = 1) {
        adicionar_vertice(origem);
        adicionar_vertice(destino);
        ligar(origem, destino, peso);
        if (!dirigido_ && origem != destino) ligar(destino, origem, peso);  // o outro sentido
    }

    void remover_aresta(const V& origem, const V& destino) {
        if (!existe_aresta(origem, destino)) throw std::out_of_range("Aresta não existe");
        desligar(origem, destino);
        if (!dirigido_ && origem != destino) desligar(destino, origem);
    }

    bool existe_aresta(const V& origem, const V& destino) const {
        return contem(origem) && procurar(origem, destino) != adjacencia_.at(origem).end();
    }

    double peso(const V& origem, const V& destino) const {
        if (!existe_aresta(origem, destino)) throw std::out_of_range("Aresta não existe");
        return procurar(origem, destino)->second;
    }

    // Lista de (vizinho, peso). Em dígrafo, só os vizinhos de SAÍDA.
    const std::vector<Vizinho>& vizinhos(const V& vertice) const {
        validar(vertice);
        return adjacencia_.at(vertice);
    }

    // Lista de (origem, destino, peso). Sem repetir no grafo não dirigido.
    std::vector<Aresta> arestas() const {
        std::vector<Aresta> resultado;
        std::map<V, bool> ja_vistos;
        for (const V& origem : ordem_) {
            for (const auto& [destino, peso] : adjacencia_.at(origem))
                if (dirigido_ || !ja_vistos.count(destino)) resultado.emplace_back(origem, destino, peso);
            ja_vistos[origem] = true;
        }
        return resultado;
    }

    int numero_arestas() const { return static_cast<int>(arestas().size()); }

    // ---------------------------------------------------------------- graus
    int grau_saida(const V& vertice) const { return static_cast<int>(vizinhos(vertice).size()); }

    int grau_entrada(const V& vertice) const {
        validar(vertice);
        int grau = 0;
        for (const V& outro : ordem_)
            if (procurar(outro, vertice) != adjacencia_.at(outro).end()) grau++;
        return grau;
    }

    // Não dirigido: nº de arestas incidentes (laço conta 2). Dirigido: entrada + saída.
    int grau(const V& vertice) const {
        if (dirigido_) return grau_entrada(vertice) + grau_saida(vertice);
        int laco = existe_aresta(vertice, vertice) ? 1 : 0;
        return grau_saida(vertice) + laco;
    }

    // ---------------------------------------------------------------- outras
    // Grafo com todas as arestas INVERTIDAS (usado no algoritmo de Kosaraju).
    GrafoLista transposto() const {
        GrafoLista novo(dirigido_);
        for (const V& vertice : ordem_) novo.adicionar_vertice(vertice);
        for (const auto& [origem, destino, peso] : arestas()) novo.adicionar_aresta(destino, origem, peso);
        return novo;
    }

    std::string desenho() const {
        if (ordem_.empty()) return "(grafo vazio)\n";
        std::ostringstream saida;
        for (const V& vertice : ordem_) {
            saida << vertice << " -> ";
            const auto& vizinhos = adjacencia_.at(vertice);
            if (vizinhos.empty()) saida << "(isolado)";
            for (std::size_t i = 0; i < vizinhos.size(); i++)
                saida << (i > 0 ? ", " : "") << vizinhos[i].first << "(" << vizinhos[i].second << ")";
            saida << "\n";
        }
        return saida.str();
    }

private:
    bool dirigido_;
    std::vector<V> ordem_;                          // vértices na ordem de chegada
    std::map<V, std::vector<Vizinho>> adjacencia_;  // vértice -> vizinhos

    void validar(const V& vertice) const {
        if (!contem(vertice)) throw std::out_of_range("Vértice não existe");
    }

    typename std::vector<Vizinho>::const_iterator procurar(const V& origem, const V& destino) const {
        const auto& vizinhos = adjacencia_.at(origem);
        return std::find_if(vizinhos.begin(), vizinhos.end(), [&](const Vizinho& v) { return v.first == destino; });
    }

    void ligar(const V& origem, const V& destino, double peso) {
        auto& vizinhos = adjacencia_[origem];
        for (auto& [vizinho, p] : vizinhos) {
            if (vizinho == destino) {
                p = peso;
                return;
            }
        }
        vizinhos.emplace_back(destino, peso);
    }

    void desligar(const V& origem, const V& destino) {
        auto& vizinhos = adjacencia_[origem];
        vizinhos.erase(procurar(origem, destino));
    }
};
