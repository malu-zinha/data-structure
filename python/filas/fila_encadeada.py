"""
FILA ENCADEADA (dinâmica)
=========================

- FIFO: entra no FIM, sai do INÍCIO.
- Guardamos dois ponteiros: `inicio` (quem sai) e `fim` (onde entra).

    inicio                     fim
      |                         |
      v                         v
    [10] --> [20] --> [30] --> [40] --> None

Complexidade: enfileirar, desenfileirar, frente e vazia são O(1).

Pega-ratão
----------
- Quando a fila ESVAZIA, zere também o `fim` (senão ele aponta para um nó que
  já saiu da fila).
- Na primeira inserção, o nó novo é `inicio` e `fim` ao mesmo tempo.
"""


class No:
    def __init__(self, valor):
        self.valor = valor
        self.proximo = None


class FilaEncadeada:
    def __init__(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    def vazia(self):
        return self.inicio is None

    def tamanho(self):
        return self._tamanho

    def enfileirar(self, valor):
        novo = No(valor)
        if self.vazia():
            self.inicio = novo       # único elemento: é início e fim
        else:
            self.fim.proximo = novo  # o antigo último aponta para o novo
        self.fim = novo
        self._tamanho += 1

    def desenfileirar(self):
        if self.vazia():
            raise IndexError("Fila vazia")
        no = self.inicio
        self.inicio = no.proximo
        if self.inicio is None:
            self.fim = None          # esvaziou: zera o fim também!
        self._tamanho -= 1
        return no.valor

    def frente(self):
        if self.vazia():
            raise IndexError("Fila vazia")
        return self.inicio.valor

    def ultimo(self):
        if self.vazia():
            raise IndexError("Fila vazia")
        return self.fim.valor

    def limpar(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        atual = self.inicio
        while atual is not None:
            yield atual.valor
            atual = atual.proximo

    def __str__(self):
        return "início -> [" + ", ".join(str(v) for v in self) + "] <- fim"


if __name__ == "__main__":
    fila = FilaEncadeada()
    for pessoa in ("Ana", "Bia", "Caio"):
        fila.enfileirar(pessoa)
    print("Fila:", fila)
    print("Atendida:", fila.desenfileirar(), "->", fila)
    fila.enfileirar("Davi")
    print("Chegou Davi:", fila, "| frente:", fila.frente(), "| último:", fila.ultimo())
