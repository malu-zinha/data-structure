"""
GRAFO — LISTA DE ADJACÊNCIA
===========================

- Cada vértice guarda a LISTA dos seus vizinhos (com o peso da aresta).
- Em C seria um vetor de listas encadeadas; em Python usamos um dicionário
  de dicionários: {vertice: {vizinho: peso}}. Assim o rótulo do vértice pode
  ser qualquer coisa ("A", 0, "Recife"...).
- Funciona para grafo DIRIGIDO (dígrafo) ou NÃO DIRIGIDO, com ou sem peso
  (sem peso = todo mundo com peso 1).

    A --4-- B          A: B(4), C(2)
    |      /           B: A(4), C(1)
    2     1            C: A(2), B(1)
    |    /
    C ---

Lista x Matriz de adjacência
----------------------------
| pergunta                       | lista       | matriz |
|--------------------------------|-------------|--------|
| memória                        | O(V + E)    | O(V²)  |
| existe aresta (u, v)?          | O(1)*       | O(1)   |
| percorrer vizinhos de v        | O(grau(v))  | O(V)   |
| adicionar vértice              | O(1)        | O(V²)  |
(*) O(1) aqui porque usamos dicionário; com lista encadeada seria O(grau(v)).

Use LISTA quando o grafo é ESPARSO (poucas arestas) — o caso mais comum.

Pega-ratão
----------
- Em grafo NÃO dirigido, toda aresta precisa ser inserida nos DOIS sentidos.
- Grau em dígrafo se divide em grau de ENTRADA e de SAÍDA.
"""


class GrafoLista:
    def __init__(self, dirigido=False):
        self.dirigido = dirigido
        self._adjacencia = {}  # {vertice: {vizinho: peso}}

    # ------------------------------------------------------------------
    # Vértices
    # ------------------------------------------------------------------
    def adicionar_vertice(self, vertice):
        if vertice not in self._adjacencia:
            self._adjacencia[vertice] = {}

    def remover_vertice(self, vertice):
        self._validar(vertice)
        del self._adjacencia[vertice]
        for vizinhos in self._adjacencia.values():
            vizinhos.pop(vertice, None)  # apaga as arestas que chegavam nele

    def vertices(self):
        return list(self._adjacencia)

    def numero_vertices(self):
        return len(self._adjacencia)

    # ------------------------------------------------------------------
    # Arestas
    # ------------------------------------------------------------------
    def adicionar_aresta(self, origem, destino, peso=1):
        self.adicionar_vertice(origem)
        self.adicionar_vertice(destino)
        self._adjacencia[origem][destino] = peso
        if not self.dirigido and origem != destino:
            self._adjacencia[destino][origem] = peso  # o outro sentido

    def remover_aresta(self, origem, destino):
        if not self.existe_aresta(origem, destino):
            raise KeyError(f"Aresta ({origem!r}, {destino!r}) não existe")
        del self._adjacencia[origem][destino]
        if not self.dirigido and origem != destino:
            del self._adjacencia[destino][origem]

    def existe_aresta(self, origem, destino):
        return origem in self._adjacencia and destino in self._adjacencia[origem]

    def peso(self, origem, destino):
        if not self.existe_aresta(origem, destino):
            raise KeyError(f"Aresta ({origem!r}, {destino!r}) não existe")
        return self._adjacencia[origem][destino]

    def vizinhos(self, vertice):
        """Lista de (vizinho, peso). Em dígrafo, só os vizinhos de SAÍDA."""
        self._validar(vertice)
        return list(self._adjacencia[vertice].items())

    def arestas(self):
        """Lista de (origem, destino, peso). Sem repetir no grafo não dirigido."""
        resultado = []
        ja_vistos = set()
        for origem, vizinhos in self._adjacencia.items():
            for destino, peso in vizinhos.items():
                if self.dirigido or destino not in ja_vistos:
                    resultado.append((origem, destino, peso))
            ja_vistos.add(origem)
        return resultado

    def numero_arestas(self):
        return len(self.arestas())

    # ------------------------------------------------------------------
    # Graus
    # ------------------------------------------------------------------
    def grau_saida(self, vertice):
        self._validar(vertice)
        return len(self._adjacencia[vertice])

    def grau_entrada(self, vertice):
        self._validar(vertice)
        return sum(1 for vizinhos in self._adjacencia.values() if vertice in vizinhos)

    def grau(self, vertice):
        """Não dirigido: nº de arestas incidentes (laço conta 2).
        Dirigido: entrada + saída."""
        self._validar(vertice)
        if self.dirigido:
            return self.grau_entrada(vertice) + self.grau_saida(vertice)
        laco = 1 if vertice in self._adjacencia[vertice] else 0
        return len(self._adjacencia[vertice]) + laco

    # ------------------------------------------------------------------
    # Outras operações
    # ------------------------------------------------------------------
    def transposto(self):
        """Grafo com todas as arestas INVERTIDAS (usado no algoritmo de Kosaraju)."""
        novo = GrafoLista(dirigido=self.dirigido)
        for vertice in self.vertices():
            novo.adicionar_vertice(vertice)
        for origem, destino, peso in self.arestas():
            novo.adicionar_aresta(destino, origem, peso)
        return novo

    def _validar(self, vertice):
        if vertice not in self._adjacencia:
            raise KeyError(f"Vértice {vertice!r} não existe")

    def __contains__(self, vertice):
        return vertice in self._adjacencia

    def __len__(self):
        return self.numero_vertices()

    def __str__(self):
        linhas = []
        for vertice, vizinhos in self._adjacencia.items():
            ligacoes = ", ".join(f"{v}({p})" for v, p in vizinhos.items())
            linhas.append(f"{vertice} -> {ligacoes if ligacoes else '(isolado)'}")
        return "\n".join(linhas) if linhas else "(grafo vazio)"


if __name__ == "__main__":
    grafo = GrafoLista()
    grafo.adicionar_aresta("A", "B", 4)
    grafo.adicionar_aresta("A", "C", 2)
    grafo.adicionar_aresta("B", "C", 1)
    grafo.adicionar_aresta("C", "D", 7)
    grafo.adicionar_vertice("E")  # vértice isolado
    print(grafo)
    print("Vértices:", grafo.vertices(), "| arestas:", grafo.arestas())
    print("Vizinhos de C:", grafo.vizinhos("C"), "| grau de C:", grafo.grau("C"))
    print("Existe A-D?", grafo.existe_aresta("A", "D"), "| peso A-B:", grafo.peso("A", "B"))

    digrafo = GrafoLista(dirigido=True)
    for origem, destino in (("A", "B"), ("B", "C"), ("C", "A"), ("C", "D")):
        digrafo.adicionar_aresta(origem, destino)
    print("\nDígrafo:")
    print(digrafo)
    print("Grau de entrada de A:", digrafo.grau_entrada("A"), "| de saída:", digrafo.grau_saida("A"))
    print("Transposto:")
    print(digrafo.transposto())
