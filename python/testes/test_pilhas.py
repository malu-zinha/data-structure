"""Testes das pilhas e das suas aplicações clássicas.

Rode a partir da pasta `python/`:  python3 -m unittest
"""

import random
import unittest

from pilhas.aplicacoes_pilha import (
    avaliar_infixa,
    avaliar_posfixa,
    converter_base,
    eh_palindromo,
    infixa_para_posfixa,
    inverter_texto,
    parenteses_balanceados,
    proximo_maior_elemento,
    torre_de_hanoi,
)
from pilhas.pilha_encadeada import PilhaEncadeada
from pilhas.pilha_sequencial import PilhaSequencial


class ContratoPilha:
    """Testes aplicados às duas pilhas (a `list` do Python serve de modelo)."""

    def criar(self):
        raise NotImplementedError

    def esta_cheia(self, pilha):
        return hasattr(pilha, "cheia") and pilha.cheia()

    def test_comeca_vazia(self):
        pilha = self.criar()
        self.assertTrue(pilha.vazia())
        self.assertEqual(len(pilha), 0)

    def test_pilha_vazia_levanta_erro(self):
        pilha = self.criar()
        with self.assertRaises(IndexError):
            pilha.desempilhar()
        with self.assertRaises(IndexError):
            pilha.topo()

    def test_ordem_lifo(self):
        pilha = self.criar()
        for valor in (10, 20, 30):
            pilha.empilhar(valor)
        self.assertEqual(pilha.topo(), 30)
        self.assertEqual(len(pilha), 3)  # topo() não remove
        self.assertEqual([pilha.desempilhar() for _ in range(3)], [30, 20, 10])
        self.assertTrue(pilha.vazia())

    def test_operacoes_aleatorias(self):
        sorteio = random.Random(11)
        pilha = self.criar()
        modelo = []
        for _ in range(3000):
            operacao = sorteio.choice(["empilhar", "empilhar", "desempilhar", "topo", "limpar"])
            if operacao == "empilhar":
                if self.esta_cheia(pilha):
                    with self.assertRaises(OverflowError):
                        pilha.empilhar(0)
                    continue
                valor = sorteio.randrange(100)
                pilha.empilhar(valor)
                modelo.append(valor)
            elif operacao == "desempilhar" and modelo:
                self.assertEqual(pilha.desempilhar(), modelo.pop())
            elif operacao == "topo" and modelo:
                self.assertEqual(pilha.topo(), modelo[-1])
            elif operacao == "limpar" and sorteio.random() < 0.02:
                pilha.limpar()
                modelo.clear()
            self.assertEqual(list(pilha), modelo[::-1])  # a pilha percorre do topo à base
            self.assertEqual(len(pilha), len(modelo))
            self.assertEqual(pilha.vazia(), not modelo)


class TestPilhaSequencial(ContratoPilha, unittest.TestCase):
    def criar(self):
        return PilhaSequencial(capacidade=30)

    def test_estouro_de_capacidade(self):
        pilha = PilhaSequencial(capacidade=2)
        pilha.empilhar(1)
        pilha.empilhar(2)
        self.assertTrue(pilha.cheia())
        with self.assertRaises(OverflowError):
            pilha.empilhar(3)


class TestPilhaEncadeada(ContratoPilha, unittest.TestCase):
    def criar(self):
        return PilhaEncadeada()

    def test_cresce_sem_limite(self):
        pilha = PilhaEncadeada()
        for valor in range(10_000):
            pilha.empilhar(valor)
        self.assertEqual(len(pilha), 10_000)
        self.assertEqual(pilha.topo(), 9_999)


class TestParentesesBalanceados(unittest.TestCase):
    def test_casos_conhecidos(self):
        for expressao in ("", "()", "([]{})", "{[()()]}", "a(b)[c]{d}"):
            self.assertTrue(parenteses_balanceados(expressao), expressao)
        for expressao in ("(", ")", "([)]", "((a + b) * c", "a)b("):
            self.assertFalse(parenteses_balanceados(expressao), expressao)


class TestExpressoes(unittest.TestCase):
    def test_infixa_para_posfixa(self):
        casos = {
            "A + B": "A B +",
            "A + B * C": "A B C * +",
            "(A + B) * C": "A B + C *",
            "a - b - c": "a b - c -",            # associativo à esquerda
            "2 ^ 3 ^ 2": "2 3 2 ^ ^",            # ^ é associativo à direita
            "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3": "3 4 2 * 1 5 - 2 3 ^ ^ / +",
        }
        for infixa, posfixa in casos.items():
            self.assertEqual(infixa_para_posfixa(infixa), posfixa, infixa)

    def test_parenteses_desbalanceados_na_conversao(self):
        for expressao in ("(A + B", "A + B)"):
            with self.assertRaises(ValueError):
                infixa_para_posfixa(expressao)

    def test_simbolo_invalido(self):
        with self.assertRaises(ValueError):
            infixa_para_posfixa("A # B")

    def test_avaliar_posfixa(self):
        self.assertEqual(avaliar_posfixa("3 4 +"), 7)
        self.assertEqual(avaliar_posfixa("5 2 -"), 3)          # ordem dos operandos!
        self.assertEqual(avaliar_posfixa("5 1 2 + 4 * + 3 -"), 14)
        self.assertEqual(avaliar_posfixa("2 3 ^"), 8)
        self.assertEqual(avaliar_posfixa("x y *", {"x": 3, "y": 4}), 12)

    def test_avaliar_posfixa_mal_formada(self):
        for expressao in ("3 +", "3 4", ""):
            with self.assertRaises(ValueError):
                avaliar_posfixa(expressao)

    def test_avaliar_infixa_confere_com_o_python(self):
        sorteio = random.Random(42)
        for _ in range(200):
            a, b, c = (sorteio.randint(1, 9) for _ in range(3))
            expressoes = [
                (f"{a} + {b} * {c}", a + b * c),
                (f"({a} + {b}) * {c}", (a + b) * c),
                (f"{a} - {b} - {c}", a - b - c),
                (f"{a} * {b} / {c}", a * b / c),
                (f"{a} ^ {b}", a ** b),
                (f"({a} + {b}) / ({b} + {c})", (a + b) / (b + c)),
            ]
            for expressao, esperado in expressoes:
                self.assertAlmostEqual(avaliar_infixa(expressao), esperado, msg=expressao)


class TestOutrasAplicacoes(unittest.TestCase):
    def test_inverter_texto(self):
        sorteio = random.Random(1)
        for _ in range(100):
            texto = "".join(sorteio.choice("abcde ") for _ in range(sorteio.randrange(15)))
            self.assertEqual(inverter_texto(texto), texto[::-1])

    def test_palindromo(self):
        for frase in ("", "a", "arara", "Ame a ema", "A base do teto desaba",
                      "Socorram-me, subi no ônibus em Marrocos"):
            self.assertTrue(eh_palindromo(frase), frase)
        for frase in ("estrutura", "abcd", "quase arara?"):
            self.assertFalse(eh_palindromo(frase), frase)

    def test_converter_base(self):
        self.assertEqual(converter_base(0, 2), "0")
        self.assertEqual(converter_base(10, 2), "1010")
        self.assertEqual(converter_base(255, 16), "FF")
        self.assertEqual(converter_base(-5, 2), "-101")
        sorteio = random.Random(7)
        for _ in range(300):
            numero = sorteio.randrange(1, 10_000)
            self.assertEqual(converter_base(numero, 2), format(numero, "b"))
            self.assertEqual(converter_base(numero, 8), format(numero, "o"))
            self.assertEqual(converter_base(numero, 16), format(numero, "X"))
        with self.assertRaises(ValueError):
            converter_base(10, 20)

    def test_torre_de_hanoi(self):
        for discos in range(1, 9):
            movimentos = torre_de_hanoi(discos)
            self.assertEqual(len(movimentos), 2 ** discos - 1)
            # simula os movimentos e confere se nenhum disco maior ficou sobre um menor
            torres = {"A": list(range(discos, 0, -1)), "B": [], "C": []}
            for disco, origem, destino in movimentos:
                self.assertEqual(torres[origem][-1], disco)
                self.assertTrue(not torres[destino] or torres[destino][-1] > disco)
                torres[destino].append(torres[origem].pop())
            self.assertEqual(torres["C"], list(range(discos, 0, -1)))
            self.assertEqual(torres["A"], [])
        self.assertEqual(torre_de_hanoi(0), [])

    def test_proximo_maior_elemento(self):
        self.assertEqual(proximo_maior_elemento([4, 5, 2, 25]), [5, 25, 25, None])
        self.assertEqual(proximo_maior_elemento([]), [])
        sorteio = random.Random(9)
        for _ in range(200):
            valores = [sorteio.randrange(20) for _ in range(sorteio.randrange(12))]
            esperado = [
                next((v for v in valores[i + 1:] if v > valores[i]), None)
                for i in range(len(valores))
            ]
            self.assertEqual(proximo_maior_elemento(valores), esperado, valores)


if __name__ == "__main__":
    unittest.main()
