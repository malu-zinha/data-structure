"""Testes do heap mínimo, do heap máximo e do heapsort.

Rode a partir da pasta `python/`:  python3 -m unittest
"""

import random
import unittest

from heap import HeapMaximo, HeapMinimo, heapsort


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
