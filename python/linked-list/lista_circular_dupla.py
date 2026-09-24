"""
LISTA CIRCULAR DUPLAMENTE ENCADEADA
===================================

- Junta as duas ideias: cada nó tem `anterior` e `proximo`, e as pontas se
  ligam: `fim.proximo` é o início e `inicio.anterior` é o fim.
- Guardamos só o `inicio`; o fim é `inicio.anterior` (acesso O(1)).

       +--------------------------------+
       v                                |
     [10] <-> [20] <-> [30] <-> [40] <--+   (e 10.anterior = 40)

Complexidade
------------
| operação                          | custo |
|-----------------------------------|-------|
| inserir / remover no início       | O(1)  |
| inserir / remover no fim          | O(1)  |
| inserir / remover em uma posição  | O(n)  |
| buscar                            | O(n)  |

Pega-ratão
----------
- Igual à circular simples: nunca pare no None (ele não existe).
- Com um único nó, `anterior` e `proximo` apontam para ele mesmo.
- Ao remover o `inicio`, o novo início é o próximo dele.
"""


class No:
    def __init__(self, valor, anterior=None, proximo=None):
        self.valor = valor
        self.anterior = anterior
        self.proximo = proximo


class ListaCircularDupla:
    def __init__(self):
        self.inicio = None
        self._tamanho = 0

    @property
    def fim(self):
        return self.inicio.anterior if self.inicio is not None else None

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
    def _inserir_antes(self, referencia, valor):
        """Liga um nó novo entre `referencia.anterior` e `referencia`."""
        novo = No(valor, anterior=referencia.anterior, proximo=referencia)
        referencia.anterior.proximo = novo
        referencia.anterior = novo
        self._tamanho += 1
        return novo

    def _inserir_em_lista_vazia(self, valor):
        novo = No(valor)
        novo.anterior = novo  # sozinho: aponta para si mesmo nos dois sentidos
        novo.proximo = novo
        self.inicio = novo
        self._tamanho = 1

    def inserir_inicio(self, valor):
        if self.vazia():
            self._inserir_em_lista_vazia(valor)
        else:
            self.inicio = self._inserir_antes(self.inicio, valor)

    def inserir_fim(self, valor):
        if self.vazia():
            self._inserir_em_lista_vazia(valor)
        else:
            # "antes do início" é exatamente "depois do fim" numa lista circular
            self._inserir_antes(self.inicio, valor)

    def inserir_posicao(self, pos, valor):
        if pos < 0 or pos > self._tamanho:
            raise IndexError("Posição inválida")
        if pos == 0:
            self.inserir_inicio(valor)
        elif pos == self._tamanho:
            self.inserir_fim(valor)
        else:
            self._inserir_antes(self._no_na_posicao(pos), valor)

    # ------------------------------------------------------------------
    # Remoção
    # ------------------------------------------------------------------
    def _desligar(self, no):
        if self._tamanho == 1:
            self.inicio = None
        else:
            no.anterior.proximo = no.proximo
            no.proximo.anterior = no.anterior
            if no is self.inicio:
                self.inicio = no.proximo
        self._tamanho -= 1
        return no.valor

    def remover_inicio(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self._desligar(self.inicio)

    def remover_fim(self):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self._desligar(self.inicio.anterior)

    def remover_posicao(self, pos):
        if self.vazia():
            raise IndexError("Lista vazia")
        return self._desligar(self._no_na_posicao(pos))

    def remover_valor(self, valor):
        atual = self.inicio
        for _ in range(self._tamanho):
            if atual.valor == valor:
                self._desligar(atual)
                return True
            atual = atual.proximo
        return False

    # ------------------------------------------------------------------
    # Outras operações
    # ------------------------------------------------------------------
    def rotacionar(self, passos=1):
        """Passos positivos giram para frente, negativos para trás."""
        if self.vazia():
            return
        passos %= self._tamanho
        for _ in range(passos):
            self.inicio = self.inicio.proximo

    def inverter(self):
        if self.vazia():
            return
        antigo_fim = self.inicio.anterior
        atual = self.inicio
        for _ in range(self._tamanho):
            atual.anterior, atual.proximo = atual.proximo, atual.anterior
            atual = atual.anterior  # o "próximo" antigo
        self.inicio = antigo_fim

    def limpar(self):
        self.inicio = None
        self._tamanho = 0

    def _no_na_posicao(self, pos):
        if pos < 0 or pos >= self._tamanho:
            raise IndexError("Posição inválida")
        if pos <= self._tamanho // 2:
            atual = self.inicio
            for _ in range(pos):
                atual = atual.proximo
        else:  # mais perto do fim: anda para trás
            atual = self.inicio.anterior
            for _ in range(self._tamanho - 1 - pos):
                atual = atual.anterior
        return atual

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        atual = self.inicio
        for _ in range(self._tamanho):
            yield atual.valor
            atual = atual.proximo

    def __reversed__(self):
        if self.vazia():
            return
        atual = self.inicio.anterior
        for _ in range(self._tamanho):
            yield atual.valor
            atual = atual.anterior

    def __contains__(self, valor):
        return self.buscar(valor) != -1

    def __str__(self):
        if self.vazia():
            return "(vazia)"
        return " <-> ".join(str(v) for v in self) + f" <-> (volta ao {self.inicio.valor})"


if __name__ == "__main__":
    lista = ListaCircularDupla()
    for valor in (10, 20, 30, 40):
        lista.inserir_fim(valor)
    print("Lista:", lista)
    print("Fim:", lista.fim.valor, "| anterior do início:", lista.inicio.anterior.valor)
    print("De trás para frente:", list(reversed(lista)))
    lista.rotacionar(-1)
    print("Rotacionada para trás:", lista)
    print("Removido do fim:", lista.remover_fim(), "->", lista)
    lista.inverter()
    print("Invertida:", lista)
