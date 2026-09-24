"""
CAMINHOS MÍNIMOS (grafos com PESO)
==================================

| algoritmo      | resolve                  | pesos negativos | complexidade      |
|----------------|--------------------------|-----------------|-------------------|
| BFS            | 1 origem, SEM peso       | -               | O(V + E)          |
| Dijkstra       | 1 origem -> todos        | NÃO aceita      | O((V + E) log V)  |
| Bellman-Ford   | 1 origem -> todos        | aceita (e       | O(V * E)          |
|                |                          | detecta ciclo   |                   |
|                |                          | negativo)       |                   |
| Floyd-Warshall | TODOS -> todos           | aceita          | O(V³)             |

Ideia de cada um
----------------
- Dijkstra: sempre fecha o vértice não visitado de MENOR distância conhecida
  (guloso) e RELAXA as arestas dele. Usa fila de prioridade (heap).
- Bellman-Ford: relaxa TODAS as arestas V-1 vezes. Se ainda melhorar na
  V-ésima passada, existe ciclo negativo.
- Floyd-Warshall: programação dinâmica. Para cada vértice k, testa se passar
  por k encurta o caminho entre todos os pares i -> j.

Relaxar uma aresta (u, v):
    se distancia[u] + peso(u, v) < distancia[v]:
        distancia[v] = distancia[u] + peso(u, v);  pai[v] = u

Pega-ratão
----------
- Dijkstra com peso negativo dá resposta ERRADA (ele fecha um vértice cedo
  demais e nunca revisa). Para pesos negativos use Bellman-Ford.
- Vértice inalcançável fica com distância infinita (`float('inf')`).

Rode a partir da pasta `python/`:  python3 graphs/caminhos_minimos.py
"""

import heapq  # heap mínimo da biblioteca padrão (veja heap/heap.py)
from itertools import count

from buscas import reconstruir_caminho

INFINITO = float("inf")


def dijkstra(grafo, origem):
    """Devolve (distancia, pai) com o menor custo de `origem` até cada vértice."""
    if origem not in grafo:
        raise KeyError(f"Vértice {origem!r} não existe")

    distancia = {v: INFINITO for v in grafo.vertices()}
    pai = {v: None for v in grafo.vertices()}
    distancia[origem] = 0

    fechados = set()
    contador = count()  # desempate: evita comparar vértices de tipos diferentes
    heap = [(0, next(contador), origem)]

    while heap:
        custo, _, atual = heapq.heappop(heap)
        if atual in fechados:
            continue  # entrada velha, já melhorada depois
        fechados.add(atual)

        for vizinho, peso in grafo.vizinhos(atual):
            if peso < 0:
                raise ValueError("Dijkstra não aceita pesos negativos")
            if custo + peso < distancia[vizinho]:      # relaxamento
                distancia[vizinho] = custo + peso
                pai[vizinho] = atual
                heapq.heappush(heap, (distancia[vizinho], next(contador), vizinho))

    return distancia, pai


def bellman_ford(grafo, origem):
    """Aceita peso negativo. Levanta ValueError se houver ciclo negativo."""
    if origem not in grafo:
        raise KeyError(f"Vértice {origem!r} não existe")

    distancia = {v: INFINITO for v in grafo.vertices()}
    pai = {v: None for v in grafo.vertices()}
    distancia[origem] = 0

    # Em grafo não dirigido cada aresta vale nos dois sentidos.
    arestas = [(u, v, peso) for u in grafo.vertices() for v, peso in grafo.vizinhos(u)]

    for _ in range(grafo.numero_vertices() - 1):
        mudou = False
        for u, v, peso in arestas:
            if distancia[u] + peso < distancia[v]:
                distancia[v] = distancia[u] + peso
                pai[v] = u
                mudou = True
        if not mudou:
            break  # nada melhorou: já convergiu

    # V-ésima passada: se ainda melhora, é porque existe ciclo negativo
    for u, v, peso in arestas:
        if distancia[u] + peso < distancia[v]:
            raise ValueError("O grafo tem ciclo de peso negativo")

    return distancia, pai


def floyd_warshall(grafo):
    """Menor distância entre TODOS os pares.

    Devolve (distancia, proximo), dicionários de dicionários. `proximo[u][v]` é
    o próximo vértice do caminho de u até v (use `caminho_floyd` para montá-lo).
    """
    vertices = grafo.vertices()
    distancia = {u: {v: INFINITO for v in vertices} for u in vertices}
    proximo = {u: {v: None for v in vertices} for u in vertices}

    for u in vertices:
        distancia[u][u] = 0
        for v, peso in grafo.vizinhos(u):
            if peso < distancia[u][v]:  # guarda a aresta mais barata
                distancia[u][v] = peso
                proximo[u][v] = v

    # k = vértice intermediário permitido; i -> k -> j encurta o caminho?
    for k in vertices:
        for i in vertices:
            if distancia[i][k] == INFINITO:
                continue
            for j in vertices:
                if distancia[i][k] + distancia[k][j] < distancia[i][j]:
                    distancia[i][j] = distancia[i][k] + distancia[k][j]
                    proximo[i][j] = proximo[i][k]

    for v in vertices:
        if distancia[v][v] < 0:
            raise ValueError("O grafo tem ciclo de peso negativo")

    return distancia, proximo


def caminho_floyd(proximo, origem, destino):
    """Monta o caminho origem -> destino usando a matriz `proximo`."""
    if proximo[origem][destino] is None and origem != destino:
        return None
    caminho = [origem]
    atual = origem
    while atual != destino:
        atual = proximo[atual][destino]
        caminho.append(atual)
    return caminho


def caminho_minimo(grafo, origem, destino):
    """Atalho: roda Dijkstra e devolve (caminho, custo)."""
    distancia, pai = dijkstra(grafo, origem)
    if distancia[destino] == INFINITO:
        return None, INFINITO
    return reconstruir_caminho(pai, origem, destino), distancia[destino]


if __name__ == "__main__":
    from grafo_lista import GrafoLista

    mapa = GrafoLista()
    for origem, destino, km in (("A", "B", 4), ("A", "C", 2), ("B", "C", 1),
                                ("B", "D", 5), ("C", "D", 8), ("C", "E", 10),
                                ("D", "E", 2), ("D", "F", 6), ("E", "F", 3)):
        mapa.adicionar_aresta(origem, destino, km)
    print(mapa)

    distancia, pai = dijkstra(mapa, "A")
    print("\nDijkstra a partir de A:", distancia)
    caminho, custo = caminho_minimo(mapa, "A", "F")
    print(f"Menor caminho A -> F: {' -> '.join(caminho)} (custo {custo})")

    distancia_bf, _ = bellman_ford(mapa, "A")
    print("Bellman-Ford dá o mesmo resultado?", distancia_bf == distancia)

    todos, proximo = floyd_warshall(mapa)
    print("\nFloyd-Warshall (distância de cada par):")
    vertices = mapa.vertices()
    print("      " + "  ".join(f"{v:>3}" for v in vertices))
    for u in vertices:
        print(f"  {u} " + "  ".join(f"{todos[u][v]:>3}" for v in vertices))
    print("Caminho C -> F pela matriz:", caminho_floyd(proximo, "C", "F"))

    negativo = GrafoLista(dirigido=True)
    for origem, destino, peso in (("S", "A", 4), ("S", "B", 5), ("A", "B", -3), ("B", "C", 2)):
        negativo.adicionar_aresta(origem, destino, peso)
    print("\nCom peso negativo, Bellman-Ford:", bellman_ford(negativo, "S")[0])
    try:
        dijkstra(negativo, "S")
    except ValueError as erro:
        print("Dijkstra recusa:", erro)
