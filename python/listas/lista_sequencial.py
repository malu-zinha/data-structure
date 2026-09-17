"""
LISTA SEQUENCIAL ESTÁTICA (baseada em vetor)
============================================

- Os elementos ficam em posições CONTÍGUAS de um vetor com CAPACIDADE fixa,
  definida na criação (igual a um `int dados[MAX]` em C).
- Além do vetor, guardamos QUANTOS elementos estão ocupados (`_tamanho`).
- Acesso por posição é direto: `dados[i]`.
- Para inserir/remover no início ou no meio é preciso DESLOCAR elementos.

Complexidade
------------
| operação                          | custo |
|-----------------------------------|-------|
| obter / alterar (por posição)     | O(1)  |
| inserir / remover no fim          | O(1)  |
| inserir / remover no início/meio  | O(n)  |
| buscar (por valor)                | O(n)  |

Pega-ratão
----------
- Checar se está CHEIA antes de inserir e VAZIA antes de remover.
- Ao inserir, deslocar de TRÁS PARA FRENTE (senão sobrescreve os dados).
- Ao remover, deslocar de FRENTE PARA TRÁS.

Obs.: a `list` do Python já cresce sozinha. Aqui simulamos o vetor fixo do C
com `[None] * capacidade` justamente para estudar o funcionamento.
Posições começam em 0.
"""


class ListaSequencial:
    def __init__(self, capacidade=10):
        if capacidade <= 0:
            raise ValueError("A capacidade deve ser positiva")
        self._dados = [None] * capacidade  # o "vetor" de tamanho fixo
        self._capacidade = capacidade
        self._tamanho = 0                  # quantas posições estão ocupadas

    # ------------------------------------------------------------------
    # Consultas
    # ------------------------------------------------------------------
    def vazia(self):
        return self._tamanho == 0

    def cheia(self):
        return self._tamanho == self._capacidade

    def tamanho(self):
        return self._tamanho

    def capacidade(self):
        return self._capacidade

    def obter(self, pos):
        """Devolve o valor da posição `pos` -> O(1)."""
        self._validar_posicao(pos)
        return self._dados[pos]

    def alterar(self, pos, valor):
        """Troca o valor da posição `pos` -> O(1)."""
        self._validar_posicao(pos)
        self._dados[pos] = valor

    def buscar(self, valor):
        """Busca sequencial: devolve a posição do valor ou -1 se não achar."""
        for i in range(self._tamanho):
            if self._dados[i] == valor:
                return i
        return -1

    # ------------------------------------------------------------------
    # Inserção
    # ------------------------------------------------------------------
    def inserir_inicio(self, valor):
        self.inserir_posicao(0, valor)

    def inserir_fim(self, valor):
        self.inserir_posicao(self._tamanho, valor)

    def inserir_posicao(self, pos, valor):
        """Insere `valor` na posição `pos` (0 = início, tamanho = fim)."""
        if self.cheia():
            raise OverflowError("Lista cheia")
        if pos < 0 or pos > self._tamanho:
            raise IndexError("Posição inválida")

        # Abre espaço em `pos` empurrando todo mundo uma casa para a direita.
        # Começa pelo FIM para não sobrescrever nenhum valor.
        for i in range(self._tamanho, pos, -1):
            self._dados[i] = self._dados[i - 1]

        self._dados[pos] = valor
        self._tamanho += 1

    # ------------------------------------------------------------------
    # Remoção
    # ------------------------------------------------------------------
    def remover_inicio(self):
        return self.remover_posicao(0)

    def remover_fim(self):
        return self.remover_posicao(self._tamanho - 1)

    def remover_posicao(self, pos):
        """Remove e devolve o valor da posição `pos`."""
        if self.vazia():
            raise IndexError("Lista vazia")
        self._validar_posicao(pos)

        valor = self._dados[pos]
        # Fecha o "buraco" puxando todo mundo uma casa para a esquerda.
        for i in range(pos, self._tamanho - 1):
            self._dados[i] = self._dados[i + 1]

        self._tamanho -= 1
        self._dados[self._tamanho] = None  # limpa a última casa, que sobrou
        return valor

    def remover_valor(self, valor):
        """Remove a primeira ocorrência de `valor`. Devolve True se removeu."""
        pos = self.buscar(valor)
        if pos == -1:
            return False
        self.remover_posicao(pos)
        return True

    # ------------------------------------------------------------------
    # Outras operações
    # ------------------------------------------------------------------
    def inverter(self):
        """Inverte a lista no próprio vetor, trocando as pontas -> O(n)."""
        i, j = 0, self._tamanho - 1
        while i < j:
            self._dados[i], self._dados[j] = self._dados[j], self._dados[i]
            i += 1
            j -= 1

    def limpar(self):
        self._dados = [None] * self._capacidade
        self._tamanho = 0

    def _validar_posicao(self, pos):
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        for i in range(self._tamanho):
            yield self._dados[i]

    def __contains__(self, valor):
        return self.buscar(valor) != -1

    def __str__(self):
        return "[" + ", ".join(str(v) for v in self) + "]"


if __name__ == "__main__":
    lista = ListaSequencial(capacidade=5)
    lista.inserir_fim(10)
    lista.inserir_fim(30)
    lista.inserir_inicio(5)
    lista.inserir_posicao(2, 20)
    print("Lista:", lista, "| tamanho:", len(lista), "| capacidade:", lista.capacidade())
    print("Posição do 20:", lista.buscar(20))
    print("Removido do início:", lista.remover_inicio(), "->", lista)
    print("Removido da posição 1:", lista.remover_posicao(1), "->", lista)
    lista.inverter()
    print("Invertida:", lista)
    lista.inserir_fim(1)
    lista.inserir_fim(2)
    lista.inserir_fim(3)
    print("Cheia?", lista.cheia(), "->", lista)
    try:
        lista.inserir_fim(99)
    except OverflowError as erro:
        print("Erro esperado:", erro)
