r"""
ÁRVORE AVL (Adelson-Velsky e Landis)
====================================

- É uma ABB que se AUTO-BALANCEIA: para TODO nó,
      fator de balanceamento (FB) = altura(esquerda) - altura(direita)
  precisa ficar em {-1, 0, +1}.
- Depois de inserir/remover, subindo pelo caminho, se algum nó ficar com
  FB = +2 ou -2, aplica-se uma ROTAÇÃO.
- Garante altura O(log n) -> busca, inserção e remoção O(log n) SEMPRE.

As 4 situações
--------------
FB do nó | FB do filho | caso | solução
---------|-------------|------|----------------------------------------------
   +2    |  >= 0 (esq) |  LL  | rotação simples à DIREITA
   -2    |  <= 0 (dir) |  RR  | rotação simples à ESQUERDA
   +2    |   -1  (esq) |  LR  | rotação dupla: esquerda no filho, direita no nó
   -2    |   +1  (dir) |  RL  | rotação dupla: direita no filho, esquerda no nó

Rotação à direita (caso LL):

          y                x
         / \              / \
        x   C    --->    A   y
       / \                  / \
      A   B                B   C

Pega-ratão
----------
- Depois de rotacionar, atualize a altura de quem ficou EMBAIXO primeiro.
- Na remoção pode ser preciso rotacionar em VÁRIOS níveis (na inserção, no
  máximo uma vez).
- Altura: folha = 0, vazia = -1 (o FB dá o mesmo resultado com outra convenção).
"""

from collections import deque


class No:
    def __init__(self, valor):
        self.valor = valor
        self.esquerda = None
        self.direita = None
        self.altura = 0  # folha


def _altura(no):
    return no.altura if no is not None else -1


def _atualizar_altura(no):
    no.altura = 1 + max(_altura(no.esquerda), _altura(no.direita))


def _fator_balanceamento(no):
    return _altura(no.esquerda) - _altura(no.direita)


class ArvoreAVL:
    def __init__(self, valores=()):
        self.raiz = None
        self._tamanho = 0
        self.rotacoes = []  # histórico das rotações, bom para conferir exercícios
        for valor in valores:
            self.inserir(valor)

    def vazia(self):
        return self.raiz is None

    def tamanho(self):
        return self._tamanho

    def altura(self):
        return _altura(self.raiz)

    # ==================================================================
    # Rotações
    # ==================================================================
    def _rotacao_direita(self, y):
        x = y.esquerda
        y.esquerda = x.direita  # a subárvore B troca de pai
        x.direita = y
        _atualizar_altura(y)    # y agora está embaixo: atualiza primeiro
        _atualizar_altura(x)
        self.rotacoes.append(f"direita em {y.valor}")
        return x                # x é a nova raiz desta subárvore

    def _rotacao_esquerda(self, x):
        y = x.direita
        x.direita = y.esquerda
        y.esquerda = x
        _atualizar_altura(x)
        _atualizar_altura(y)
        self.rotacoes.append(f"esquerda em {x.valor}")
        return y

    def _balancear(self, no):
        """Atualiza a altura e, se o nó estiver desbalanceado, aplica a rotação certa."""
        _atualizar_altura(no)
        fb = _fator_balanceamento(no)

        if fb > 1:  # pesado à ESQUERDA
            if _fator_balanceamento(no.esquerda) < 0:          # caso LR
                no.esquerda = self._rotacao_esquerda(no.esquerda)
            return self._rotacao_direita(no)                   # caso LL

        if fb < -1:  # pesado à DIREITA
            if _fator_balanceamento(no.direita) > 0:           # caso RL
                no.direita = self._rotacao_direita(no.direita)
            return self._rotacao_esquerda(no)                  # caso RR

        return no  # já estava balanceado

    # ==================================================================
    # Inserção e remoção (iguais às da ABB + balancear na volta da recursão)
    # ==================================================================
    def inserir(self, valor):
        if self.contem(valor):
            return False
        self.raiz = self._inserir(self.raiz, valor)
        self._tamanho += 1
        return True

    def _inserir(self, no, valor):
        if no is None:
            return No(valor)
        if valor < no.valor:
            no.esquerda = self._inserir(no.esquerda, valor)
        else:
            no.direita = self._inserir(no.direita, valor)
        return self._balancear(no)  # <- a única diferença para a ABB

    def remover(self, valor):
        if not self.contem(valor):
            return False
        self.raiz = self._remover(self.raiz, valor)
        self._tamanho -= 1
        return True

    def _remover(self, no, valor):
        if no is None:
            return None
        if valor < no.valor:
            no.esquerda = self._remover(no.esquerda, valor)
        elif valor > no.valor:
            no.direita = self._remover(no.direita, valor)
        else:
            if no.esquerda is None:
                return no.direita
            if no.direita is None:
                return no.esquerda
            sucessor = no.direita
            while sucessor.esquerda is not None:
                sucessor = sucessor.esquerda
            no.valor = sucessor.valor
            no.direita = self._remover(no.direita, sucessor.valor)
        return self._balancear(no)

    # ==================================================================
    # Consultas
    # ==================================================================
    def buscar(self, valor):
        atual = self.raiz
        while atual is not None and atual.valor != valor:
            atual = atual.esquerda if valor < atual.valor else atual.direita
        return atual

    def contem(self, valor):
        return self.buscar(valor) is not None

    def minimo(self):
        if self.raiz is None:
            raise ValueError("Árvore vazia")
        atual = self.raiz
        while atual.esquerda:
            atual = atual.esquerda
        return atual.valor

    def maximo(self):
        if self.raiz is None:
            raise ValueError("Árvore vazia")
        atual = self.raiz
        while atual.direita:
            atual = atual.direita
        return atual.valor

    def fator_balanceamento(self, valor):
        no = self.buscar(valor)
        if no is None:
            raise ValueError("Valor não está na árvore")
        return _fator_balanceamento(no)

    def pre_ordem(self):
        resultado = []

        def visitar(no):
            if no:
                resultado.append(no.valor)
                visitar(no.esquerda)
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def em_ordem(self):
        resultado = []

        def visitar(no):
            if no:
                visitar(no.esquerda)
                resultado.append(no.valor)
                visitar(no.direita)

        visitar(self.raiz)
        return resultado

    def pos_ordem(self):
        resultado = []

        def visitar(no):
            if no:
                visitar(no.esquerda)
                visitar(no.direita)
                resultado.append(no.valor)

        visitar(self.raiz)
        return resultado

    def em_nivel(self):
        resultado = []
        fila = deque([self.raiz] if self.raiz else [])
        while fila:
            no = fila.popleft()
            resultado.append(no.valor)
            if no.esquerda:
                fila.append(no.esquerda)
            if no.direita:
                fila.append(no.direita)
        return resultado

    def eh_valida(self):
        """Confere: ordem de ABB, alturas guardadas corretas e |FB| <= 1."""
        def verificar(no, menor, maior):
            if no is None:
                return True, -1
            if (menor is not None and no.valor <= menor) or (maior is not None and no.valor >= maior):
                return False, 0
            ok_esq, h_esq = verificar(no.esquerda, menor, no.valor)
            ok_dir, h_dir = verificar(no.direita, no.valor, maior)
            altura = 1 + max(h_esq, h_dir)
            ok = ok_esq and ok_dir and abs(h_esq - h_dir) <= 1 and no.altura == altura
            return ok, altura

        return verificar(self.raiz, None, None)[0]

    def __len__(self):
        return self._tamanho

    def __contains__(self, valor):
        return self.contem(valor)

    def __iter__(self):
        return iter(self.em_ordem())

    def __str__(self):
        """Desenho deitado mostrando o fator de balanceamento de cada nó."""
        if self.raiz is None:
            return "(árvore vazia)"
        linhas = []

        def desenhar(no, nivel):
            if no:
                desenhar(no.direita, nivel + 1)
                linhas.append("      " * nivel + f"{no.valor}(fb={_fator_balanceamento(no)})")
                desenhar(no.esquerda, nivel + 1)

        desenhar(self.raiz, 0)
        return "\n".join(linhas)


if __name__ == "__main__":
    avl = ArvoreAVL()
    for valor in (10, 20, 30, 40, 50, 25):
        avl.rotacoes.clear()
        avl.inserir(valor)
        print(f"inseriu {valor:>2} | rotações: {avl.rotacoes or 'nenhuma'}")
    print(avl)
    print("Em ordem:", avl.em_ordem(), "| altura:", avl.altura(), "| válida?", avl.eh_valida())

    for valor in (40, 50):
        avl.rotacoes.clear()
        avl.remover(valor)
        print(f"removeu {valor} | rotações: {avl.rotacoes or 'nenhuma'}")
    print(avl)

    grande = ArvoreAVL(range(1, 1001))
    print("\n1000 valores inseridos EM ORDEM -> altura", grande.altura(), "(uma ABB comum teria 999)")
