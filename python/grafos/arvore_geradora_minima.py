"""
ÁRVORE GERADORA MÍNIMA (AGM / MST)
==================================

Problema: ligar TODOS os vértices de um grafo não dirigido com peso, usando o
MENOR custo total e SEM formar ciclo. O resultado tem V-1 arestas (se o grafo
for conexo) e é uma árvore.
Ex.: puxar cabo de rede entre prédios gastando o mínimo possível.

Dois algoritmos GULOSOS clássicos:

KRUSKAL — pensa nas ARESTAS
  1. Ordena todas as arestas da mais barata para a mais cara.
  2. Pega a aresta se ela LIGAR DOIS GRUPOS DIFERENTES (senão formaria ciclo).
  3. Para quando tiver V-1 arestas.
  Complexidade: O(E log E), dominada pela ordenação.
  Precisa da estrutura UNION-FIND (conjuntos disjuntos) para saber, em tempo
  quase constante, se dois vértices já estão no mesmo grupo.

PRIM — pensa nos VÉRTICES
  1. Começa de um vértice qualquer.
  2. Sempre adiciona a aresta mais barata que liga a árvore a um vértice de FORA.
  3. Para quando todos os vértices entrarem.
  Complexidade: O(E log V) com fila de prioridade (heap).

Qual usar? Kruskal vai bem em grafo esparso; Prim, em grafo denso.
Se o grafo for desconexo, os dois devolvem uma FLORESTA (Prim só a componente
do vértice inicial; Kruskal, a de todas as componentes).

Rode a partir da pasta `python/`:  python3 -m grafos.arvore_geradora_minima
"""

import heapq
from itertools import count


class UniaoBusca:
    """UNION-FIND / Conjuntos Disjuntos (DSU).

    Responde "esses dois estão no mesmo grupo?" e junta grupos.
    Otimizações: união por TAMANHO + compressão de caminho -> quase O(1).
    """

    def __init__(self, elementos=()):
        self._pai = {}       # cada elemento aponta para o "chefe" do grupo
        self._tamanho = {}
        for elemento in elementos:
            self.adicionar(elemento)

    def adicionar(self, elemento):
        if elemento not in self._pai:
            self._pai[elemento] = elemento  # começa sozinho, chefe de si mesmo
            self._tamanho[elemento] = 1

    def encontrar(self, elemento):
        """Devolve o representante do grupo, encurtando o caminho na volta."""
        if elemento not in self._pai:
            raise KeyError(f"Elemento {elemento!r} não existe")
        raiz = elemento
        while self._pai[raiz] != raiz:
            raiz = self._pai[raiz]
        while self._pai[elemento] != raiz:  # compressão de caminho
            self._pai[elemento], elemento = raiz, self._pai[elemento]
        return raiz

    def unir(self, a, b):
        """Junta os grupos de a e b. Devolve False se já eram o mesmo grupo."""
        raiz_a, raiz_b = self.encontrar(a), self.encontrar(b)
        if raiz_a == raiz_b:
            return False
        if self._tamanho[raiz_a] < self._tamanho[raiz_b]:  # o menor entra no maior
            raiz_a, raiz_b = raiz_b, raiz_a
        self._pai[raiz_b] = raiz_a
        self._tamanho[raiz_a] += self._tamanho[raiz_b]
        return True

    def mesmo_conjunto(self, a, b):
        return self.encontrar(a) == self.encontrar(b)

    def quantidade_de_grupos(self):
        return sum(1 for e in self._pai if self._pai[e] == e)


def kruskal(grafo):
    """Devolve (arestas_escolhidas, custo_total)."""
    if grafo.dirigido:
        raise ValueError("AGM é definida para grafos não dirigidos")

    conjuntos = UniaoBusca(grafo.vertices())
    arestas_ordenadas = sorted(grafo.arestas(), key=lambda aresta: aresta[2])

    escolhidas = []
    custo = 0
    for origem, destino, peso in arestas_ordenadas:
        if conjuntos.unir(origem, destino):  # False = formaria ciclo
            escolhidas.append((origem, destino, peso))
            custo += peso
            if len(escolhidas) == grafo.numero_vertices() - 1:
                break
    return escolhidas, custo


def prim(grafo, inicio=None):
    """Devolve (arestas_escolhidas, custo_total) da componente de `inicio`."""
    if grafo.dirigido:
        raise ValueError("AGM é definida para grafos não dirigidos")
    if grafo.numero_vertices() == 0:
        return [], 0
    if inicio is None:
        inicio = grafo.vertices()[0]
    elif inicio not in grafo:
        raise KeyError(f"Vértice {inicio!r} não existe")

    na_arvore = {inicio}
    escolhidas = []
    custo = 0
    contador = count()
    # heap com as arestas que saem da árvore: (peso, desempate, origem, destino)
    heap = [(peso, next(contador), inicio, vizinho) for vizinho, peso in grafo.vizinhos(inicio)]
    heapq.heapify(heap)

    while heap and len(na_arvore) < grafo.numero_vertices():
        peso, _, origem, destino = heapq.heappop(heap)
        if destino in na_arvore:
            continue  # essa aresta fecharia ciclo
        na_arvore.add(destino)
        escolhidas.append((origem, destino, peso))
        custo += peso
        for vizinho, peso_vizinho in grafo.vizinhos(destino):
            if vizinho not in na_arvore:
                heapq.heappush(heap, (peso_vizinho, next(contador), destino, vizinho))
    return escolhidas, custo


if __name__ == "__main__":
    from grafos.grafo_lista import GrafoLista

    rede = GrafoLista()
    for origem, destino, custo in (("A", "B", 4), ("A", "C", 3), ("B", "C", 1),
                                   ("B", "D", 2), ("C", "D", 4), ("D", "E", 2),
                                   ("C", "E", 6), ("E", "F", 3), ("D", "F", 5)):
        rede.adicionar_aresta(origem, destino, custo)
    print(rede)

    arestas, custo = kruskal(rede)
    print("\nKruskal:", arestas)
    print("Custo total:", custo)

    arestas_prim, custo_prim = prim(rede, "A")
    print("\nPrim:", arestas_prim)
    print("Custo total:", custo_prim, "| mesmo custo do Kruskal?", custo == custo_prim)

    print("\nUnion-Find sozinho:")
    conjuntos = UniaoBusca(["A", "B", "C", "D"])
    conjuntos.unir("A", "B")
    conjuntos.unir("C", "D")
    print("A e B juntos?", conjuntos.mesmo_conjunto("A", "B"))
    print("A e C juntos?", conjuntos.mesmo_conjunto("A", "C"))
    conjuntos.unir("B", "C")
    print("Depois de unir B e C -> A e D juntos?", conjuntos.mesmo_conjunto("A", "D"))
    print("Quantidade de grupos:", conjuntos.quantidade_de_grupos())
