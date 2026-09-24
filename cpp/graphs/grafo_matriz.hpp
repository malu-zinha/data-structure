// GRAFO — MATRIZ DE ADJACÊNCIA
//
// - Matriz V x V: `matriz[u][v]` guarda o PESO da aresta u -> v, ou nada
//   (std::nullopt) se não existe aresta. Os vértices são os números 0, 1, ..., V-1.
// - Grafo NÃO dirigido: a matriz é SIMÉTRICA (matriz[u][v] == matriz[v][u]).
//
//       0  1  2  3
//     0[ -  4  2  - ]
//     1[ 4  -  1  - ]
//     2[ 2  1  -  7 ]
//     3[ -  -  7  - ]
//
// Quando usar
// - Matriz: grafo DENSO (muitas arestas) ou quando é preciso responder
//   "existe aresta (u, v)?" o tempo todo em O(1).
// - Lista: grafo ESPARSO (veja `grafo_lista.hpp`). Memória O(V²) x O(V+E).
//
// Pega-ratão
// - Percorrer os vizinhos de um vértice custa O(V), mesmo que ele tenha 1 vizinho.
// - Muitos livros usam 0 para "sem aresta"; aqui usamos std::optional para que
//   peso 0 seja uma aresta válida.
#pragma once

#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

class GrafoMatriz {
public:
    using Vertice = int;
    using Vizinho = std::pair<int, double>;
    using Aresta = std::tuple<int, int, double>;

    explicit GrafoMatriz(int num_vertices, bool dirigido = false)
        : dirigido_(dirigido), matriz_(num_vertices, std::vector<std::optional<double>>(num_vertices)) {
        if (num_vertices < 0) throw std::invalid_argument("Número de vértices não pode ser negativo");
    }

    bool dirigido() const { return dirigido_; }

    // ---------------------------------------------------------------- vértices
    // Aumenta a matriz em uma linha e uma coluna. Devolve o índice novo.
    int adicionar_vertice() {
        for (auto& linha : matriz_) linha.emplace_back();
        matriz_.emplace_back(matriz_.size() + 1);
        return numero_vertices() - 1;
    }

    std::vector<int> vertices() const {
        std::vector<int> resultado;
        for (int v = 0; v < numero_vertices(); v++) resultado.push_back(v);
        return resultado;
    }

    int numero_vertices() const { return static_cast<int>(matriz_.size()); }
    bool contem(int vertice) const { return vertice >= 0 && vertice < numero_vertices(); }

    // ---------------------------------------------------------------- arestas
    void adicionar_aresta(int origem, int destino, double peso = 1) {
        validar(origem);
        validar(destino);
        matriz_[origem][destino] = peso;
        if (!dirigido_) matriz_[destino][origem] = peso;  // matriz simétrica
    }

    void remover_aresta(int origem, int destino) {
        if (!existe_aresta(origem, destino)) throw std::out_of_range("Aresta não existe");
        matriz_[origem][destino].reset();
        if (!dirigido_) matriz_[destino][origem].reset();
    }

    bool existe_aresta(int origem, int destino) const {
        validar(origem);
        validar(destino);
        return matriz_[origem][destino].has_value();
    }

    double peso(int origem, int destino) const {
        if (!existe_aresta(origem, destino)) throw std::out_of_range("Aresta não existe");
        return *matriz_[origem][destino];
    }

    // Lista de (vizinho, peso) — precisa varrer a linha inteira: O(V).
    std::vector<Vizinho> vizinhos(int vertice) const {
        validar(vertice);
        std::vector<Vizinho> resultado;
        for (int v = 0; v < numero_vertices(); v++)
            if (matriz_[vertice][v]) resultado.emplace_back(v, *matriz_[vertice][v]);
        return resultado;
    }

    std::vector<Aresta> arestas() const {
        std::vector<Aresta> resultado;
        for (int origem = 0; origem < numero_vertices(); origem++)
            for (int destino = 0; destino < numero_vertices(); destino++)
                if (matriz_[origem][destino] && (dirigido_ || destino >= origem))  // metade de cima só
                    resultado.emplace_back(origem, destino, *matriz_[origem][destino]);
        return resultado;
    }

    int numero_arestas() const { return static_cast<int>(arestas().size()); }

    // ---------------------------------------------------------------- graus
    int grau_saida(int vertice) const { return static_cast<int>(vizinhos(vertice).size()); }

    int grau_entrada(int vertice) const {
        validar(vertice);
        int grau = 0;
        for (const auto& linha : matriz_)
            if (linha[vertice]) grau++;
        return grau;
    }

    int grau(int vertice) const {
        if (dirigido_) return grau_entrada(vertice) + grau_saida(vertice);
        int laco = matriz_[vertice][vertice] ? 1 : 0;
        return grau_saida(vertice) + laco;
    }

    std::string desenho() const {
        std::ostringstream saida;
        saida << "     ";
        for (int v = 0; v < numero_vertices(); v++) saida << std::setw(4) << v;
        saida << "\n";
        for (int origem = 0; origem < numero_vertices(); origem++) {
            saida << std::setw(3) << origem << " [";
            for (const auto& peso : matriz_[origem]) {
                if (peso)
                    saida << std::setw(4) << *peso;
                else
                    saida << std::setw(4) << ".";
            }
            saida << " ]\n";
        }
        return saida.str();
    }

private:
    bool dirigido_;
    std::vector<std::vector<std::optional<double>>> matriz_;

    void validar(int vertice) const {
        if (!contem(vertice)) throw std::out_of_range("Vértice não existe");
    }
};
