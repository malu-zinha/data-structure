"""
PILHA SEQUENCIAL (estática, com vetor)
======================================

- LIFO: Last In, First Out -> o ÚLTIMO a entrar é o PRIMEIRO a sair.
- Tudo acontece no TOPO. Pense numa pilha de pratos.
- Implementação com vetor de capacidade fixa e um índice `_topo`:
  `_topo = -1` significa pilha vazia.

     |    |
     | 30 |  <- topo (índice 2)
     | 20 |
     | 10 |
     +----+

Complexidade: empilhar, desempilhar, topo, vazia e cheia são todos O(1).

Pega-ratão
----------
- Checar VAZIA antes de desempilhar (stack underflow).
- Checar CHEIA antes de empilhar (stack overflow).
- `topo()` só consulta, não remove.
"""


class PilhaSequencial:
    def __init__(self, capacidade=10):
        if capacidade <= 0:
            raise ValueError("A capacidade deve ser positiva")
        self._dados = [None] * capacidade
        self._capacidade = capacidade
        self._topo = -1  # índice do elemento do topo (-1 = vazia)

    def vazia(self):
        return self._topo == -1

    def cheia(self):
        return self._topo == self._capacidade - 1

    def tamanho(self):
        return self._topo + 1

    def empilhar(self, valor):
        """push: coloca no topo."""
        if self.cheia():
            raise OverflowError("Pilha cheia")
        self._topo += 1
        self._dados[self._topo] = valor

    def desempilhar(self):
        """pop: tira e devolve o elemento do topo."""
        if self.vazia():
            raise IndexError("Pilha vazia")
        valor = self._dados[self._topo]
        self._dados[self._topo] = None
        self._topo -= 1
        return valor

    def topo(self):
        """peek/top: só olha o topo, sem remover."""
        if self.vazia():
            raise IndexError("Pilha vazia")
        return self._dados[self._topo]

    def limpar(self):
        self._dados = [None] * self._capacidade
        self._topo = -1

    def __len__(self):
        return self.tamanho()

    def __iter__(self):
        """Percorre do topo até a base (a ordem em que sairiam)."""
        for i in range(self._topo, -1, -1):
            yield self._dados[i]

    def __str__(self):
        base_ate_topo = [str(v) for v in reversed(list(self))]
        return "[" + ", ".join(base_ate_topo) + "] <- topo"


if __name__ == "__main__":
    pilha = PilhaSequencial(capacidade=3)
    for valor in (10, 20, 30):
        pilha.empilhar(valor)
        print(f"empilhou {valor}:", pilha)
    print("Cheia?", pilha.cheia(), "| topo:", pilha.topo())
    while not pilha.vazia():
        print("desempilhou", pilha.desempilhar(), "->", pilha)
    try:
        pilha.desempilhar()
    except IndexError as erro:
        print("Erro esperado:", erro)
