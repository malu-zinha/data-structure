"""
APLICAÇÕES CLÁSSICAS DE PILHA
=============================

1. parenteses_balanceados   -> "([]{})" está bem formado?
2. infixa_para_posfixa      -> "A + B * C" vira "A B C * +" (algoritmo de Shunting-yard)
3. avaliar_posfixa          -> calcula "3 4 2 * +" = 11
4. avaliar_infixa           -> junta os dois acima: "3 + 4 * 2" = 11
5. inverter_texto           -> "abc" vira "cba"
6. eh_palindromo            -> "Ame a ema" é palíndromo?
7. converter_base           -> 10 na base 2 = "1010" (divisões sucessivas)
8. torre_de_hanoi           -> resolve Hanói com 3 pilhas de verdade
9. proximo_maior_elemento   -> para cada item, o próximo maior à direita (pilha monotônica)

Rode a partir da pasta `python/`:  python3 stack/aplicacoes_pilha.py
"""

import re
import unicodedata

from pilha_encadeada import PilhaEncadeada

ABRE_PARA_FECHA = {"(": ")", "[": "]", "{": "}"}
FECHA_PARA_ABRE = {")": "(", "]": "[", "}": "{"}

PRECEDENCIA = {"+": 1, "-": 1, "*": 2, "/": 2, "^": 3}
ASSOCIATIVO_A_DIREITA = {"^"}  # 2 ^ 3 ^ 2 = 2 ^ (3 ^ 2)


# ----------------------------------------------------------------------
# 1. Parênteses balanceados
# ----------------------------------------------------------------------
def parenteses_balanceados(expressao):
    """Todo símbolo que abre empilha; todo que fecha precisa casar com o topo."""
    pilha = PilhaEncadeada()
    for simbolo in expressao:
        if simbolo in ABRE_PARA_FECHA:
            pilha.empilhar(simbolo)
        elif simbolo in FECHA_PARA_ABRE:
            if pilha.vazia() or pilha.desempilhar() != FECHA_PARA_ABRE[simbolo]:
                return False
    return pilha.vazia()  # sobrou algum aberto? então não está balanceado


# ----------------------------------------------------------------------
# 2, 3 e 4. Expressões infixa / pós-fixa
# ----------------------------------------------------------------------
def tokenizar(expressao):
    """Quebra a expressão em números, variáveis, operadores e parênteses."""
    tokens = re.findall(r"\d+(?:\.\d+)?|[A-Za-z_]\w*|\S", expressao)
    for token in tokens:
        eh_operando = token[0].isdigit() or token[0].isalpha() or token[0] == "_"
        if not (eh_operando or token in PRECEDENCIA or token in "()"):
            raise ValueError(f"Símbolo inválido: {token!r}")
    return tokens


def infixa_para_posfixa(expressao):
    """Converte notação infixa para pós-fixa (notação polonesa reversa).

    Regras (Shunting-yard, de Dijkstra):
    - operando  -> vai direto para a saída;
    - "("       -> empilha;
    - ")"       -> desempilha para a saída até achar o "(";
    - operador  -> antes de empilhar, desempilha os operadores do topo que têm
                   precedência MAIOR (ou IGUAL, se for associativo à esquerda);
    - no fim    -> desempilha tudo que sobrou.

    Não trata menos unário (ex.: "-3").
    """
    pilha = PilhaEncadeada()
    saida = []

    for token in tokenizar(expressao):
        if token == "(":
            pilha.empilhar(token)
        elif token == ")":
            while not pilha.vazia() and pilha.topo() != "(":
                saida.append(pilha.desempilhar())
            if pilha.vazia():
                raise ValueError("Parênteses desbalanceados")
            pilha.desempilhar()  # descarta o "("
        elif token in PRECEDENCIA:
            while not pilha.vazia() and pilha.topo() != "(":
                topo = pilha.topo()
                sai_antes = PRECEDENCIA[topo] > PRECEDENCIA[token] or (
                    PRECEDENCIA[topo] == PRECEDENCIA[token] and token not in ASSOCIATIVO_A_DIREITA
                )
                if not sai_antes:
                    break
                saida.append(pilha.desempilhar())
            pilha.empilhar(token)
        else:
            saida.append(token)  # operando

    while not pilha.vazia():
        topo = pilha.desempilhar()
        if topo == "(":
            raise ValueError("Parênteses desbalanceados")
        saida.append(topo)

    return " ".join(saida)


def avaliar_posfixa(expressao, variaveis=None):
    """Calcula uma expressão pós-fixa com tokens separados por espaço.

    Operando -> empilha. Operador -> desempilha DOIS, calcula e empilha o resultado.
    Pega-ratão: o primeiro desempilhado é o operando da DIREITA ("5 2 -" = 5 - 2).
    """
    variaveis = variaveis or {}
    pilha = PilhaEncadeada()

    for token in expressao.split():
        if token in PRECEDENCIA:
            if len(pilha) < 2:
                raise ValueError("Expressão mal formada")
            direita = pilha.desempilhar()
            esquerda = pilha.desempilhar()
            pilha.empilhar(_aplicar(token, esquerda, direita))
        elif re.fullmatch(r"\d+", token):
            pilha.empilhar(int(token))
        elif re.fullmatch(r"\d+\.\d+", token):
            pilha.empilhar(float(token))
        elif token in variaveis:
            pilha.empilhar(variaveis[token])
        else:
            raise ValueError(f"Token desconhecido: {token!r}")

    if len(pilha) != 1:
        raise ValueError("Expressão mal formada")
    return pilha.desempilhar()


def avaliar_infixa(expressao, variaveis=None):
    return avaliar_posfixa(infixa_para_posfixa(expressao), variaveis)


def _aplicar(operador, a, b):
    if operador == "+":
        return a + b
    if operador == "-":
        return a - b
    if operador == "*":
        return a * b
    if operador == "/":
        return a / b
    return a ** b  # "^"


# ----------------------------------------------------------------------
# 5 e 6. Inversão e palíndromo
# ----------------------------------------------------------------------
def inverter_texto(texto):
    pilha = PilhaEncadeada()
    for caractere in texto:
        pilha.empilhar(caractere)
    invertido = []
    while not pilha.vazia():
        invertido.append(pilha.desempilhar())
    return "".join(invertido)


def eh_palindromo(texto):
    """Ignora espaços, pontuação, maiúsculas e acentos."""
    sem_acento = unicodedata.normalize("NFD", texto)
    letras = [c.lower() for c in sem_acento if c.isalnum()]

    pilha = PilhaEncadeada()
    for letra in letras:
        pilha.empilhar(letra)
    # a pilha devolve as letras na ordem inversa: basta comparar uma a uma
    return all(letra == pilha.desempilhar() for letra in letras)


# ----------------------------------------------------------------------
# 7. Conversão de base
# ----------------------------------------------------------------------
def converter_base(numero, base=2):
    """Divisões sucessivas: os restos saem de trás para frente -> pilha!"""
    if not 2 <= base <= 16:
        raise ValueError("Base deve estar entre 2 e 16")
    if numero == 0:
        return "0"
    digitos = "0123456789ABCDEF"
    sinal = "-" if numero < 0 else ""
    numero = abs(numero)

    pilha = PilhaEncadeada()
    while numero > 0:
        pilha.empilhar(digitos[numero % base])
        numero //= base

    resultado = []
    while not pilha.vazia():
        resultado.append(pilha.desempilhar())
    return sinal + "".join(resultado)


# ----------------------------------------------------------------------
# 8. Torre de Hanói
# ----------------------------------------------------------------------
def torre_de_hanoi(n):
    """Move n discos da torre A para a C usando B. Cada torre é uma pilha.

    Devolve a lista de movimentos (disco, origem, destino). Total = 2^n - 1.
    """
    torres = {"A": PilhaEncadeada(), "B": PilhaEncadeada(), "C": PilhaEncadeada()}
    for disco in range(n, 0, -1):  # disco maior embaixo
        torres["A"].empilhar(disco)
    movimentos = []

    def mover(quantidade, origem, destino, auxiliar):
        if quantidade == 0:
            return
        mover(quantidade - 1, origem, auxiliar, destino)  # tira os de cima do caminho
        disco = torres[origem].desempilhar()
        # regra do jogo: nunca colocar disco maior sobre um menor
        assert torres[destino].vazia() or torres[destino].topo() > disco
        torres[destino].empilhar(disco)
        movimentos.append((disco, origem, destino))
        mover(quantidade - 1, auxiliar, destino, origem)  # coloca-os de volta por cima

    mover(n, "A", "C", "B")
    return movimentos


# ----------------------------------------------------------------------
# 9. Próximo maior elemento (pilha monotônica)
# ----------------------------------------------------------------------
def proximo_maior_elemento(valores):
    """[4, 5, 2, 25] -> [5, 25, 25, None] em O(n).

    A pilha guarda ÍNDICES de quem ainda está esperando um valor maior.
    """
    resposta = [None] * len(valores)
    pilha = PilhaEncadeada()
    for i, valor in enumerate(valores):
        while not pilha.vazia() and valores[pilha.topo()] < valor:
            resposta[pilha.desempilhar()] = valor
        pilha.empilhar(i)
    return resposta


if __name__ == "__main__":
    for expr in ("([]{})", "([)]", "((a + b) * c"):
        print(f"balanceada({expr!r}) = {parenteses_balanceados(expr)}")

    for expr in ("A + B * C", "(A + B) * C", "3 + 4 * 2 / (1 - 5) ^ 2 ^ 3"):
        print(f"infixa: {expr:<30} pós-fixa: {infixa_para_posfixa(expr)}")

    print("avaliar_posfixa('5 1 2 + 4 * + 3 -') =", avaliar_posfixa("5 1 2 + 4 * + 3 -"))
    print("avaliar_infixa('(2 + 3) * x', x=4) =", avaliar_infixa("(2 + 3) * x", {"x": 4}))
    print("inverter_texto('estrutura') =", inverter_texto("estrutura"))
    print("eh_palindromo('Socorram-me, subi no ônibus em Marrocos') =",
          eh_palindromo("Socorram-me, subi no ônibus em Marrocos"))
    print("converter_base(10, 2) =", converter_base(10, 2), "| converter_base(255, 16) =", converter_base(255, 16))
    movimentos = torre_de_hanoi(3)
    print(f"Hanói com 3 discos: {len(movimentos)} movimentos")
    for disco, origem, destino in movimentos:
        print(f"  disco {disco}: {origem} -> {destino}")
    print("proximo_maior_elemento([4, 5, 2, 25]) =", proximo_maior_elemento([4, 5, 2, 25]))
