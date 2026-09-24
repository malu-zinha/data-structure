"""
BUSCAS EM GRAFOS — BFS e DFS
============================

Funcionam com QUALQUER das duas representações (`GrafoLista` ou `GrafoMatriz`),
porque só usam `grafo.vertices()` e `grafo.vizinhos(v)`.

BFS (Busca em LARGURA) — usa FILA
---------------------------------
Visita por "ondas": primeiro os vizinhos, depois os vizinhos dos vizinhos.
Acha o MENOR caminho em número de arestas (grafo sem peso).

DFS (Busca em PROFUNDIDADE) — usa PILHA (ou recursão)
-----------------------------------------------------
Vai fundo por um caminho até não dar mais e então volta (backtracking).
Base para: detectar ciclo, ordenação topológica, componentes fortemente conexas.

Complexidade das duas: O(V + E) com lista de adjacência; O(V²) com matriz.

Pega-ratão
----------
- MARQUE o vértice como visitado ao ENFILEIRAR na BFS (não ao desenfileirar),
  senão ele entra várias vezes na fila.
- Sem a marcação de visitados, um ciclo faz o algoritmo rodar para sempre.
- A DFS recursiva pode estourar a pilha em grafos enormes; use a iterativa.

Rode a partir da pasta `python/`:  python3 graphs/buscas.py
"""

from collections import deque


def _validar(grafo, vertice):
    if vertice not in grafo:
        raise KeyError(f"Vértice {vertice!r} não existe")


# ======================================================================
# BFS — Busca em largura
# ======================================================================
def bfs(grafo, origem):
    """Devolve a ordem de visita a partir de `origem`."""
    _validar(grafo, origem)
    visitados = {origem}
    fila = deque([origem])
    ordem = []
    while fila:
        atual = fila.popleft()      # sai o mais antigo (FIFO)
        ordem.append(atual)
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in visitados:
                visitados.add(vizinho)  # marca AO ENFILEIRAR
                fila.append(vizinho)
    return ordem


def bfs_distancias(grafo, origem):
    """Devolve (distancia, pai) em NÚMERO DE ARESTAS (ignora os pesos).

    Só aparecem os vértices alcançáveis a partir da origem.
    """
    _validar(grafo, origem)
    distancia = {origem: 0}
    pai = {origem: None}
    fila = deque([origem])
    while fila:
        atual = fila.popleft()
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in distancia:
                distancia[vizinho] = distancia[atual] + 1
                pai[vizinho] = atual
                fila.append(vizinho)
    return distancia, pai


def reconstruir_caminho(pai, origem, destino):
    """Monta o caminho origem -> destino andando de trás para frente pelos pais."""
    if destino not in pai:
        return None
    caminho = []
    atual = destino
    while atual is not None:
        caminho.append(atual)
        if atual == origem:
            break
        atual = pai.get(atual)
    caminho.reverse()
    return caminho if caminho and caminho[0] == origem else None


def menor_caminho_bfs(grafo, origem, destino):
    """Caminho com MENOS ARESTAS entre dois vértices (ou None)."""
    _, pai = bfs_distancias(grafo, origem)
    return reconstruir_caminho(pai, origem, destino)


# ======================================================================
# DFS — Busca em profundidade
# ======================================================================
def dfs(grafo, origem):
    """Versão RECURSIVA. Devolve a ordem de visita."""
    _validar(grafo, origem)
    visitados = set()
    ordem = []

    def visitar(atual):
        visitados.add(atual)
        ordem.append(atual)
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in visitados:
                visitar(vizinho)

    visitar(origem)
    return ordem


def dfs_iterativa(grafo, origem):
    """Mesma ordem da recursiva, trocando a recursão por uma PILHA explícita."""
    _validar(grafo, origem)
    visitados = set()
    ordem = []
    pilha = [origem]
    while pilha:
        atual = pilha.pop()
        if atual in visitados:
            continue
        visitados.add(atual)
        ordem.append(atual)
        # empilha ao contrário para visitar os vizinhos na ordem original
        for vizinho, _ in reversed(grafo.vizinhos(atual)):
            if vizinho not in visitados:
                pilha.append(vizinho)
    return ordem


def dfs_completa(grafo):
    """DFS em TODOS os vértices (pega também as partes desconexas).

    Devolve (descoberta, finalizacao, pai) com os tempos do algoritmo do CLRS:
    descoberta = quando o vértice ficou cinza; finalizacao = quando ficou preto.
    """
    descoberta, finalizacao, pai = {}, {}, {}
    tempo = 0

    def visitar(atual):
        nonlocal tempo
        descoberta[atual] = tempo
        tempo += 1
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in descoberta:
                pai[vizinho] = atual
                visitar(vizinho)
        finalizacao[atual] = tempo
        tempo += 1

    for vertice in grafo.vertices():
        if vertice not in descoberta:
            pai[vertice] = None
            visitar(vertice)
    return descoberta, finalizacao, pai


def existe_caminho(grafo, origem, destino):
    _validar(grafo, destino)
    return destino in bfs(grafo, origem)


def todos_os_caminhos(grafo, origem, destino):
    """Todos os caminhos SIMPLES (sem repetir vértice) entre origem e destino.

    Usa backtracking: entra no vizinho, e ao voltar DESMARCA para que ele possa
    ser usado em outro caminho. Cuidado: a quantidade de caminhos pode explodir.
    """
    _validar(grafo, origem)
    _validar(grafo, destino)
    caminhos = []
    caminho_atual = [origem]
    no_caminho = {origem}

    def explorar(atual):
        if atual == destino:
            caminhos.append(list(caminho_atual))
            return
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in no_caminho:
                no_caminho.add(vizinho)
                caminho_atual.append(vizinho)
                explorar(vizinho)
                caminho_atual.pop()        # desfaz (backtracking)
                no_caminho.remove(vizinho)

    explorar(origem)
    return caminhos


if __name__ == "__main__":
    from grafo_lista import GrafoLista

    grafo = GrafoLista()
    for origem, destino in (("A", "B"), ("A", "C"), ("B", "D"), ("C", "D"),
                            ("D", "E"), ("E", "F"), ("C", "F")):
        grafo.adicionar_aresta(origem, destino)
    print(grafo)

    print("\nBFS a partir de A:", bfs(grafo, "A"))
    print("DFS a partir de A:", dfs(grafo, "A"))
    print("DFS iterativa:    ", dfs_iterativa(grafo, "A"))

    distancia, pai = bfs_distancias(grafo, "A")
    print("\nDistâncias (em arestas) a partir de A:", distancia)
    print("Menor caminho A -> F:", menor_caminho_bfs(grafo, "A", "F"))
    print("Todos os caminhos A -> F:", todos_os_caminhos(grafo, "A", "F"))

    descoberta, finalizacao, _ = dfs_completa(grafo)
    print("\nTempos de descoberta:", descoberta)
    print("Tempos de finalização:", finalizacao)
