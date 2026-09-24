"""
LISTA DUPLAMENTE ENCADEADA
==========================

- Cada nó tem DUAS referências: `anterior` e `proximo`.
- A lista guarda o primeiro (`inicio`) e o último (`fim`) nó.
- Dá para percorrer nos dois sentidos e remover um nó conhecido em O(1),
  porque ele já sabe quem é o seu anterior.

    None <- [10] <-> [20] <-> [30] -> None
            inicio            fim

Complexidade
------------
| operação                          | custo |
|-----------------------------------|-------|
| inserir / remover no início       | O(1)  |
| inserir / remover no fim          | O(1)  |  (graças ao ponteiro `fim`)
| inserir / remover em uma posição  | O(n)  |  (anda pelo lado mais perto)
| buscar                            | O(n)  |

Pega-ratão
----------
- Toda ligação tem DOIS lados: se mexeu no `proximo` de um nó, lembre do
  `anterior` do outro.
- Casos especiais: lista vazia, remover o primeiro (atualiza `inicio`) e
  remover o último (atualiza `fim`).
- Gasta mais memória que a simplesmente encadeada (um ponteiro extra por nó).
"""


class No:
    def __init__(self, valor, anterior=None, proximo=None):
        self.valor = valor
        self.anterior = anterior
        self.proximo = proximo


class ListaDuplamenteEncadeada:
    def __init__(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    # ------------------------------------------------------------------
    # Consultas
    # ------------------------------------------------------------------
    def vazia(self):
        return self.inicio is None

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
        novo = No(valor, proximo=self.inicio)
        if self.vazia():
            self.fim = novo              # único nó: é início e fim ao mesmo tempo
        else:
            self.inicio.anterior = novo  # antigo primeiro aponta de volta pro novo
        self.inicio = novo
        self._tamanho += 1

    def inserir_fim(self, valor):
        novo = No(valor, anterior=self.fim)
        if self.vazia():
            self.inicio = novo
        else:
            self.fim.proximo = novo
        self.fim = novo
        self._tamanho += 1

    def inserir_posicao(self, pos, valor):
        """Insere na posição `pos` (0 = início, tamanho = fim)."""
        if pos < 0 or pos > self._tamanho:
            raise IndexError("Posição inválida")
        if pos == 0:
            self.inserir_inicio(valor)
        elif pos == self._tamanho:
            self.inserir_fim(valor)
        else:
            atual = self._no_na_posicao(pos)  # o novo entra ANTES deste nó
            novo = No(valor, anterior=atual.anterior, proximo=atual)
            atual.anterior.proximo = novo
            atual.anterior = novo
            self._tamanho += 1

    # ------------------------------------------------------------------
    # Remoção
    # ------------------------------------------------------------------
    def remover_inicio(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self._desligar(self.inicio)

    def remover_fim(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self._desligar(self.fim)

    def remover_posicao(self, pos):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self._desligar(self._no_na_posicao(pos))

    def remover_valor(self, valor):
        atual = self.inicio
        while atual is not None:
            if atual.valor == valor:
                self._desligar(atual)
                return True
            atual = atual.proximo
        return False

    def _desligar(self, no):
        """Tira `no` da lista em O(1): o anterior e o próximo se ligam direto."""
        if no.anterior is None:          # era o primeiro
            self.inicio = no.proximo
        else:
            no.anterior.proximo = no.proximo

        if no.proximo is None:           # era o último
            self.fim = no.anterior
        else:
            no.proximo.anterior = no.anterior

        self._tamanho -= 1
        return no.valor

    # ------------------------------------------------------------------
    # Outras operações
    # ------------------------------------------------------------------
    def inverter(self):
        """Troca `anterior` e `proximo` de cada nó e depois troca início/fim."""
        atual = self.inicio
        while atual is not None:
            atual.anterior, atual.proximo = atual.proximo, atual.anterior
            atual = atual.anterior  # depois da troca, o "próximo" antigo está em `anterior`
        self.inicio, self.fim = self.fim, self.inicio

    def limpar(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    def _no_na_posicao(self, pos):
        """Anda a partir da ponta mais próxima da posição (no máximo n/2 passos)."""
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        if pos < self._tamanho // 2:
            atual = self.inicio
            for _ in range(pos):
                atual = atual.proximo
        else:
            atual = self.fim
            for _ in range(self._tamanho - 1 - pos):
                atual = atual.anterior
        return atual

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        atual = self.inicio
        while atual is not None:
            yield atual.valor
            atual = atual.proximo

    def __reversed__(self):
        """Percorre do fim para o início usando os ponteiros `anterior`."""
        atual = self.fim
        while atual is not None:
            yield atual.valor
            atual = atual.anterior

    def __contains__(self, valor):
        return self.buscar(valor) != -1

    def __str__(self):
        if self.vazia():
            return "None"
        return "None <- " + " <-> ".join(str(v) for v in self) + " -> None"


if __name__ == "__main__":
    lista = ListaDuplamenteEncadeada()
    for valor in (10, 20, 30):
        lista.inserir_fim(valor)
    lista.inserir_inicio(5)
    lista.inserir_posicao(2, 15)
    print("Lista:", lista)
    print("De trás para frente:", list(reversed(lista)))
    print("Removido do fim:", lista.remover_fim(), "->", lista)
    print("Removido da posição 1:", lista.remover_posicao(1), "->", lista)
    lista.inverter()
    print("Invertida:", lista)
