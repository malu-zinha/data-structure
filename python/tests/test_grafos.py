"""Testes dos grafos: representações, buscas, propriedades, caminhos mínimos e AGM.

Estratégia: gerar grafos aleatórios e comparar cada algoritmo com uma resposta
obtida por FORÇA BRUTA (lenta, mas obviamente correta) ou com outro algoritmo
que resolve o mesmo problema por um caminho diferente.

Rode a partir da pasta `python/`:  python3 -m unittest
"""

import itertools
import random
import unittest

from arvore_geradora_minima import UniaoBusca, kruskal, prim
from buscas import (
    bfs,
    bfs_distancias,
    dfs,
    dfs_completa,
    dfs_iterativa,
    existe_caminho,
    menor_caminho_bfs,
    todos_os_caminhos,
)
from caminhos_minimos import (
    INFINITO,
    bellman_ford,
    caminho_floyd,
    caminho_minimo,
    dijkstra,
    floyd_warshall,
)
from grafo_lista import GrafoLista
from grafo_matriz import GrafoMatriz
from propriedades import (
    componentes_conexas,
    componentes_fortemente_conexas,
    eh_bipartido,
    eh_conexo,
    ordenacao_topologica,
    ordenacao_topologica_dfs,
    pontes,
    pontos_de_articulacao,
    tem_ciclo,
)


# ----------------------------------------------------------------------
# Geradores de grafos para os testes
# ----------------------------------------------------------------------
def grafo_aleatorio(sorteio, quantidade, probabilidade=0.35, dirigido=False, peso_maximo=9):
    grafo = GrafoLista(dirigido=dirigido)
    for vertice in range(quantidade):
        grafo.adicionar_vertice(vertice)
    for origem in range(quantidade):
        for destino in range(quantidade):
            if origem == destino:
                continue
            if not dirigido and destino < origem:
                continue
            if sorteio.random() < probabilidade:
                grafo.adicionar_aresta(origem, destino, sorteio.randint(1, peso_maximo))
    return grafo


def grafo_conexo_aleatorio(sorteio, quantidade, extras=3, peso_maximo=9):
    """Uma árvore aleatória (garante conexo) + algumas arestas extras."""
    grafo = GrafoLista()
    grafo.adicionar_vertice(0)
    for vertice in range(1, quantidade):
        pai = sorteio.randrange(vertice)
        grafo.adicionar_aresta(pai, vertice, sorteio.randint(1, peso_maximo))
    for _ in range(extras):
        origem, destino = sorteio.randrange(quantidade), sorteio.randrange(quantidade)
        if origem != destino and not grafo.existe_aresta(origem, destino):
            grafo.adicionar_aresta(origem, destino, sorteio.randint(1, peso_maximo))
    return grafo


def dag_aleatorio(sorteio, quantidade, probabilidade=0.3):
    """Só cria arestas de um vértice menor para um maior: nunca forma ciclo."""
    grafo = GrafoLista(dirigido=True)
    for vertice in range(quantidade):
        grafo.adicionar_vertice(vertice)
    for origem in range(quantidade):
        for destino in range(origem + 1, quantidade):
            if sorteio.random() < probabilidade:
                grafo.adicionar_aresta(origem, destino)
    return grafo


def para_matriz(grafo):
    """Copia um GrafoLista de vértices 0..n-1 para a representação em matriz."""
    matriz = GrafoMatriz(grafo.numero_vertices(), dirigido=grafo.dirigido)
    for origem, destino, peso in grafo.arestas():
        matriz.adicionar_aresta(origem, destino, peso)
    return matriz


def alcancaveis(grafo, origem):
    """Força bruta: conjunto de vértices alcançáveis (BFS simples e independente)."""
    vistos = {origem}
    pilha = [origem]
    while pilha:
        atual = pilha.pop()
        for vizinho, _ in grafo.vizinhos(atual):
            if vizinho not in vistos:
                vistos.add(vizinho)
                pilha.append(vizinho)
    return vistos


# ----------------------------------------------------------------------
# Representações
# ----------------------------------------------------------------------
class TestRepresentacoes(unittest.TestCase):
    def test_lista_de_adjacencia_nao_dirigido(self):
        grafo = GrafoLista()
        grafo.adicionar_aresta("A", "B", 4)
        grafo.adicionar_aresta("A", "C", 2)
        self.assertTrue(grafo.existe_aresta("B", "A"))  # vale nos dois sentidos
        self.assertEqual(grafo.peso("B", "A"), 4)
        self.assertEqual(sorted(grafo.vertices()), ["A", "B", "C"])
        self.assertEqual(grafo.numero_arestas(), 2)
        self.assertEqual(grafo.grau("A"), 2)
        grafo.remover_aresta("A", "B")
        self.assertFalse(grafo.existe_aresta("B", "A"))
        with self.assertRaises(KeyError):
            grafo.remover_aresta("A", "B")
        grafo.remover_vertice("C")
        self.assertNotIn("C", grafo)
        self.assertEqual(grafo.numero_arestas(), 0)

    def test_lista_de_adjacencia_dirigido(self):
        grafo = GrafoLista(dirigido=True)
        grafo.adicionar_aresta("A", "B")
        self.assertTrue(grafo.existe_aresta("A", "B"))
        self.assertFalse(grafo.existe_aresta("B", "A"))
        self.assertEqual(grafo.grau_saida("A"), 1)
        self.assertEqual(grafo.grau_entrada("A"), 0)
        self.assertEqual(grafo.grau_entrada("B"), 1)
        transposto = grafo.transposto()
        self.assertTrue(transposto.existe_aresta("B", "A"))
        self.assertFalse(transposto.existe_aresta("A", "B"))

    def test_matriz_de_adjacencia(self):
        grafo = GrafoMatriz(3)
        grafo.adicionar_aresta(0, 1, 5)
        self.assertEqual(grafo.peso(1, 0), 5)  # matriz simétrica
        self.assertEqual(grafo.vizinhos(0), [(1, 5)])
        self.assertEqual(grafo.numero_arestas(), 1)
        with self.assertRaises(KeyError):
            grafo.adicionar_aresta(0, 9)
        novo = grafo.adicionar_vertice()
        self.assertEqual(novo, 3)
        self.assertEqual(grafo.numero_vertices(), 4)
        self.assertEqual(grafo.peso(1, 0), 5)  # a matriz cresceu sem perder dados

    def test_matriz_nao_compartilha_linhas(self):
        """O pega-ratão do [[None]*n]*n."""
        grafo = GrafoMatriz(3, dirigido=True)
        grafo.adicionar_aresta(0, 1)
        self.assertFalse(grafo.existe_aresta(1, 1))
        self.assertFalse(grafo.existe_aresta(2, 1))

    def test_as_duas_representacoes_concordam(self):
        sorteio = random.Random(3)
        for _ in range(30):
            lista = grafo_aleatorio(sorteio, 7)
            matriz = para_matriz(lista)
            self.assertEqual(lista.numero_arestas(), matriz.numero_arestas())
            for vertice in lista.vertices():
                self.assertEqual(sorted(lista.vizinhos(vertice)), sorted(matriz.vizinhos(vertice)))
                self.assertEqual(lista.grau(vertice), matriz.grau(vertice))


# ----------------------------------------------------------------------
# Buscas
# ----------------------------------------------------------------------
class TestBuscas(unittest.TestCase):
    def montar(self):
        grafo = GrafoLista()
        for origem, destino in (("A", "B"), ("A", "C"), ("B", "D"), ("C", "D"),
                                ("D", "E"), ("E", "F"), ("C", "F")):
            grafo.adicionar_aresta(origem, destino)
        return grafo

    def test_ordem_das_buscas(self):
        grafo = self.montar()
        self.assertEqual(bfs(grafo, "A"), ["A", "B", "C", "D", "F", "E"])
        self.assertEqual(dfs(grafo, "A"), ["A", "B", "D", "C", "F", "E"])

    def test_dfs_iterativa_tem_a_mesma_ordem_da_recursiva(self):
        sorteio = random.Random(5)
        for _ in range(100):
            grafo = grafo_aleatorio(sorteio, sorteio.randint(2, 12), probabilidade=0.4)
            origem = sorteio.choice(grafo.vertices())
            self.assertEqual(dfs_iterativa(grafo, origem), dfs(grafo, origem))

    def test_bfs_e_dfs_visitam_os_mesmos_vertices(self):
        sorteio = random.Random(6)
        for _ in range(100):
            grafo = grafo_aleatorio(sorteio, sorteio.randint(2, 12), dirigido=sorteio.choice([True, False]))
            origem = sorteio.choice(grafo.vertices())
            self.assertEqual(set(bfs(grafo, origem)), alcancaveis(grafo, origem))
            self.assertEqual(set(dfs(grafo, origem)), alcancaveis(grafo, origem))

    def test_bfs_da_o_caminho_com_menos_arestas(self):
        sorteio = random.Random(7)
        for _ in range(60):
            grafo = grafo_aleatorio(sorteio, 9, probabilidade=0.3)
            origem = 0
            distancia, _ = bfs_distancias(grafo, origem)
            # compara com Floyd-Warshall usando peso 1 em todas as arestas
            sem_peso = GrafoLista()
            for vertice in grafo.vertices():
                sem_peso.adicionar_vertice(vertice)
            for u, v, _ in grafo.arestas():
                sem_peso.adicionar_aresta(u, v, 1)
            todos, _ = floyd_warshall(sem_peso)
            for destino in grafo.vertices():
                esperado = todos[origem][destino]
                if esperado == INFINITO:
                    self.assertNotIn(destino, distancia)
                    self.assertIsNone(menor_caminho_bfs(grafo, origem, destino))
                else:
                    self.assertEqual(distancia[destino], esperado)
                    caminho = menor_caminho_bfs(grafo, origem, destino)
                    self.assertEqual(len(caminho) - 1, esperado)
                    self.assertEqual(caminho[0], origem)
                    self.assertEqual(caminho[-1], destino)
                    for a, b in zip(caminho, caminho[1:]):
                        self.assertTrue(grafo.existe_aresta(a, b))

    def test_existe_caminho(self):
        grafo = self.montar()
        grafo.adicionar_vertice("Z")
        self.assertTrue(existe_caminho(grafo, "A", "F"))
        self.assertFalse(existe_caminho(grafo, "A", "Z"))

    def test_todos_os_caminhos_sao_simples_e_validos(self):
        grafo = self.montar()
        caminhos = todos_os_caminhos(grafo, "A", "F")
        self.assertEqual(len(caminhos), 4)
        for caminho in caminhos:
            self.assertEqual(len(caminho), len(set(caminho)))  # sem repetir vértice
            self.assertEqual((caminho[0], caminho[-1]), ("A", "F"))
            for a, b in zip(caminho, caminho[1:]):
                self.assertTrue(grafo.existe_aresta(a, b))

    def test_dfs_completa_visita_todo_mundo(self):
        sorteio = random.Random(8)
        grafo = grafo_aleatorio(sorteio, 10, probabilidade=0.15, dirigido=True)
        descoberta, finalizacao, _ = dfs_completa(grafo)
        self.assertEqual(set(descoberta), set(grafo.vertices()))
        for vertice in grafo.vertices():
            self.assertLess(descoberta[vertice], finalizacao[vertice])

    def test_vertice_inexistente(self):
        grafo = self.montar()
        with self.assertRaises(KeyError):
            bfs(grafo, "Z")
        with self.assertRaises(KeyError):
            dfs(grafo, "Z")

    def test_algoritmos_funcionam_com_a_matriz(self):
        sorteio = random.Random(9)
        for _ in range(30):
            lista = grafo_aleatorio(sorteio, 8)
            matriz = para_matriz(lista)
            self.assertEqual(bfs(lista, 0), bfs(matriz, 0))
            self.assertEqual(dfs(lista, 0), dfs(matriz, 0))
            self.assertEqual(dijkstra(lista, 0)[0], dijkstra(matriz, 0)[0])


# ----------------------------------------------------------------------
# Propriedades
# ----------------------------------------------------------------------
class TestPropriedades(unittest.TestCase):
    def test_componentes_conexas(self):
        sorteio = random.Random(11)
        for _ in range(60):
            grafo = grafo_aleatorio(sorteio, sorteio.randint(1, 12), probabilidade=0.15)
            componentes = componentes_conexas(grafo)
            # cada vértice aparece exatamente uma vez
            todos = [v for componente in componentes for v in componente]
            self.assertEqual(sorted(todos), sorted(grafo.vertices()))
            # dentro de uma componente todos se alcançam; entre componentes, ninguém
            for componente in componentes:
                self.assertEqual(alcancaveis(grafo, componente[0]), set(componente))
            self.assertEqual(eh_conexo(grafo), len(componentes) <= 1)

    def test_ciclo_em_grafo_nao_dirigido(self):
        sorteio = random.Random(12)
        for _ in range(100):
            grafo = grafo_aleatorio(sorteio, sorteio.randint(1, 10), probabilidade=0.2)
            # num grafo não dirigido simples: tem ciclo  <=>  arestas > vértices - componentes
            componentes = len(componentes_conexas(grafo))
            esperado = grafo.numero_arestas() > grafo.numero_vertices() - componentes
            self.assertEqual(tem_ciclo(grafo), esperado)

    def test_ciclo_em_digrafo(self):
        sorteio = random.Random(13)
        for _ in range(100):
            grafo = grafo_aleatorio(sorteio, sorteio.randint(1, 9), probabilidade=0.2, dirigido=True)
            # força bruta: existe ciclo se algum vértice se alcança por um vizinho
            esperado = any(
                vertice in alcancaveis(grafo, vizinho)
                for vertice in grafo.vertices()
                for vizinho, _ in grafo.vizinhos(vertice)
            )
            self.assertEqual(tem_ciclo(grafo), esperado)
        self.assertFalse(tem_ciclo(dag_aleatorio(sorteio, 10)))

    def test_ordenacao_topologica(self):
        sorteio = random.Random(14)
        for _ in range(60):
            dag = dag_aleatorio(sorteio, sorteio.randint(1, 12))
            for ordem in (ordenacao_topologica(dag), ordenacao_topologica_dfs(dag)):
                self.assertEqual(sorted(ordem), sorted(dag.vertices()))
                posicao = {vertice: i for i, vertice in enumerate(ordem)}
                for origem, destino, _ in dag.arestas():
                    self.assertLess(posicao[origem], posicao[destino])

    def test_ordenacao_topologica_recusa_ciclo(self):
        ciclo = GrafoLista(dirigido=True)
        for origem, destino in ((1, 2), (2, 3), (3, 1)):
            ciclo.adicionar_aresta(origem, destino)
        with self.assertRaises(ValueError):
            ordenacao_topologica(ciclo)
        with self.assertRaises(ValueError):
            ordenacao_topologica_dfs(ciclo)
        with self.assertRaises(ValueError):
            ordenacao_topologica(GrafoLista())  # não dirigido

    def test_bipartido_contra_forca_bruta(self):
        sorteio = random.Random(15)
        for _ in range(60):
            quantidade = sorteio.randint(1, 8)
            grafo = grafo_aleatorio(sorteio, quantidade, probabilidade=0.3)
            arestas = [(u, v) for u, v, _ in grafo.arestas()]
            # força bruta: testa todas as 2^n pinturas possíveis
            esperado = any(
                all(cores[u] != cores[v] for u, v in arestas)
                for cores in itertools.product((0, 1), repeat=quantidade)
            )
            resposta, cores = eh_bipartido(grafo)
            self.assertEqual(resposta, esperado)
            if resposta:
                for u, v in arestas:
                    self.assertNotEqual(cores[u], cores[v])

    def test_bipartido_em_ciclos(self):
        par = GrafoLista()
        for origem, destino in ((1, 2), (2, 3), (3, 4), (4, 1)):
            par.adicionar_aresta(origem, destino)
        self.assertTrue(eh_bipartido(par)[0])
        par.adicionar_aresta(4, 2)  # cria um triângulo (ciclo ímpar)
        self.assertFalse(eh_bipartido(par)[0])

    def test_componentes_fortemente_conexas(self):
        sorteio = random.Random(16)
        for _ in range(60):
            grafo = grafo_aleatorio(sorteio, sorteio.randint(1, 9), probabilidade=0.25, dirigido=True)
            componentes = componentes_fortemente_conexas(grafo)
            todos = [v for componente in componentes for v in componente]
            self.assertEqual(sorted(todos), sorted(grafo.vertices()))
            # força bruta: u e v estão na mesma componente <=> um alcança o outro
            alcance = {v: alcancaveis(grafo, v) for v in grafo.vertices()}
            for componente in componentes:
                grupo = set(componente)
                for u in grupo:
                    esperado = {v for v in grafo.vertices() if v in alcance[u] and u in alcance[v]}
                    self.assertEqual(grupo, esperado)

    def test_pontes_e_articulacoes_contra_forca_bruta(self):
        sorteio = random.Random(17)
        for _ in range(60):
            quantidade = sorteio.randint(2, 9)
            grafo = grafo_aleatorio(sorteio, quantidade, probabilidade=0.3)
            componentes_iniciais = len(componentes_conexas(grafo))

            # ponte: tirar a aresta aumenta o número de componentes
            esperadas = set()
            for origem, destino, peso in grafo.arestas():
                grafo.remover_aresta(origem, destino)
                if len(componentes_conexas(grafo)) > componentes_iniciais:
                    esperadas.add(frozenset((origem, destino)))
                grafo.adicionar_aresta(origem, destino, peso)
            self.assertEqual({frozenset(p) for p in pontes(grafo)}, esperadas)

            # articulação: tirar o vértice aumenta o número de componentes
            esperados = set()
            for vertice in grafo.vertices():
                arestas_do_vertice = [(vertice, v, p) for v, p in grafo.vizinhos(vertice)]
                grafo.remover_vertice(vertice)
                componentes = len(componentes_conexas(grafo)) if grafo.numero_vertices() else 0
                sozinho = 1 if not arestas_do_vertice else 0
                if componentes > componentes_iniciais - sozinho:
                    esperados.add(vertice)
                grafo.adicionar_vertice(vertice)
                for origem, destino, peso in arestas_do_vertice:
                    grafo.adicionar_aresta(origem, destino, peso)
            self.assertEqual(set(pontos_de_articulacao(grafo)), esperados)

    def test_recusa_tipo_errado_de_grafo(self):
        with self.assertRaises(ValueError):
            componentes_conexas(GrafoLista(dirigido=True))
        with self.assertRaises(ValueError):
            componentes_fortemente_conexas(GrafoLista())
        with self.assertRaises(ValueError):
            eh_bipartido(GrafoLista(dirigido=True))
        with self.assertRaises(ValueError):
            pontes(GrafoLista(dirigido=True))


# ----------------------------------------------------------------------
# Caminhos mínimos
# ----------------------------------------------------------------------
class TestCaminhosMinimos(unittest.TestCase):
    def test_exemplo_conhecido(self):
        mapa = GrafoLista()
        for origem, destino, km in (("A", "B", 4), ("A", "C", 2), ("B", "C", 1),
                                    ("B", "D", 5), ("C", "D", 8), ("C", "E", 10),
                                    ("D", "E", 2), ("D", "F", 6), ("E", "F", 3)):
            mapa.adicionar_aresta(origem, destino, km)
        distancia, _ = dijkstra(mapa, "A")
        self.assertEqual(distancia, {"A": 0, "B": 3, "C": 2, "D": 8, "E": 10, "F": 13})
        caminho, custo = caminho_minimo(mapa, "A", "F")
        self.assertEqual(caminho, ["A", "C", "B", "D", "E", "F"])
        self.assertEqual(custo, 13)

    def test_os_tres_algoritmos_concordam(self):
        sorteio = random.Random(21)
        for _ in range(60):
            dirigido = sorteio.choice([True, False])
            grafo = grafo_aleatorio(sorteio, sorteio.randint(2, 10), probabilidade=0.35, dirigido=dirigido)
            todos, proximo = floyd_warshall(grafo)
            for origem in grafo.vertices():
                por_dijkstra, pai = dijkstra(grafo, origem)
                por_bellman, _ = bellman_ford(grafo, origem)
                self.assertEqual(por_dijkstra, por_bellman)
                self.assertEqual(por_dijkstra, todos[origem])
                # o caminho reconstruído precisa existir e custar o prometido
                for destino in grafo.vertices():
                    if por_dijkstra[destino] == INFINITO:
                        continue
                    caminho = caminho_floyd(proximo, origem, destino)
                    custo = sum(grafo.peso(a, b) for a, b in zip(caminho, caminho[1:]))
                    self.assertEqual(custo, por_dijkstra[destino])

    def test_vertice_inalcancavel_fica_com_infinito(self):
        grafo = GrafoLista()
        grafo.adicionar_aresta("A", "B")
        grafo.adicionar_vertice("Z")
        distancia, _ = dijkstra(grafo, "A")
        self.assertEqual(distancia["Z"], INFINITO)
        self.assertEqual(caminho_minimo(grafo, "A", "Z"), (None, INFINITO))

    def test_dijkstra_recusa_peso_negativo(self):
        grafo = GrafoLista(dirigido=True)
        grafo.adicionar_aresta("A", "B", -1)
        with self.assertRaises(ValueError):
            dijkstra(grafo, "A")
        with self.assertRaises(KeyError):
            dijkstra(grafo, "Z")

    def test_bellman_ford_aceita_peso_negativo(self):
        grafo = GrafoLista(dirigido=True)
        for origem, destino, peso in (("S", "A", 4), ("S", "B", 5), ("A", "B", -3), ("B", "C", 2)):
            grafo.adicionar_aresta(origem, destino, peso)
        distancia, _ = bellman_ford(grafo, "S")
        self.assertEqual(distancia, {"S": 0, "A": 4, "B": 1, "C": 3})

    def test_ciclo_negativo_e_detectado(self):
        grafo = GrafoLista(dirigido=True)
        for origem, destino, peso in (("A", "B", 1), ("B", "C", -3), ("C", "A", 1)):
            grafo.adicionar_aresta(origem, destino, peso)
        with self.assertRaises(ValueError):
            bellman_ford(grafo, "A")
        with self.assertRaises(ValueError):
            floyd_warshall(grafo)


# ----------------------------------------------------------------------
# Árvore geradora mínima
# ----------------------------------------------------------------------
class TestArvoreGeradoraMinima(unittest.TestCase):
    def test_prim_e_kruskal_dao_o_mesmo_custo(self):
        sorteio = random.Random(31)
        for _ in range(60):
            quantidade = sorteio.randint(2, 12)
            grafo = grafo_conexo_aleatorio(sorteio, quantidade, extras=quantidade)
            arestas_kruskal, custo_kruskal = kruskal(grafo)
            arestas_prim, custo_prim = prim(grafo)
            self.assertEqual(custo_kruskal, custo_prim)
            for arestas in (arestas_kruskal, arestas_prim):
                self.assertEqual(len(arestas), quantidade - 1)  # é uma árvore geradora
                # a AGM precisa ligar todos os vértices sem formar ciclo
                conjuntos = UniaoBusca(grafo.vertices())
                for origem, destino, _ in arestas:
                    self.assertTrue(conjuntos.unir(origem, destino))
                self.assertEqual(conjuntos.quantidade_de_grupos(), 1)

    def test_custo_minimo_contra_forca_bruta(self):
        """Em grafos pequenos, testa TODAS as combinações de V-1 arestas."""
        sorteio = random.Random(32)
        for _ in range(40):
            quantidade = sorteio.randint(3, 6)
            grafo = grafo_conexo_aleatorio(sorteio, quantidade, extras=3)
            arestas = grafo.arestas()
            melhor = min(
                sum(peso for _, _, peso in combinacao)
                for combinacao in itertools.combinations(arestas, quantidade - 1)
                if self.eh_arvore_geradora(grafo, combinacao)
            )
            self.assertEqual(kruskal(grafo)[1], melhor)
            self.assertEqual(prim(grafo)[1], melhor)

    def eh_arvore_geradora(self, grafo, arestas):
        conjuntos = UniaoBusca(grafo.vertices())
        return all(conjuntos.unir(origem, destino) for origem, destino, _ in arestas)

    def test_grafo_desconexo(self):
        grafo = GrafoLista()
        grafo.adicionar_aresta("A", "B", 1)
        grafo.adicionar_aresta("C", "D", 2)
        arestas, custo = kruskal(grafo)  # devolve uma FLORESTA geradora mínima
        self.assertEqual(custo, 3)
        self.assertEqual(len(arestas), 2)
        arestas_prim, custo_prim = prim(grafo, "A")  # só a componente do início
        self.assertEqual(custo_prim, 1)
        self.assertEqual(len(arestas_prim), 1)

    def test_recusa_digrafo(self):
        with self.assertRaises(ValueError):
            kruskal(GrafoLista(dirigido=True))
        with self.assertRaises(ValueError):
            prim(GrafoLista(dirigido=True))

    def test_union_find(self):
        conjuntos = UniaoBusca(range(6))
        self.assertEqual(conjuntos.quantidade_de_grupos(), 6)
        self.assertTrue(conjuntos.unir(0, 1))
        self.assertFalse(conjuntos.unir(1, 0))  # já estavam juntos
        conjuntos.unir(2, 3)
        conjuntos.unir(1, 3)
        self.assertTrue(conjuntos.mesmo_conjunto(0, 2))
        self.assertFalse(conjuntos.mesmo_conjunto(0, 4))
        self.assertEqual(conjuntos.quantidade_de_grupos(), 3)
        with self.assertRaises(KeyError):
            conjuntos.encontrar(99)

    def test_union_find_contra_modelo_simples(self):
        sorteio = random.Random(33)
        conjuntos = UniaoBusca(range(20))
        grupos = {i: {i} for i in range(20)}  # modelo ingênuo, com conjuntos de verdade
        for _ in range(500):
            a, b = sorteio.randrange(20), sorteio.randrange(20)
            juntou = conjuntos.unir(a, b)
            self.assertEqual(juntou, grupos[a] is not grupos[b])
            if juntou:
                novo = grupos[a] | grupos[b]
                for elemento in novo:
                    grupos[elemento] = novo
            for x in range(20):
                for y in range(20):
                    self.assertEqual(conjuntos.mesmo_conjunto(x, y), y in grupos[x])


if __name__ == "__main__":
    unittest.main()
