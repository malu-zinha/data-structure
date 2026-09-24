"""
LISTA CIRCULAR (simplesmente encadeada)
=======================================

- Igual à lista encadeada, mas o ÚLTIMO nó aponta de volta para o PRIMEIRO.
  Não existe None no fim.
- Guardamos só o ponteiro para o ÚLTIMO nó (`fim`). O primeiro é `fim.proximo`.
  Assim inserir no início E no fim ficam O(1).

        +-------------------------------+
        v                               |
      [10] --> [20] --> [30] --> [40] --+
                                 fim

Complexidade
------------
| operação                          | custo |
|-----------------------------------|-------|
| inserir no início / no fim        | O(1)  |
| remover no início                 | O(1)  |
| remover no fim                    | O(n)  |  (precisa achar o penúltimo)
| buscar / obter                    | O(n)  |

Pega-ratão
----------
- NUNCA use `while atual is not None` para percorrer: não há None, vira loop
  infinito! Pare quando voltar ao primeiro nó (ou conte `tamanho` passos).
- Com um único nó, ele aponta para ELE MESMO.
- Aplicação clássica: problema de Josephus, escalonamento round-robin.
"""


class No:
    def __init__(self, valor, proximo=None):
        self.valor = valor
        self.proximo = proximo


class ListaCircular:
    def __init__(self):
        self.fim = None  # último nó; o primeiro é self.fim.proximo
        self._tamanho = 0

    @property
    def inicio(self):
        return self.fim.proximo if self.fim is not None else None

    # ------------------------------------------------------------------
    # Consultas
    # ------------------------------------------------------------------
    def vazia(self):
        return self.fim is None

    def tamanho(self):
        return self._tamanho

    def obter(self, pos):
        return self._no_na_posicao(pos).valor

    def alterar(self, pos, valor):
        self._no_na_posicao(pos).valor = valor

    def buscar(self, valor):
        for pos, atual in enumerate(self):
            if atual == valor:
                return pos
        return -1

    # ------------------------------------------------------------------
    # Inserção
    # ------------------------------------------------------------------
    def inserir_inicio(self, valor):
        novo = No(valor)
        if self.vazia():
            novo.proximo = novo              # sozinho: aponta para si mesmo
            self.fim = novo
        else:
            novo.proximo = self.fim.proximo  # novo aponta para o antigo primeiro
            self.fim.proximo = novo          # o último passa a apontar para o novo
        self._tamanho += 1

    def inserir_fim(self, valor):
        # Truque: insere no início e "anda" o fim uma casa.
        # O nó novo, que estava logo depois do fim, vira o próprio fim.
        self.inserir_inicio(valor)
        self.fim = self.fim.proximo

    def inserir_posicao(self, pos, valor):
        if pos < 0 or pos > self._tamanho:
            raise IndexError("Posição inválida")
        if pos == 0:
            self.inserir_inicio(valor)
        elif pos == self._tamanho:
            self.inserir_fim(valor)
        else:
            anterior = self._no_na_posicao(pos - 1)
            anterior.proximo = No(valor, anterior.proximo)
            self._tamanho += 1

    # ------------------------------------------------------------------
    # Remoção
    # ------------------------------------------------------------------
    def remover_inicio(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        primeiro = self.fim.proximo
        if primeiro is self.fim:              # era o único nó
            self.fim = None
        else:
            self.fim.proximo = primeiro.proximo  # o último pula o antigo primeiro
        self._tamanho -= 1
        return primeiro.valor

    def remover_fim(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        if self._tamanho == 1:
            return self.remover_inicio()
        penultimo = self._no_na_posicao(self._tamanho - 2)
        valor = self.fim.valor
        penultimo.proximo = self.fim.proximo  # penúltimo aponta para o primeiro
        self.fim = penultimo
        self._tamanho -= 1
        return valor

    def remover_posicao(self, pos):
        if self.vazia():
            raise IndexError("Lista vazia")
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        if pos == 0:
            return self.remover_inicio()
        if pos == self._tamanho - 1:
            return self.remover_fim()
        anterior = self._no_na_posicao(pos - 1)
        removido = anterior.proximo
        anterior.proximo = removido.proximo
        self._tamanho -= 1
        return removido.valor

    def remover_valor(self, valor):
        pos = self.buscar(valor)
        if pos == -1:
            return False
        self.remover_posicao(pos)
        return True

    # ------------------------------------------------------------------
    # Outras operações
    # ------------------------------------------------------------------
    def rotacionar(self, passos=1):
        """Gira a lista: o início anda `passos` casas para frente -> O(passos).

        [1, 2, 3, 4] rotacionar(1) -> [2, 3, 4, 1]
        Na lista circular basta mover o ponteiro `fim`; nenhum nó muda.
        """
        if self.vazia():
            return
        for _ in range(passos % self._tamanho):
            self.fim = self.fim.proximo

    def inverter(self):
        if self._tamanho < 2:
            return
        primeiro = self.fim.proximo
        anterior = self.fim
        atual = primeiro
        for _ in range(self._tamanho):
            proximo = atual.proximo
            atual.proximo = anterior
            anterior = atual
            atual = proximo
        self.fim = primeiro  # o antigo primeiro vira o último

    def limpar(self):
        self.fim = None
        self._tamanho = 0

    def _no_na_posicao(self, pos):
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        atual = self.fim.proximo
        for _ in range(pos):
            atual = atual.proximo
        return atual

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        if self.vazia():
            return
        # Estilo "faça-enquanto": visita e para quando VOLTAR ao primeiro.
        primeiro = self.fim.proximo
        atual = primeiro
        while True:
            yield atual.valor
            atual = atual.proximo
            if atual is primeiro:
                break

    def __contains__(self, valor):
        return self.buscar(valor) != -1

    def __str__(self):
        if self.vazia():
            return "(vazia)"
        return " -> ".join(str(v) for v in self) + f" -> (volta ao {self.inicio.valor})"


def problema_de_josephus(n, k):
    """n pessoas em roda (1..n); a cada k contadas, a k-ésima sai.

    Devolve (ordem_de_eliminacao, sobrevivente).
    Ex.: n=7, k=3 -> ([3, 6, 2, 7, 5, 1], 4)
    """
    if n < 1 or k < 1:
        raise ValueError("n e k devem ser positivos")
    roda = ListaCircular()
    for pessoa in range(1, n + 1):
        roda.inserir_fim(pessoa)

    eliminados = []
    while len(roda) > 1:
        roda.rotacionar(k - 1)                      # k-1 pessoas passam a vez
        eliminados.append(roda.remover_inicio())    # a k-ésima sai da roda
    return eliminados, roda.obter(0)


if __name__ == "__main__":
    lista = ListaCircular()
    for valor in (10, 20, 30):
        lista.inserir_fim(valor)
    lista.inserir_inicio(5)
    print("Lista:", lista)
    lista.rotacionar(1)
    print("Rotacionada 1x:", lista)
    print("Removido do fim:", lista.remover_fim(), "->", lista)
    lista.inverter()
    print("Invertida:", lista)
    ordem, sobrevivente = problema_de_josephus(7, 3)
    print(f"Josephus(7, 3): eliminados {ordem}, sobrevivente {sobrevivente}")
