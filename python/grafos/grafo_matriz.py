"""
GRAFO — MATRIZ DE ADJACÊNCIA
============================

- Matriz V x V: `matriz[u][v]` guarda o PESO da aresta u -> v, ou None se não
  existe aresta. Os vértices são os números 0, 1, ..., V-1.
- Grafo NÃO dirigido: a matriz é SIMÉTRICA (matriz[u][v] == matriz[v][u]).

      0  1  2  3
    0[ -  4  2  - ]
    1[ 4  -  1  - ]
    2[ 2  1  -  7 ]
    3[ -  -  7  - ]

Quando usar
-----------
- Matriz: grafo DENSO (muitas arestas) ou quando é preciso responder
  "existe aresta (u, v)?" o tempo todo em O(1).
- Lista: grafo ESPARSO (veja `grafos/grafo_lista.py`). Memória O(V²) x O(V+E).

Pega-ratão
----------
- `[[None] * n] * n` cria n referências para a MESMA linha! Sempre use
  uma list comprehension: `[[None] * n for _ in range(n)]`.
- Percorrer os vizinhos de um vértice custa O(V), mesmo que ele tenha 1 vizinho.
- Muitos livros usam 0 para "sem aresta"; aqui usamos None para que peso 0 seja
  uma aresta válida.
"""


class GrafoMatriz:
    def __init__(self, num_vertices, dirigido=False):
        if num_vertices < 0:
            raise ValueError("Número de vértices não pode ser negativo")
        self.dirigido = dirigido
        self._n = num_vertices
        # CUIDADO: uma list comprehension por linha (veja o pega-ratão acima)
        self._matriz = [[None] * num_vertices for _ in range(num_vertices)]

    # ------------------------------------------------------------------
    # Vértices
    # ------------------------------------------------------------------
    def adicionar_vertice(self):
        """Aumenta a matriz em uma linha e uma coluna. Devolve o índice novo."""
        for linha in self._matriz:
            linha.append(None)
        self._n += 1
        self._matriz.append([None] * self._n)
        return self._n - 1

    def vertices(self):
        return list(range(self._n))

    def numero_vertices(self):
        return self._n

    # ------------------------------------------------------------------
    # Arestas
    # ------------------------------------------------------------------
    def adicionar_aresta(self, origem, destino, peso=1):
        self._validar(origem)
        self._validar(destino)
        self._matriz[origem][destino] = peso
        if not self.dirigido:
            self._matriz[destino][origem] = peso  # matriz simétrica

    def remover_aresta(self, origem, destino):
        if not self.existe_aresta(origem, destino):
            raise KeyError(f"Aresta ({origem}, {destino}) não existe")
        self._matriz[origem][destino] = None
        if not self.dirigido:
            self._matriz[destino][origem] = None

    def existe_aresta(self, origem, destino):
        self._validar(origem)
        self._validar(destino)
        return self._matriz[origem][destino] is not None

    def peso(self, origem, destino):
        if not self.existe_aresta(origem, destino):
            raise KeyError(f"Aresta ({origem}, {destino}) não existe")
        return self._matriz[origem][destino]

    def vizinhos(self, vertice):
        """Lista de (vizinho, peso) — precisa varrer a linha inteira: O(V)."""
        self._validar(vertice)
        return [(v, peso) for v, peso in enumerate(self._matriz[vertice]) if peso is not None]

    def arestas(self):
        resultado = []
        for origem in range(self._n):
            for destino in range(self._n):
                peso = self._matriz[origem][destino]
                if peso is None:
                    continue
                if self.dirigido or destino >= origem:  # metade de cima só
                    resultado.append((origem, destino, peso))
        return resultado

    def numero_arestas(self):
        return len(self.arestas())

    # ------------------------------------------------------------------
    # Graus
    # ------------------------------------------------------------------
    def grau_saida(self, vertice):
        self._validar(vertice)
        return sum(1 for peso in self._matriz[vertice] if peso is not None)

    def grau_entrada(self, vertice):
        self._validar(vertice)
        return sum(1 for linha in self._matriz if linha[vertice] is not None)

    def grau(self, vertice):
        self._validar(vertice)
        if self.dirigido:
            return self.grau_entrada(vertice) + self.grau_saida(vertice)
        laco = 1 if self._matriz[vertice][vertice] is not None else 0
        return self.grau_saida(vertice) + laco

    def _validar(self, vertice):
        if not isinstance(vertice, int) or not 0 <= vertice < self._n:
            raise KeyError(f"Vértice {vertice!r} não existe")

    def __contains__(self, vertice):
        return isinstance(vertice, int) and 0 <= vertice < self._n

    def __len__(self):
        return self._n

    def __str__(self):
        cabecalho = "     " + " ".join(f"{v:>3}" for v in range(self._n))
        linhas = [cabecalho]
        for origem in range(self._n):
            celulas = " ".join(f"{'  .' if p is None else f'{p:>3}'}" for p in self._matriz[origem])
            linhas.append(f"{origem:>3} [{celulas} ]")
        return "\n".join(linhas)


if __name__ == "__main__":
    grafo = GrafoMatriz(4)
    grafo.adicionar_aresta(0, 1, 4)
    grafo.adicionar_aresta(0, 2, 2)
    grafo.adicionar_aresta(1, 2, 1)
    grafo.adicionar_aresta(2, 3, 7)
    print(grafo)
    print("Vizinhos de 2:", grafo.vizinhos(2), "| grau de 2:", grafo.grau(2))
    print("Arestas:", grafo.arestas())
    novo = grafo.adicionar_vertice()
    print(f"Vértice {novo} adicionado -> agora {grafo.numero_vertices()} vértices")
