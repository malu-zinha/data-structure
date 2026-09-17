"""
PILHA ENCADEADA (dinâmica)
==========================

- LIFO: o último a entrar é o primeiro a sair.
- Cada nó aponta para o nó de BAIXO. A pilha só guarda o nó do `_topo`.
- Não tem limite de capacidade (cresce enquanto houver memória).

    _topo -> [30] -> [20] -> [10] -> None
                              (base)

Empilhar = inserir no início da lista encadeada.
Desempilhar = remover do início da lista encadeada.

Complexidade: empilhar, desempilhar, topo e vazia são O(1).

Pega-ratão
----------
- Checar se está vazia antes de desempilhar.
- Não existe "cheia" (a não ser que a memória acabe).
"""


class No:
    def __init__(self, valor, abaixo=None):
        self.valor = valor
        self.abaixo = abaixo  # o nó que está embaixo deste na pilha


class PilhaEncadeada:
    def __init__(self):
        self._topo = None
        self._tamanho = 0

    def vazia(self):
        return self._topo is None

    def tamanho(self):
        return self._tamanho

    def empilhar(self, valor):
        # o novo nó fica por cima e aponta para o antigo topo
        self._topo = No(valor, self._topo)
        self._tamanho += 1

    def desempilhar(self):
        if self.vazia():
            raise IndexError("Pilha vazia")
        no = self._topo
        self._topo = no.abaixo  # quem estava embaixo vira o topo
        self._tamanho -= 1
        return no.valor

    def topo(self):
        if self.vazia():
            raise IndexError("Pilha vazia")
        return self._topo.valor

    def limpar(self):
        self._topo = None
        self._tamanho = 0

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        """Percorre do topo até a base."""
        atual = self._topo
        while atual is not None:
            yield atual.valor
            atual = atual.abaixo

    def __str__(self):
        base_ate_topo = [str(v) for v in reversed(list(self))]
        return "[" + ", ".join(base_ate_topo) + "] <- topo"


if __name__ == "__main__":
    pilha = PilhaEncadeada()
    for valor in ("a", "b", "c"):
        pilha.empilhar(valor)
    print("Pilha:", pilha, "| tamanho:", len(pilha), "| topo:", pilha.topo())
    print("Desempilhou:", pilha.desempilhar(), "->", pilha)
    print("Ordem de saída do resto:", list(pilha))
