"""Testes das árvores (binária, ABB, AVL, rubro-negra) e do heap.

Estratégia: além dos casos conhecidos, fazemos muitas inserções e remoções
aleatórias e conferimos a cada passo que
  - o percurso EM ORDEM continua igual ao `sorted()` do Python; e
  - as INVARIANTES da estrutura continuam valendo (ABB válida, |FB| <= 1 na
    AVL, as 5 regras na rubro-negra, propriedade do heap).

Rode a partir da pasta `python/`:  python3 -m unittest
"""

import math
import random
import unittest

from arvores.arvore_avl import ArvoreAVL
from arvores.arvore_binaria import ArvoreBinaria, No
from arvores.arvore_binaria_busca import ArvoreBinariaBusca
from arvores.arvore_rubro_negra import ArvoreRubroNegra
from arvores.heap import HeapMaximo, HeapMinimo, heapsort


def arvore_aleatoria(sorteio, quantidade):
    """Monta uma árvore binária genérica com formato aleatório e valores 0..n-1."""
    if quantidade == 0:
        return ArvoreBinaria()
    valores = list(range(quantidade))
    sorteio.shuffle(valores)
    raiz = No(valores[0])
    nos = [raiz]
    for valor in valores[1:]:
        while True:
            pai = sorteio.choice(nos)
            lado = sorteio.choice(("esquerda", "direita"))
            if getattr(pai, lado) is None:
                novo = No(valor)
                setattr(pai, lado, novo)
                nos.append(novo)
                break
    return ArvoreBinaria(raiz)


class TestArvoreBinaria(unittest.TestCase):
    def montar(self):
        return ArvoreBinaria.de_lista_por_nivel([1, 2, 3, 4, 5, None, 7])

    def test_percursos(self):
        arvore = self.montar()
        self.assertEqual(arvore.pre_ordem(), [1, 2, 4, 5, 3, 7])
        self.assertEqual(arvore.em_ordem(), [4, 2, 5, 1, 3, 7])
        self.assertEqual(arvore.pos_ordem(), [4, 5, 2, 7, 3, 1])
        self.assertEqual(arvore.em_nivel(), [1, 2, 3, 4, 5, 7])
        self.assertEqual(arvore.por_niveis(), [[1], [2, 3], [4, 5, 7]])

    def test_medidas(self):
        arvore = self.montar()
        self.assertEqual(arvore.tamanho(), 6)
        self.assertEqual(arvore.altura(), 2)
        self.assertEqual(arvore.contar_folhas(), 3)
        self.assertEqual(arvore.contar_nos_internos(), 3)
        self.assertEqual(arvore.nivel(1), 0)
        self.assertEqual(arvore.nivel(7), 2)
        self.assertEqual(arvore.nivel(99), -1)
        self.assertTrue(arvore.contem(5))
        self.assertFalse(arvore.contem(6))

    def test_arvore_vazia(self):
        vazia = ArvoreBinaria()
        self.assertEqual(vazia.altura(), -1)
        self.assertEqual(vazia.tamanho(), 0)
        self.assertEqual(vazia.pre_ordem(), [])
        self.assertEqual(vazia.em_nivel(), [])
        self.assertTrue(vazia.eh_completa())
        self.assertFalse(vazia.remover(1))
        self.assertEqual(str(vazia), "(árvore vazia)")

    def test_classificacoes(self):
        cheia = ArvoreBinaria.de_lista_por_nivel([1, 2, 3, 4, 5, 6, 7])
        self.assertTrue(cheia.eh_cheia())
        self.assertTrue(cheia.eh_completa())
        self.assertTrue(cheia.eh_estritamente_binaria())

        completa = ArvoreBinaria.de_lista_por_nivel([1, 2, 3, 4])
        self.assertTrue(completa.eh_completa())
        self.assertFalse(completa.eh_cheia())
        self.assertFalse(completa.eh_estritamente_binaria())

        # o 3 tem só o filho esquerdo, e o nível de cima ficou com buracos
        furada = ArvoreBinaria.de_lista_por_nivel([1, 2, 3, None, None, 6])
        self.assertFalse(furada.eh_completa())
        self.assertFalse(furada.eh_estritamente_binaria())
        self.assertFalse(furada.eh_cheia())

    def test_espelhar_inverte_o_em_ordem(self):
        sorteio = random.Random(2)
        for _ in range(100):
            arvore = arvore_aleatoria(sorteio, sorteio.randrange(12))
            original = arvore.em_ordem()
            arvore.espelhar()
            self.assertEqual(arvore.em_ordem(), original[::-1])
            arvore.espelhar()
            self.assertEqual(arvore.em_ordem(), original)

    def test_percursos_iterativos_batem_com_os_recursivos(self):
        sorteio = random.Random(4)
        for _ in range(200):
            arvore = arvore_aleatoria(sorteio, sorteio.randrange(15))
            self.assertEqual(arvore.pre_ordem_iterativa(), arvore.pre_ordem())
            self.assertEqual(arvore.em_ordem_iterativa(), arvore.em_ordem())
            self.assertEqual(arvore.pos_ordem_iterativa(), arvore.pos_ordem())

    def test_reconstruir_a_partir_dos_percursos(self):
        sorteio = random.Random(6)
        for _ in range(200):
            arvore = arvore_aleatoria(sorteio, sorteio.randrange(1, 15))
            copia = ArvoreBinaria.de_percursos(arvore.pre_ordem(), arvore.em_ordem())
            self.assertTrue(copia.igual_a(arvore))
        with self.assertRaises(ValueError):
            ArvoreBinaria.de_percursos([1, 2], [1, 3])

    def test_inserir_por_nivel_mantem_completa(self):
        arvore = ArvoreBinaria()
        for valor in range(1, 20):
            arvore.inserir_por_nivel(valor)
            self.assertTrue(arvore.eh_completa())
        self.assertEqual(arvore.em_nivel(), list(range(1, 20)))
        self.assertEqual(arvore.altura(), math.floor(math.log2(19)))

    def test_remover(self):
        arvore = ArvoreBinaria.de_lista_por_nivel([1, 2, 3, 4, 5])
        self.assertTrue(arvore.remover(2))
        self.assertEqual(sorted(arvore.em_ordem()), [1, 3, 4, 5])
        self.assertEqual(arvore.tamanho(), 4)
        self.assertFalse(arvore.remover(99))
        for valor in list(arvore.em_ordem()):
            self.assertTrue(arvore.remover(valor))
        self.assertEqual(arvore.tamanho(), 0)


class ContratoArvoreDeBusca:
    """Testes comuns à ABB, à AVL e à rubro-negra."""

    def criar(self, valores=()):
        raise NotImplementedError

    def conferir_invariantes(self, arvore):
        self.assertTrue(arvore.eh_valida())

    def test_em_ordem_sai_ordenado(self):
        arvore = self.criar([50, 30, 70, 20, 40, 60, 80])
        self.assertEqual(arvore.em_ordem(), [20, 30, 40, 50, 60, 70, 80])
        self.assertEqual(len(arvore), 7)
        self.conferir_invariantes(arvore)

    def test_nao_aceita_repetidos(self):
        arvore = self.criar([10, 20])
        self.assertFalse(arvore.inserir(10))
        self.assertEqual(len(arvore), 2)

    def test_busca(self):
        arvore = self.criar([50, 30, 70])
        self.assertTrue(arvore.contem(30))
        self.assertFalse(arvore.contem(31))
        self.assertIn(70, arvore)
        self.assertNotIn(71, arvore)

    def test_minimo_e_maximo(self):
        arvore = self.criar([50, 30, 70, 20, 80])
        self.assertEqual(arvore.minimo(), 20)
        self.assertEqual(arvore.maximo(), 80)
        vazia = self.criar()
        with self.assertRaises(ValueError):
            vazia.minimo()
        with self.assertRaises(ValueError):
            vazia.maximo()

    def test_remover_valor_inexistente(self):
        arvore = self.criar([10, 20, 30])
        self.assertFalse(arvore.remover(99))
        self.assertEqual(len(arvore), 3)

    def test_os_tres_casos_de_remocao(self):
        arvore = self.criar([50, 30, 70, 20, 40, 60, 80, 35, 45])
        self.assertTrue(arvore.remover(20))   # folha
        self.assertTrue(arvore.remover(60))   # (na ABB) folha; nas balanceadas, qualquer caso
        self.assertTrue(arvore.remover(40))   # dois filhos
        self.assertTrue(arvore.remover(50))   # a raiz
        self.assertEqual(arvore.em_ordem(), [30, 35, 45, 70, 80])
        self.conferir_invariantes(arvore)

    def test_insercoes_e_remocoes_aleatorias(self):
        sorteio = random.Random(101)
        arvore = self.criar()
        modelo = set()
        for _ in range(1200):
            valor = sorteio.randrange(60)
            if sorteio.random() < 0.55:
                self.assertEqual(arvore.inserir(valor), valor not in modelo)
                modelo.add(valor)
            else:
                self.assertEqual(arvore.remover(valor), valor in modelo)
                modelo.discard(valor)
            self.assertEqual(arvore.em_ordem(), sorted(modelo))
            self.assertEqual(len(arvore), len(modelo))
            self.conferir_invariantes(arvore)

    def test_percursos_coerentes(self):
        sorteio = random.Random(77)
        valores = sorteio.sample(range(100), 30)
        arvore = self.criar(valores)
        self.assertEqual(sorted(arvore.pre_ordem()), sorted(valores))
        self.assertEqual(sorted(arvore.pos_ordem()), sorted(valores))
        self.assertEqual(sorted(arvore.em_nivel()), sorted(valores))
        self.assertEqual(arvore.pre_ordem()[0], arvore.em_nivel()[0])   # os dois começam na raiz
        self.assertEqual(arvore.pos_ordem()[-1], arvore.em_nivel()[0])  # e a pós-ordem termina nela


class TestArvoreBinariaBusca(ContratoArvoreDeBusca, unittest.TestCase):
    def criar(self, valores=()):
        return ArvoreBinariaBusca(valores)

    def test_insercao_recursiva_gera_a_mesma_arvore(self):
        sorteio = random.Random(15)
        valores = sorteio.sample(range(200), 40)
        iterativa = ArvoreBinariaBusca(valores)
        recursiva = ArvoreBinariaBusca()
        for valor in valores:
            recursiva.inserir_recursivo(valor)
        self.assertEqual(iterativa.pre_ordem(), recursiva.pre_ordem())

    def test_sucessor_e_antecessor(self):
        valores = [50, 30, 70, 20, 40, 60, 80]
        arvore = ArvoreBinariaBusca(valores)
        ordenados = sorted(valores)
        for alvo in range(0, 100, 7):
            esperado_sucessor = next((v for v in ordenados if v > alvo), None)
            esperado_antecessor = next((v for v in reversed(ordenados) if v < alvo), None)
            self.assertEqual(arvore.sucessor(alvo), esperado_sucessor)
            self.assertEqual(arvore.antecessor(alvo), esperado_antecessor)

    def test_k_esimo_menor_e_intervalo(self):
        sorteio = random.Random(19)
        valores = sorteio.sample(range(100), 25)
        arvore = ArvoreBinariaBusca(valores)
        ordenados = sorted(valores)
        for k in range(1, len(ordenados) + 1):
            self.assertEqual(arvore.k_esimo_menor(k), ordenados[k - 1])
        for _ in range(50):
            a, b = sorted(sorteio.sample(range(100), 2))
            self.assertEqual(arvore.intervalo(a, b), [v for v in ordenados if a <= v <= b])
        with self.assertRaises(IndexError):
            arvore.k_esimo_menor(0)

    def test_ancestral_comum(self):
        arvore = ArvoreBinariaBusca([50, 30, 70, 20, 40, 60, 80, 35, 45])
        self.assertEqual(arvore.ancestral_comum(35, 45), 40)
        self.assertEqual(arvore.ancestral_comum(20, 45), 30)
        self.assertEqual(arvore.ancestral_comum(20, 80), 50)
        self.assertEqual(arvore.ancestral_comum(30, 45), 30)  # um deles é o ancestral
        with self.assertRaises(ValueError):
            arvore.ancestral_comum(20, 99)

    def test_nivel_e_folhas(self):
        arvore = ArvoreBinariaBusca([50, 30, 70, 20])
        self.assertEqual(arvore.nivel(50), 0)
        self.assertEqual(arvore.nivel(20), 2)
        self.assertEqual(arvore.nivel(99), -1)
        self.assertEqual(arvore.contar_folhas(), 2)

    def test_pior_caso_vira_lista(self):
        arvore = ArvoreBinariaBusca(range(30))
        self.assertEqual(arvore.altura(), 29)


class TestArvoreAVL(ContratoArvoreDeBusca, unittest.TestCase):
    def criar(self, valores=()):
        return ArvoreAVL(valores)

    def test_rotacoes_dos_quatro_casos(self):
        casos = {
            "LL": ([30, 20, 10], [20, 10, 30]),
            "RR": ([10, 20, 30], [20, 10, 30]),
            "LR": ([30, 10, 20], [20, 10, 30]),
            "RL": ([10, 30, 20], [20, 10, 30]),
        }
        for caso, (entrada, pre_ordem_esperada) in casos.items():
            avl = ArvoreAVL(entrada)
            self.assertEqual(avl.pre_ordem(), pre_ordem_esperada, caso)
            self.assertEqual(avl.altura(), 1, caso)
            self.assertTrue(avl.eh_valida(), caso)

    def test_altura_fica_logaritmica(self):
        for quantidade in (10, 100, 1000, 5000):
            avl = ArvoreAVL(range(quantidade))  # pior caso da ABB: tudo em ordem
            limite = 1.4405 * math.log2(quantidade + 2) - 0.3277  # limite teórico da AVL
            self.assertLessEqual(avl.altura(), limite)
            self.assertTrue(avl.eh_valida())

    def test_fator_de_balanceamento(self):
        avl = ArvoreAVL([20, 10, 30, 5])
        self.assertEqual(avl.fator_balanceamento(20), 1)
        self.assertEqual(avl.fator_balanceamento(30), 0)
        with self.assertRaises(ValueError):
            avl.fator_balanceamento(99)


class TestArvoreRubroNegra(ContratoArvoreDeBusca, unittest.TestCase):
    def criar(self, valores=()):
        return ArvoreRubroNegra(valores)

    def test_raiz_preta_e_altura_limitada(self):
        for quantidade in (10, 100, 1000, 5000):
            arvore = ArvoreRubroNegra(range(quantidade))
            self.assertEqual(arvore.cor_de(arvore.em_nivel()[0]), "P")
            self.assertLessEqual(arvore.altura(), 2 * math.log2(quantidade + 1))
            self.assertTrue(arvore.eh_valida())

    def test_continua_valida_em_ordem_aleatoria(self):
        sorteio = random.Random(55)
        for _ in range(20):
            valores = sorteio.sample(range(500), 100)
            arvore = ArvoreRubroNegra(valores)
            self.assertTrue(arvore.eh_valida())
            for valor in sorteio.sample(valores, 50):
                arvore.remover(valor)
                self.assertTrue(arvore.eh_valida())
            self.assertEqual(len(arvore), 50)


class TestHeap(unittest.TestCase):
    def test_heap_minimo_devolve_em_ordem_crescente(self):
        sorteio = random.Random(88)
        for _ in range(100):
            valores = [sorteio.randrange(100) for _ in range(sorteio.randrange(30))]
            heap = HeapMinimo()
            for valor in valores:
                heap.inserir(valor)
                self.assertTrue(heap.eh_valido())
            self.assertEqual([heap.remover() for _ in range(len(valores))], sorted(valores))

    def test_heap_maximo_devolve_em_ordem_decrescente(self):
        sorteio = random.Random(89)
        valores = [sorteio.randrange(100) for _ in range(40)]
        heap = HeapMaximo(valores)  # construção com heapify
        self.assertTrue(heap.eh_valido())
        self.assertEqual([heap.remover() for _ in range(len(valores))], sorted(valores, reverse=True))

    def test_construir_com_heapify(self):
        sorteio = random.Random(90)
        for _ in range(100):
            valores = [sorteio.randrange(50) for _ in range(sorteio.randrange(25))]
            heap = HeapMinimo(valores)
            self.assertTrue(heap.eh_valido())
            self.assertEqual(sorted(heap), sorted(valores))
            if valores:
                self.assertEqual(heap.topo(), min(valores))

    def test_heap_vazio(self):
        heap = HeapMinimo()
        self.assertTrue(heap.vazio())
        with self.assertRaises(IndexError):
            heap.remover()
        with self.assertRaises(IndexError):
            heap.topo()

    def test_heapsort(self):
        sorteio = random.Random(91)
        for _ in range(200):
            valores = [sorteio.randrange(100) for _ in range(sorteio.randrange(40))]
            copia = list(valores)
            self.assertEqual(heapsort(copia), sorted(valores))
            self.assertEqual(copia, sorted(valores))  # ordena no próprio vetor


if __name__ == "__main__":
    unittest.main()
