r"""
ÁRVORE RUBRO-NEGRA (Red-Black Tree)
===================================

É uma ABB em que cada nó tem uma COR (vermelho ou preto) e valem as regras:

1. Todo nó é VERMELHO ou PRETO.
2. A RAIZ é preta.
3. Toda folha nula (NIL) é preta.
4. Nó VERMELHO não pode ter filho vermelho (nada de dois vermelhos seguidos).
5. Todo caminho de um nó até as folhas NIL tem a MESMA quantidade de nós
   pretos (a "altura negra").

Consequência: o caminho mais longo tem no máximo o dobro do mais curto, então
a altura é no máximo 2*log2(n+1) -> busca, inserção e remoção O(log n).

AVL x Rubro-Negra
-----------------
- AVL é MAIS balanceada: buscas um pouco mais rápidas.
- Rubro-Negra faz MENOS rotações para inserir/remover. É a usada no TreeMap
  do Java, no std::map do C++ e no escalonador do Linux.

Inserção: o nó novo entra VERMELHO. Se o pai também for vermelho (fere a regra 4):
  - Caso 1: tio VERMELHO    -> recolore pai e tio de preto, avô de vermelho, sobe.
  - Caso 2: tio PRETO, nó em "joelho" (zigue-zague) -> rotaciona o pai e cai no caso 3.
  - Caso 3: tio PRETO, nó em "linha" -> rotaciona o avô e troca as cores.

Implementação baseada no livro do Cormen (CLRS), com um nó SENTINELA `NIL`
(preto) no lugar de todos os None. Isso evita um monte de `if no is None`.
"""

from collections import deque

VERMELHO = "V"
PRETO = "P"


class No:
    def __init__(self, valor, cor, nil=None):
        self.valor = valor
        self.cor = cor
        self.esquerda = nil
        self.direita = nil
        self.pai = nil


class ArvoreRubroNegra:
    def __init__(self, valores=()):
        self.NIL = No(None, PRETO)  # sentinela: representa todas as folhas nulas
        self.raiz = self.NIL
        self._tamanho = 0
        for valor in valores:
            self.inserir(valor)

    def vazia(self):
        return self.raiz is self.NIL

    def tamanho(self):
        return self._tamanho

    # ==================================================================
    # Rotações (iguais às da AVL, mas também atualizam o ponteiro `pai`)
    # ==================================================================
    def _rotacao_esquerda(self, x):
        y = x.direita
        x.direita = y.esquerda
        if y.esquerda is not self.NIL:
            y.esquerda.pai = x
        y.pai = x.pai
        if x.pai is self.NIL:
            self.raiz = y
        elif x is x.pai.esquerda:
            x.pai.esquerda = y
        else:
            x.pai.direita = y
        y.esquerda = x
        x.pai = y

    def _rotacao_direita(self, x):
        y = x.esquerda
        x.esquerda = y.direita
        if y.direita is not self.NIL:
            y.direita.pai = x
        y.pai = x.pai
        if x.pai is self.NIL:
            self.raiz = y
        elif x is x.pai.direita:
            x.pai.direita = y
        else:
            x.pai.esquerda = y
        y.direita = x
        x.pai = y

    # ==================================================================
    # Inserção
    # ==================================================================
    def inserir(self, valor):
        pai = self.NIL
        atual = self.raiz
        while atual is not self.NIL:  # desce como numa ABB comum
            if valor == atual.valor:
                return False
            pai = atual
            atual = atual.esquerda if valor < atual.valor else atual.direita

        novo = No(valor, VERMELHO, self.NIL)  # todo nó novo nasce vermelho
        novo.pai = pai
        if pai is self.NIL:
            self.raiz = novo
        elif valor < pai.valor:
            pai.esquerda = novo
        else:
            pai.direita = novo

        self._tamanho += 1
        self._corrigir_insercao(novo)
        return True

    def _corrigir_insercao(self, no):
        while no.pai.cor == VERMELHO:  # dois vermelhos seguidos: precisa arrumar
            avo = no.pai.pai
            if no.pai is avo.esquerda:
                tio = avo.direita
                if tio.cor == VERMELHO:                 # Caso 1: só recolore
                    no.pai.cor = PRETO
                    tio.cor = PRETO
                    avo.cor = VERMELHO
                    no = avo                            # o problema pode ter subido
                else:
                    if no is no.pai.direita:            # Caso 2: joelho -> vira linha
                        no = no.pai
                        self._rotacao_esquerda(no)
                    no.pai.cor = PRETO                  # Caso 3: linha
                    no.pai.pai.cor = VERMELHO
                    self._rotacao_direita(no.pai.pai)
            else:  # espelho: o pai é filho DIREITO do avô
                tio = avo.esquerda
                if tio.cor == VERMELHO:
                    no.pai.cor = PRETO
                    tio.cor = PRETO
                    avo.cor = VERMELHO
                    no = avo
                else:
                    if no is no.pai.esquerda:
                        no = no.pai
                        self._rotacao_direita(no)
                    no.pai.cor = PRETO
                    no.pai.pai.cor = VERMELHO
                    self._rotacao_esquerda(no.pai.pai)
        self.raiz.cor = PRETO  # regra 2

    # ==================================================================
    # Remoção
    # ==================================================================
    def _transplantar(self, u, v):
        """Coloca a subárvore `v` no lugar da subárvore `u`."""
        if u.pai is self.NIL:
            self.raiz = v
        elif u is u.pai.esquerda:
            u.pai.esquerda = v
        else:
            u.pai.direita = v
        v.pai = u.pai

    def remover(self, valor):
        z = self._buscar_no(valor)
        if z is self.NIL:
            return False

        y = z                  # nó que sai de fato da árvore
        cor_que_saiu = y.cor
        if z.esquerda is self.NIL:
            x = z.direita
            self._transplantar(z, z.direita)
        elif z.direita is self.NIL:
            x = z.esquerda
            self._transplantar(z, z.esquerda)
        else:
            y = self._minimo_no(z.direita)  # sucessor
            cor_que_saiu = y.cor
            x = y.direita
            if y.pai is z:
                x.pai = y
            else:
                self._transplantar(y, y.direita)
                y.direita = z.direita
                y.direita.pai = y
            self._transplantar(z, y)
            y.esquerda = z.esquerda
            y.esquerda.pai = y
            y.cor = z.cor

        self._tamanho -= 1
        # Se saiu um PRETO, algum caminho ficou com um preto a menos (regra 5).
        if cor_que_saiu == PRETO:
            self._corrigir_remocao(x)
        return True

    def _corrigir_remocao(self, x):
        # `x` carrega um "preto extra" que precisa ser resolvido.
        while x is not self.raiz and x.cor == PRETO:
            if x is x.pai.esquerda:
                irmao = x.pai.direita
                if irmao.cor == VERMELHO:                                 # Caso 1
                    irmao.cor = PRETO
                    x.pai.cor = VERMELHO
                    self._rotacao_esquerda(x.pai)
                    irmao = x.pai.direita
                if irmao.esquerda.cor == PRETO and irmao.direita.cor == PRETO:  # Caso 2
                    irmao.cor = VERMELHO
                    x = x.pai
                else:
                    if irmao.direita.cor == PRETO:                        # Caso 3
                        irmao.esquerda.cor = PRETO
                        irmao.cor = VERMELHO
                        self._rotacao_direita(irmao)
                        irmao = x.pai.direita
                    irmao.cor = x.pai.cor                                 # Caso 4
                    x.pai.cor = PRETO
                    irmao.direita.cor = PRETO
                    self._rotacao_esquerda(x.pai)
                    x = self.raiz
            else:  # espelho
                irmao = x.pai.esquerda
                if irmao.cor == VERMELHO:
                    irmao.cor = PRETO
                    x.pai.cor = VERMELHO
                    self._rotacao_direita(x.pai)
                    irmao = x.pai.esquerda
                if irmao.direita.cor == PRETO and irmao.esquerda.cor == PRETO:
                    irmao.cor = VERMELHO
                    x = x.pai
                else:
                    if irmao.esquerda.cor == PRETO:
                        irmao.direita.cor = PRETO
                        irmao.cor = VERMELHO
                        self._rotacao_esquerda(irmao)
                        irmao = x.pai.esquerda
                    irmao.cor = x.pai.cor
                    x.pai.cor = PRETO
                    irmao.esquerda.cor = PRETO
                    self._rotacao_direita(x.pai)
                    x = self.raiz
        x.cor = PRETO

    # ==================================================================
    # Consultas
    # ==================================================================
    def _buscar_no(self, valor):
        atual = self.raiz
        while atual is not self.NIL and atual.valor != valor:
            atual = atual.esquerda if valor < atual.valor else atual.direita
        return atual

    def _minimo_no(self, no):
        while no.esquerda is not self.NIL:
            no = no.esquerda
        return no

    def contem(self, valor):
        return self._buscar_no(valor) is not self.NIL

    def cor_de(self, valor):
        no = self._buscar_no(valor)
        if no is self.NIL:
            raise ValueError("Valor não está na árvore")
        return no.cor

    def minimo(self):
        if self.vazia():
            raise ValueError("Árvore vazia")
        return self._minimo_no(self.raiz).valor

    def maximo(self):
        if self.vazia():
            raise ValueError("Árvore vazia")
        atual = self.raiz
        while atual.direita is not self.NIL:
            atual = atual.direita
        return atual.valor

    def altura(self):
        def calcular(no):
            if no is self.NIL:
                return -1
            return 1 + max(calcular(no.esquerda), calcular(no.direita))

        return calcular(self.raiz)

    def altura_negra(self):
        """Nº de nós pretos da raiz até uma folha NIL (sem contar a raiz, contando o NIL)."""
        contagem = 0
        atual = self.raiz
        while atual is not self.NIL:
            atual = atual.esquerda
            if atual.cor == PRETO:
                contagem += 1
        return contagem

    def pre_ordem(self):
        resultado = []

        def visitar(no):
            if no is not self.NIL:
                resultado.append(no.valor)
                visitar(no.esquerda)
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def em_ordem(self):
        resultado = []

        def visitar(no):
            if no is not self.NIL:
                visitar(no.esquerda)
                resultado.append(no.valor)
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def pos_ordem(self):
        resultado = []

        def visitar(no):
            if no is not self.NIL:
                visitar(no.esquerda)
                visitar(no.direita)
                resultado.append(no.valor)

        visitar(self.raiz)
        return resultado

    def em_nivel(self):
        resultado = []
        fila = deque([] if self.vazia() else [self.raiz])
        while fila:
            no = fila.popleft()
            resultado.append(no.valor)
            for filho in (no.esquerda, no.direita):
                if filho is not self.NIL:
                    fila.append(filho)
        return resultado

    def eh_valida(self):
        """Confere as 5 regras + ordem de ABB + ponteiros `pai`."""
        if self.raiz.cor != PRETO or self.NIL.cor != PRETO:
            return False

        def verificar(no, menor, maior):
            """Devolve a altura negra da subárvore ou -1 se alguma regra falhar."""
            if no is self.NIL:
                return 1
            if (menor is not None and no.valor <= menor) or (maior is not None and no.valor >= maior):
                return -1
            for filho in (no.esquerda, no.direita):
                if filho is not self.NIL and filho.pai is not no:
                    return -1
                if no.cor == VERMELHO and filho.cor == VERMELHO:
                    return -1
            esquerda = verificar(no.esquerda, menor, no.valor)
            direita = verificar(no.direita, no.valor, maior)
            if esquerda == -1 or direita == -1 or esquerda != direita:
                return -1
            return esquerda + (1 if no.cor == PRETO else 0)

        return verificar(self.raiz, None, None) != -1

    def __len__(self):
        return self._tamanho

    def __contains__(self, valor):
        return self.contem(valor)

    def __iter__(self):
        return iter(self.em_ordem())

    def __str__(self):
        """Desenho deitado: V = vermelho, P = preto."""
        if self.vazia():
            return "(árvore vazia)"
        linhas = []

        def desenhar(no, nivel):
            if no is not self.NIL:
                desenhar(no.direita, nivel + 1)
                linhas.append("     " * nivel + f"{no.valor}[{no.cor}]")
                desenhar(no.esquerda, nivel + 1)

        desenhar(self.raiz, 0)
        return "\n".join(linhas)


if __name__ == "__main__":
    arvore = ArvoreRubroNegra()
    for valor in (10, 20, 30, 15, 25, 5, 1):
        arvore.inserir(valor)
    print(arvore)
    print("Em ordem:", arvore.em_ordem())
    print("Altura:", arvore.altura(), "| altura negra:", arvore.altura_negra(), "| válida?", arvore.eh_valida())

    arvore.remover(20)
    arvore.remover(10)
    print("\nDepois de remover 20 e 10:")
    print(arvore)
    print("Válida?", arvore.eh_valida())

    grande = ArvoreRubroNegra(range(1, 1001))
    print("\n1000 valores inseridos EM ORDEM -> altura", grande.altura())
