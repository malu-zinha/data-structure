"""Testes das listas.

A ideia central: fazer milhares de operações ALEATÓRIAS na nossa lista e nas
`list` do Python ao mesmo tempo, conferindo a cada passo que as duas continuam
iguais. Se houver qualquer erro de ponteiro, o teste acusa.

Rode a partir da pasta `python/`:  python3 -m unittest
"""

import random
import unittest

from lista_circular import ListaCircular, problema_de_josephus
from lista_circular_dupla import ListaCircularDupla
from lista_duplamente_encadeada import ListaDuplamenteEncadeada
from lista_encadeada import ListaEncadeada
from lista_heterogenea import ListaHeterogenea, Tipo
from lista_ordenada import ListaOrdenada
from lista_sequencial import ListaSequencial


class ContratoLista:
    """Bateria de testes aplicada a TODAS as listas com a mesma interface."""

    def criar(self):
        raise NotImplementedError

    def esta_cheia(self, lista):
        return hasattr(lista, "cheia") and lista.cheia()

    # ------------------------------------------------------------------
    def test_comeca_vazia(self):
        lista = self.criar()
        self.assertTrue(lista.vazia())
        self.assertEqual(len(lista), 0)
        self.assertEqual(lista.tamanho(), 0)
        self.assertEqual(list(lista), [])

    def test_remover_de_lista_vazia_levanta_erro(self):
        lista = self.criar()
        for remocao in (lista.remover_inicio, lista.remover_fim):
            with self.assertRaises(IndexError):
                remocao()
        with self.assertRaises(IndexError):
            lista.remover_posicao(0)

    def test_posicoes_invalidas_levantam_erro(self):
        lista = self.criar()
        lista.inserir_fim(10)
        for posicao in (-1, 1, 99):
            with self.assertRaises(IndexError):
                lista.obter(posicao)
            with self.assertRaises(IndexError):
                lista.remover_posicao(posicao)
        with self.assertRaises(IndexError):
            lista.inserir_posicao(5, 99)

    def test_insercoes_e_buscas_basicas(self):
        lista = self.criar()
        lista.inserir_fim(20)
        lista.inserir_inicio(10)
        lista.inserir_posicao(2, 30)
        lista.inserir_posicao(1, 15)
        self.assertEqual(list(lista), [10, 15, 20, 30])
        self.assertEqual(lista.buscar(20), 2)
        self.assertEqual(lista.buscar(99), -1)
        self.assertIn(30, lista)
        self.assertNotIn(99, lista)
        self.assertEqual(lista.obter(0), 10)
        lista.alterar(0, 11)
        self.assertEqual(lista.obter(0), 11)

    def test_inverter_e_limpar(self):
        lista = self.criar()
        for valor in range(6):
            lista.inserir_fim(valor)
        lista.inverter()
        self.assertEqual(list(lista), [5, 4, 3, 2, 1, 0])
        lista.inverter()
        self.assertEqual(list(lista), [0, 1, 2, 3, 4, 5])
        lista.limpar()
        self.assertTrue(lista.vazia())
        self.assertEqual(list(lista), [])

    def test_operacoes_aleatorias_contra_a_list_do_python(self):
        sorteio = random.Random(17)
        lista = self.criar()
        modelo = []

        for _ in range(3000):
            operacao = sorteio.choice([
                "inserir_inicio", "inserir_inicio", "inserir_fim", "inserir_fim",
                "inserir_posicao", "inserir_posicao",
                "remover_inicio", "remover_fim", "remover_posicao", "remover_valor",
                "buscar", "obter", "alterar", "inverter",
            ])
            valor = sorteio.randrange(20)

            if operacao.startswith("inserir"):
                if self.esta_cheia(lista):
                    with self.assertRaises(OverflowError):
                        lista.inserir_fim(valor)
                    continue
                if operacao == "inserir_inicio":
                    lista.inserir_inicio(valor)
                    modelo.insert(0, valor)
                elif operacao == "inserir_fim":
                    lista.inserir_fim(valor)
                    modelo.append(valor)
                else:
                    posicao = sorteio.randint(0, len(modelo))
                    lista.inserir_posicao(posicao, valor)
                    modelo.insert(posicao, valor)

            elif operacao == "remover_valor":
                esperado = valor in modelo
                self.assertEqual(lista.remover_valor(valor), esperado)
                if esperado:
                    modelo.remove(valor)

            elif operacao == "buscar":
                esperado = modelo.index(valor) if valor in modelo else -1
                self.assertEqual(lista.buscar(valor), esperado)

            elif operacao == "inverter":
                lista.inverter()
                modelo.reverse()

            elif modelo:  # operações que precisam de lista não vazia
                if operacao == "remover_inicio":
                    self.assertEqual(lista.remover_inicio(), modelo.pop(0))
                elif operacao == "remover_fim":
                    self.assertEqual(lista.remover_fim(), modelo.pop())
                elif operacao == "remover_posicao":
                    posicao = sorteio.randrange(len(modelo))
                    self.assertEqual(lista.remover_posicao(posicao), modelo.pop(posicao))
                elif operacao == "obter":
                    posicao = sorteio.randrange(len(modelo))
                    self.assertEqual(lista.obter(posicao), modelo[posicao])
                elif operacao == "alterar":
                    posicao = sorteio.randrange(len(modelo))
                    lista.alterar(posicao, valor)
                    modelo[posicao] = valor

            self.assertEqual(list(lista), modelo)
            self.assertEqual(len(lista), len(modelo))
            self.assertEqual(lista.vazia(), not modelo)


class TestListaSequencial(ContratoLista, unittest.TestCase):
    def criar(self):
        return ListaSequencial(capacidade=40)

    def test_capacidade_cheia(self):
        lista = ListaSequencial(capacidade=3)
        for valor in (1, 2, 3):
            lista.inserir_fim(valor)
        self.assertTrue(lista.cheia())
        with self.assertRaises(OverflowError):
            lista.inserir_fim(4)
        with self.assertRaises(ValueError):
            ListaSequencial(capacidade=0)


class TestListaEncadeada(ContratoLista, unittest.TestCase):
    def criar(self):
        return ListaEncadeada()

    def test_ligacoes_dos_nos(self):
        lista = ListaEncadeada()
        for valor in (1, 2, 3):
            lista.inserir_fim(valor)
        self.assertEqual(lista.inicio.valor, 1)
        self.assertEqual(lista.inicio.proximo.valor, 2)
        self.assertIsNone(lista.inicio.proximo.proximo.proximo)


class TestListaDuplamenteEncadeada(ContratoLista, unittest.TestCase):
    def criar(self):
        return ListaDuplamenteEncadeada()

    def test_percurso_reverso(self):
        lista = self.criar()
        for valor in range(5):
            lista.inserir_fim(valor)
        self.assertEqual(list(reversed(lista)), [4, 3, 2, 1, 0])

    def test_ponteiros_continuam_coerentes(self):
        sorteio = random.Random(3)
        lista = self.criar()
        for _ in range(300):
            if sorteio.random() < 0.6 or lista.vazia():
                lista.inserir_posicao(sorteio.randint(0, len(lista)), sorteio.randrange(50))
            else:
                lista.remover_posicao(sorteio.randrange(len(lista)))
            # ida e volta têm de bater
            self.assertEqual(list(reversed(lista)), list(lista)[::-1])
            if not lista.vazia():
                self.assertIsNone(lista.inicio.anterior)
                self.assertIsNone(lista.fim.proximo)


class TestListaCircular(ContratoLista, unittest.TestCase):
    def criar(self):
        return ListaCircular()

    def test_o_ultimo_aponta_para_o_primeiro(self):
        lista = self.criar()
        for valor in (1, 2, 3):
            lista.inserir_fim(valor)
        self.assertIs(lista.fim.proximo, lista.inicio)
        self.assertEqual(lista.fim.valor, 3)
        lista.limpar()
        lista.inserir_fim(9)
        self.assertIs(lista.fim.proximo, lista.fim)  # um nó só aponta para si

    def test_rotacionar(self):
        lista = self.criar()
        for valor in (1, 2, 3, 4):
            lista.inserir_fim(valor)
        lista.rotacionar(1)
        self.assertEqual(list(lista), [2, 3, 4, 1])
        lista.rotacionar(3)
        self.assertEqual(list(lista), [1, 2, 3, 4])
        lista.rotacionar(6)  # 6 % 4 = 2
        self.assertEqual(list(lista), [3, 4, 1, 2])

    def test_josephus(self):
        eliminados, sobrevivente = problema_de_josephus(7, 3)
        self.assertEqual(eliminados, [3, 6, 2, 7, 5, 1])
        self.assertEqual(sobrevivente, 4)
        self.assertEqual(problema_de_josephus(1, 5), ([], 1))
        self.assertEqual(problema_de_josephus(5, 1), ([1, 2, 3, 4], 5))


class TestListaCircularDupla(ContratoLista, unittest.TestCase):
    def criar(self):
        return ListaCircularDupla()

    def test_as_pontas_se_ligam(self):
        lista = self.criar()
        for valor in (1, 2, 3):
            lista.inserir_fim(valor)
        self.assertIs(lista.inicio.anterior, lista.fim)
        self.assertIs(lista.fim.proximo, lista.inicio)
        self.assertEqual(list(reversed(lista)), [3, 2, 1])

    def test_rotacionar_nos_dois_sentidos(self):
        lista = self.criar()
        for valor in (1, 2, 3, 4):
            lista.inserir_fim(valor)
        lista.rotacionar(1)
        self.assertEqual(list(lista), [2, 3, 4, 1])
        lista.rotacionar(-1)
        self.assertEqual(list(lista), [1, 2, 3, 4])


class TestListaOrdenada(unittest.TestCase):
    def test_mantem_ordenada_e_acompanha_o_modelo(self):
        sorteio = random.Random(5)
        lista = ListaOrdenada()
        modelo = []
        for _ in range(1500):
            valor = sorteio.randrange(30)
            if sorteio.random() < 0.6:
                lista.inserir(valor)
                modelo.append(valor)
                modelo.sort()
            else:
                esperado = valor in modelo
                self.assertEqual(lista.remover(valor), esperado)
                if esperado:
                    modelo.remove(valor)
            self.assertEqual(list(lista), modelo)
            self.assertEqual(len(lista), len(modelo))

    def test_busca_minimo_e_maximo(self):
        lista = ListaOrdenada([40, 10, 30, 20])
        self.assertEqual(list(lista), [10, 20, 30, 40])
        self.assertEqual(lista.buscar(30), 2)
        self.assertEqual(lista.buscar(35), -1)
        self.assertEqual(lista.minimo(), 10)
        self.assertEqual(lista.maximo(), 40)
        self.assertEqual(lista.remover_inicio(), 10)
        self.assertFalse(lista.remover(999))

    def test_mesclar(self):
        sorteio = random.Random(8)
        for _ in range(50):
            a = [sorteio.randrange(50) for _ in range(sorteio.randrange(10))]
            b = [sorteio.randrange(50) for _ in range(sorteio.randrange(10))]
            juntas = ListaOrdenada(a).mesclar(ListaOrdenada(b))
            self.assertEqual(list(juntas), sorted(a + b))
            self.assertEqual(len(juntas), len(a) + len(b))

    def test_lista_vazia(self):
        lista = ListaOrdenada()
        self.assertTrue(lista.vazia())
        for consulta in (lista.minimo, lista.maximo, lista.remover_inicio):
            with self.assertRaises(IndexError):
                consulta()


class TestListaHeterogenea(unittest.TestCase):
    def test_guarda_o_tipo_de_cada_no(self):
        lista = ListaHeterogenea()
        lista.inserir_inteiro(10)
        lista.inserir_real(3.5)
        lista.inserir_caractere("a")
        lista.inserir_texto("dados")
        lista.inserir(7)      # detecta sozinho
        lista.inserir(2.5)
        lista.inserir("xy")
        self.assertEqual(len(lista), 7)
        self.assertEqual(lista.filtrar(Tipo.INTEIRO), [10, 7])
        self.assertEqual(lista.filtrar(Tipo.REAL), [3.5, 2.5])
        self.assertEqual(lista.filtrar(Tipo.CARACTERE), ["a"])
        self.assertEqual(lista.filtrar(Tipo.TEXTO), ["dados", "xy"])
        self.assertEqual(lista.somar_numeros(), 23.0)
        self.assertEqual(lista.contar_por_tipo()[Tipo.INTEIRO], 2)

    def test_tipos_invalidos(self):
        lista = ListaHeterogenea()
        with self.assertRaises(TypeError):
            lista.inserir(True)          # bool não entra (é subclasse de int!)
        with self.assertRaises(TypeError):
            lista.inserir([1, 2])
        with self.assertRaises(TypeError):
            lista.inserir_inteiro(1.5)
        with self.assertRaises(TypeError):
            lista.inserir_caractere("abc")

    def test_remocao_zera_o_fim(self):
        lista = ListaHeterogenea()
        lista.inserir(1)
        self.assertEqual(lista.remover_inicio(), (Tipo.INTEIRO, 1))
        self.assertTrue(lista.vazia())
        self.assertIsNone(lista.fim)
        with self.assertRaises(IndexError):
            lista.remover_inicio()


if __name__ == "__main__":
    unittest.main()
