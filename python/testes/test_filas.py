"""Testes das filas (FIFO), do deque, da fila de prioridade e da fila com duas pilhas.

Rode a partir da pasta `python/`:  python3 -m unittest
"""

import random
import unittest
from collections import deque as deque_do_python

from filas.deque import Deque
from filas.fila_duas_pilhas import FilaComDuasPilhas
from filas.fila_encadeada import FilaEncadeada
from filas.fila_prioridade import FilaPrioridade
from filas.fila_sequencial import FilaSequencial


class ContratoFila:
    """Testes aplicados a todas as filas FIFO."""

    def criar(self):
        raise NotImplementedError

    def esta_cheia(self, fila):
        return hasattr(fila, "cheia") and fila.cheia()

    def test_comeca_vazia(self):
        fila = self.criar()
        self.assertTrue(fila.vazia())
        self.assertEqual(len(fila), 0)

    def test_fila_vazia_levanta_erro(self):
        fila = self.criar()
        with self.assertRaises(IndexError):
            fila.desenfileirar()
        with self.assertRaises(IndexError):
            fila.frente()

    def test_ordem_fifo(self):
        fila = self.criar()
        for valor in (10, 20, 30):
            fila.enfileirar(valor)
        self.assertEqual(fila.frente(), 10)
        self.assertEqual(len(fila), 3)  # frente() não remove
        self.assertEqual([fila.desenfileirar() for _ in range(3)], [10, 20, 30])
        self.assertTrue(fila.vazia())

    def test_operacoes_aleatorias(self):
        sorteio = random.Random(13)
        fila = self.criar()
        modelo = deque_do_python()
        for _ in range(3000):
            operacao = sorteio.choice(["enfileirar", "enfileirar", "desenfileirar", "frente"])
            if operacao == "enfileirar":
                if self.esta_cheia(fila):
                    with self.assertRaises(OverflowError):
                        fila.enfileirar(0)
                    continue
                valor = sorteio.randrange(100)
                fila.enfileirar(valor)
                modelo.append(valor)
            elif operacao == "desenfileirar" and modelo:
                self.assertEqual(fila.desenfileirar(), modelo.popleft())
            elif operacao == "frente" and modelo:
                self.assertEqual(fila.frente(), modelo[0])
            self.assertEqual(len(fila), len(modelo))
            self.assertEqual(fila.vazia(), not modelo)


class TestFilaSequencial(ContratoFila, unittest.TestCase):
    def criar(self):
        # capacidade pequena de propósito: força o vetor a dar a volta várias vezes
        return FilaSequencial(capacidade=7)

    def test_vetor_circular_da_a_volta(self):
        fila = FilaSequencial(capacidade=3)
        fila.enfileirar(1)
        fila.enfileirar(2)
        self.assertEqual(fila.desenfileirar(), 1)
        fila.enfileirar(3)
        fila.enfileirar(4)  # essa entra na posição 0 do vetor (deu a volta)
        self.assertTrue(fila.cheia())
        self.assertEqual(list(fila), [2, 3, 4])
        self.assertEqual(fila.frente(), 2)
        self.assertEqual(fila.ultimo(), 4)
        with self.assertRaises(OverflowError):
            fila.enfileirar(5)

    def test_percurso_na_ordem_de_saida(self):
        fila = self.criar()
        for valor in range(5):
            fila.enfileirar(valor)
        fila.desenfileirar()
        self.assertEqual(list(fila), [1, 2, 3, 4])


class TestFilaEncadeada(ContratoFila, unittest.TestCase):
    def criar(self):
        return FilaEncadeada()

    def test_esvaziar_zera_o_fim(self):
        fila = FilaEncadeada()
        fila.enfileirar(1)
        fila.desenfileirar()
        self.assertIsNone(fila.inicio)
        self.assertIsNone(fila.fim)  # o pega-ratão clássico
        fila.enfileirar(2)           # e a fila continua funcionando depois
        self.assertEqual(fila.frente(), 2)
        self.assertEqual(fila.ultimo(), 2)


class TestFilaComDuasPilhas(ContratoFila, unittest.TestCase):
    def criar(self):
        return FilaComDuasPilhas()

    def test_intercalando_entradas_e_saidas(self):
        fila = FilaComDuasPilhas()
        for valor in (1, 2, 3):
            fila.enfileirar(valor)
        self.assertEqual(fila.desenfileirar(), 1)
        fila.enfileirar(4)  # entra enquanto a pilha de saída ainda tem gente
        self.assertEqual([fila.desenfileirar() for _ in range(3)], [2, 3, 4])


class TestDeque(unittest.TestCase):
    def test_deque_vazio(self):
        deque = Deque()
        self.assertTrue(deque.vazio())
        for operacao in (deque.remover_inicio, deque.remover_fim, deque.primeiro, deque.ultimo):
            with self.assertRaises(IndexError):
                operacao()

    def test_usado_como_pilha_e_como_fila(self):
        pilha = Deque()
        for valor in (1, 2, 3):
            pilha.inserir_fim(valor)
        self.assertEqual([pilha.remover_fim() for _ in range(3)], [3, 2, 1])  # LIFO

        fila = Deque()
        for valor in (1, 2, 3):
            fila.inserir_fim(valor)
        self.assertEqual([fila.remover_inicio() for _ in range(3)], [1, 2, 3])  # FIFO

    def test_operacoes_aleatorias(self):
        sorteio = random.Random(23)
        deque = Deque()
        modelo = deque_do_python()
        for _ in range(3000):
            operacao = sorteio.choice([
                "inserir_inicio", "inserir_fim", "remover_inicio", "remover_fim",
                "primeiro", "ultimo",
            ])
            valor = sorteio.randrange(100)
            if operacao == "inserir_inicio":
                deque.inserir_inicio(valor)
                modelo.appendleft(valor)
            elif operacao == "inserir_fim":
                deque.inserir_fim(valor)
                modelo.append(valor)
            elif modelo:
                if operacao == "remover_inicio":
                    self.assertEqual(deque.remover_inicio(), modelo.popleft())
                elif operacao == "remover_fim":
                    self.assertEqual(deque.remover_fim(), modelo.pop())
                elif operacao == "primeiro":
                    self.assertEqual(deque.primeiro(), modelo[0])
                else:
                    self.assertEqual(deque.ultimo(), modelo[-1])
            self.assertEqual(list(deque), list(modelo))
            self.assertEqual(len(deque), len(modelo))


class TestFilaPrioridade(unittest.TestCase):
    def test_sai_pela_prioridade(self):
        fila = FilaPrioridade()
        fila.enfileirar("corte leve", 4)
        fila.enfileirar("parada cardíaca", 1)
        fila.enfileirar("fratura", 2)
        self.assertEqual(fila.frente(), ("parada cardíaca", 1))
        self.assertEqual([fila.desenfileirar() for _ in range(3)],
                         ["parada cardíaca", "fratura", "corte leve"])

    def test_empate_respeita_a_ordem_de_chegada(self):
        fila = FilaPrioridade()
        for nome in ("primeiro", "segundo", "terceiro"):
            fila.enfileirar(nome, 5)
        self.assertEqual([fila.desenfileirar() for _ in range(3)],
                         ["primeiro", "segundo", "terceiro"])

    def test_fila_vazia(self):
        fila = FilaPrioridade()
        self.assertTrue(fila.vazia())
        with self.assertRaises(IndexError):
            fila.desenfileirar()
        with self.assertRaises(IndexError):
            fila.frente()

    def test_muitas_operacoes_aleatorias(self):
        sorteio = random.Random(31)
        fila = FilaPrioridade()
        modelo = []  # lista de (prioridade, chegada, valor)
        chegada = 0
        for _ in range(2000):
            if sorteio.random() < 0.6:
                prioridade = sorteio.randrange(10)
                fila.enfileirar(f"item{chegada}", prioridade)
                modelo.append((prioridade, chegada, f"item{chegada}"))
                chegada += 1
            elif modelo:
                modelo.sort()
                esperado = modelo.pop(0)
                self.assertEqual(fila.desenfileirar(), esperado[2])
            self.assertEqual(len(fila), len(modelo))


if __name__ == "__main__":
    unittest.main()
