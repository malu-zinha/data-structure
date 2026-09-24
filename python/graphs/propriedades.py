"""
PROPRIEDADES DE GRAFOS (tudo a partir de BFS/DFS)
=================================================

- componentes_conexas            -> "pedaços" separados de um grafo não dirigido
- eh_conexo                      -> o grafo é um pedaço só?
- tem_ciclo                      -> existe ciclo? (trata dirigido e não dirigido)
- ordenacao_topologica           -> ordem válida de tarefas num DAG (algoritmo de Kahn)
- ordenacao_topologica_dfs       -> a mesma coisa, via DFS (pós-ordem invertida)
- eh_bipartido                   -> dá para colorir com 2 cores sem vizinhos iguais?
- componentes_fortemente_conexas -> algoritmo de Kosaraju (dígrafos)
- pontes / pontos_de_articulacao -> algoritmo de Tarjan (o que "quebra" o grafo)

Complexidade: todos O(V + E) (Kruskal/ordenação à parte).

Rode a partir da pasta `python/`:  python3 graphs/propriedades.py
"""

from collections import deque


# ======================================================================
# Conectividade (grafos NÃO dirigidos)
# ======================================================================
def componentes_conexas(grafo):
    """Lista de listas: cada uma é um grupo de vértices ligados entre si."""
    if grafo.dirigido:
        raise ValueError("Use componentes_fortemente_conexas para dígrafos")
    visitados = set()
    componentes = []
    for inicio in grafo.vertices():
        if inicio in visitados:
            continue
        # BFS a partir de um vértice ainda não visitado = uma componente nova
        componente = []
        visitados.add(inicio)
        fila = deque([inicio])
        while fila:
            atual = fila.popleft()
            componente.append(atual)
            for vizinho, _ in grafo.vizinhos(atual):
                if vizinho not in visitados:
                    visitados.add(vizinho)
                    fila.append(vizinho)
        componentes.append(componente)
    return componentes


def eh_conexo(grafo):
    return len(componentes_conexas(grafo)) <= 1


# ======================================================================
# Ciclos
# ======================================================================
def tem_ciclo(grafo):
    """Dígrafo: procura ARESTA DE RETORNO para um vértice ainda "em aberto".
    Não dirigido: achar um visitado que não seja o pai já é ciclo."""
    if grafo.dirigido:
        return _tem_ciclo_dirigido(grafo)
    return _tem_ciclo_nao_dirigido(grafo)


def _tem_ciclo_dirigido(grafo):
    BRANCO, CINZA, PRETO = 0, 1, 2  # não visitado / em processamento / finalizado
    cor = {v: BRANCO for v in grafo.vertices()}

    def visitar(atual):
        cor[atual] = CINZA
        for vizinho, _ in grafo.vizinhos(atual):
            if cor[vizinho] == CINZA:   # voltou para alguém do caminho atual
                return True
            if cor[vizinho] == BRANCO and visitar(vizinho):
                return True
        cor[atual] = PRETO
        return False

    return any(cor[v] == BRANCO and visitar(v) for v in grafo.vertices())


def _tem_ciclo_nao_dirigido(grafo):
    visitados = set()

    def visitar(atual, pai):
        visitados.add(atual)
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in visitados:
                if visitar(vizinho, atual):
                    return True
            elif vizinho != pai:  # visitado e não é de onde eu vim -> ciclo
                return True
        return False

    return any(v not in visitados and visitar(v, None) for v in grafo.vertices())


# ======================================================================
# Ordenação topológica (só para DAG: dígrafo acíclico)
# ======================================================================
def ordenacao_topologica(grafo):
    """Algoritmo de KAHN (usa FILA + grau de entrada).

    Ideia: quem não depende de ninguém (grau de entrada 0) pode ir primeiro.
    Ao "remover" um vértice, os dependentes dele perdem uma dependência.
    Ex.: ordem de matérias respeitando pré-requisitos.
    """
    if not grafo.dirigido:
        raise ValueError("Ordenação topológica só existe em dígrafos")

    grau_entrada = {v: 0 for v in grafo.vertices()}
    for vertice in grafo.vertices():
        for vizinho, _ in grafo.vizinhos(vertice):
            grau_entrada[vizinho] += 1

    fila = deque(v for v in grafo.vertices() if grau_entrada[v] == 0)
    ordem = []
    while fila:
        atual = fila.popleft()
        ordem.append(atual)
        for vizinho, _ in grafo.vizinhos(atual):
            grau_entrada[vizinho] -= 1
            if grau_entrada[vizinho] == 0:
                fila.append(vizinho)

    if len(ordem) != grafo.numero_vertices():
        raise ValueError("O grafo tem ciclo: não existe ordenação topológica")
    return ordem


def ordenacao_topologica_dfs(grafo):
    """Mesma ideia via DFS: empilha na PÓS-ordem e inverte no final."""
    if not grafo.dirigido:
        raise ValueError("Ordenação topológica só existe em dígrafos")
    BRANCO, CINZA, PRETO = 0, 1, 2
    cor = {v: BRANCO for v in grafo.vertices()}
    ordem = []

    def visitar(atual):
        cor[atual] = CINZA
        for vizinho, _ in grafo.vizinhos(atual):
            if cor[vizinho] == CINZA:
                raise ValueError("O grafo tem ciclo: não existe ordenação topológica")
            if cor[vizinho] == BRANCO:
                visitar(vizinho)
        cor[atual] = PRETO
        ordem.append(atual)  # entra depois de todos os seus dependentes

    for vertice in grafo.vertices():
        if cor[vertice] == BRANCO:
            visitar(vertice)
    ordem.reverse()
    return ordem


# ======================================================================
# Grafo bipartido (2-coloração)
# ======================================================================
def eh_bipartido(grafo):
    """Devolve (True, cores) se der para pintar com 2 cores, senão (False, None).

    BFS pintando cada vizinho com a cor oposta. Se dois vizinhos ficarem com a
    mesma cor, existe ciclo ÍMPAR e o grafo não é bipartido.
    """
    if grafo.dirigido:
        raise ValueError("Bipartição é definida para grafos não dirigidos")
    cor = {}
    for inicio in grafo.vertices():
        if inicio in cor:
            continue
        cor[inicio] = 0
        fila = deque([inicio])
        while fila:
            atual = fila.popleft()
            for vizinho, _ in grafo.vizinhos(atual):
                if vizinho not in cor:
                    cor[vizinho] = 1 - cor[atual]  # cor oposta
                    fila.append(vizinho)
                elif cor[vizinho] == cor[atual]:
                    return False, None
    return True, cor


# ======================================================================
# Componentes fortemente conexas (Kosaraju) — só em dígrafos
# ======================================================================
def componentes_fortemente_conexas(grafo):
    """Grupos em que TODO vértice alcança todos os outros (ida e volta).

    Kosaraju, em 2 DFS:
    1. DFS no grafo original guardando a ordem de FINALIZAÇÃO.
    2. DFS no grafo TRANSPOSTO (arestas invertidas), pegando os vértices na
       ordem inversa de finalização. Cada árvore gerada é uma componente.
    """
    if not grafo.dirigido:
        raise ValueError("Use componentes_conexas para grafos não dirigidos")

    # ---- passo 1: ordem de finalização (DFS iterativa, evita estourar a pilha)
    visitados = set()
    ordem_finalizacao = []
    for inicio in grafo.vertices():
        if inicio in visitados:
            continue
        pilha = [(inicio, iter(grafo.vizinhos(inicio)))]
        visitados.add(inicio)
        while pilha:
            atual, vizinhos = pilha[-1]
            avancou = False
            for vizinho, _ in vizinhos:
                if vizinho not in visitados:
                    visitados.add(vizinho)
                    pilha.append((vizinho, iter(grafo.vizinhos(vizinho))))
                    avancou = True
                    break
            if not avancou:
                ordem_finalizacao.append(atual)
                pilha.pop()

    # ---- passo 2: DFS no transposto, na ordem inversa de finalização
    transposto = {v: [] for v in grafo.vertices()}
    for vertice in grafo.vertices():
        for vizinho, _ in grafo.vizinhos(vertice):
            transposto[vizinho].append(vertice)

    visitados = set()
    componentes = []
    for inicio in reversed(ordem_finalizacao):
        if inicio in visitados:
            continue
        componente = []
        pilha = [inicio]
        visitados.add(inicio)
        while pilha:
            atual = pilha.pop()
            componente.append(atual)
            for vizinho in transposto[atual]:
                if vizinho not in visitados:
                    visitados.add(vizinho)
                    pilha.append(vizinho)
        componentes.append(componente)
    return componentes


# ======================================================================
# Pontes e pontos de articulação (Tarjan) — grafos não dirigidos
# ======================================================================
def _tarjan(grafo):
    """Calcula, para cada vértice, o menor tempo de descoberta alcançável (low).

    - PONTE: aresta (u, v) em que low[v] > descoberta[u] -> tirar a aresta
      desconecta o grafo.
    - PONTO DE ARTICULAÇÃO: vértice que, se removido, desconecta o grafo.
    """
    if grafo.dirigido:
        raise ValueError("Pontes e articulações valem para grafos não dirigidos")
    descoberta, low = {}, {}
    pontes, articulacoes = [], set()
    tempo = 0

    def visitar(atual, pai):
        nonlocal tempo
        descoberta[atual] = low[atual] = tempo
        tempo += 1
        filhos = 0
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in descoberta:
                filhos += 1
                visitar(vizinho, atual)
                low[atual] = min(low[atual], low[vizinho])
                if low[vizinho] > descoberta[atual]:
                    pontes.append((atual, vizinho))
                if pai is not None and low[vizinho] >= descoberta[atual]:
                    articulacoes.add(atual)
            elif vizinho != pai:  # aresta de retorno
                low[atual] = min(low[atual], descoberta[vizinho])
        if pai is None and filhos > 1:  # raiz com 2+ filhos na árvore DFS
            articulacoes.add(atual)

    for vertice in grafo.vertices():
        if vertice not in descoberta:
            visitar(vertice, None)
    return pontes, articulacoes


def pontes(grafo):
    return _tarjan(grafo)[0]


def pontos_de_articulacao(grafo):
    return sorted(_tarjan(grafo)[1], key=str)


if __name__ == "__main__":
    from grafo_lista import GrafoLista

    grafo = GrafoLista()
    for origem, destino in (("A", "B"), ("B", "C"), ("C", "A"), ("C", "D"), ("E", "F")):
        grafo.adicionar_aresta(origem, destino)
    grafo.adicionar_vertice("G")
    print("Componentes conexas:", componentes_conexas(grafo), "| conexo?", eh_conexo(grafo))
    print("Tem ciclo?", tem_ciclo(grafo))
    print("Pontes:", pontes(grafo), "| articulações:", pontos_de_articulacao(grafo))

    ciclo_par = GrafoLista()
    for origem, destino in ((1, 2), (2, 3), (3, 4), (4, 1)):
        ciclo_par.adicionar_aresta(origem, destino)
    print("\nCiclo de 4 é bipartido?", eh_bipartido(ciclo_par)[0])
    ciclo_par.adicionar_aresta(4, 2)
    print("Depois de criar um ciclo ímpar:", eh_bipartido(ciclo_par)[0])

    materias = GrafoLista(dirigido=True)
    for pre, pos in (("Algoritmos", "Estrutura de Dados"), ("Cálculo 1", "Cálculo 2"),
                     ("Estrutura de Dados", "Banco de Dados"),
                     ("Estrutura de Dados", "Compiladores"), ("Cálculo 2", "Física")):
        materias.adicionar_aresta(pre, pos)
    print("\nOrdem para cursar (Kahn):", ordenacao_topologica(materias))
    print("Ordem para cursar (DFS): ", ordenacao_topologica_dfs(materias))

    digrafo = GrafoLista(dirigido=True)
    for origem, destino in (("A", "B"), ("B", "C"), ("C", "A"), ("B", "D"),
                            ("D", "E"), ("E", "D"), ("F", "E")):
        digrafo.adicionar_aresta(origem, destino)
    print("\nComponentes fortemente conexas:", componentes_fortemente_conexas(digrafo))
