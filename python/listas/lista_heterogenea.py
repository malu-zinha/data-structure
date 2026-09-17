"""
LISTA HETEROGÊNEA
=================

- Cada nó pode guardar um TIPO DIFERENTE de dado (inteiro, real, caractere...).
- Em C isso exige um `enum` (etiqueta do tipo) + `union` (o valor). Em Python
  qualquer variável guarda qualquer tipo, mas mantemos a ETIQUETA (`tipo`)
  em cada nó para imitar o TAD e poder tratar cada tipo de um jeito.

    [INTEIRO: 10] -> [REAL: 3.14] -> [CARACTERE: 'a'] -> [TEXTO: "oi"] -> None

Complexidade: igual à lista encadeada (inserção no fim O(1) com ponteiro `fim`).

Pega-ratão
----------
- Em C, ler o campo errado da union devolve lixo. Por isso sempre olhamos a
  etiqueta `tipo` antes de usar o valor.
- Em Python, `bool` é subclasse de `int` (`isinstance(True, int)` é True)!
  Por isso o `bool` é rejeitado explicitamente.
"""

from enum import Enum


class Tipo(Enum):
    INTEIRO = "inteiro"
    REAL = "real"
    CARACTERE = "caractere"
    TEXTO = "texto"


class No:
    def __init__(self, tipo, valor):
        self.tipo = tipo    # a "etiqueta" (o enum do C)
        self.valor = valor  # o dado (a union do C)
        self.proximo = None


class ListaHeterogenea:
    def __init__(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    def vazia(self):
        return self.inicio is None

    def tamanho(self):
        return self._tamanho

    # ------------------------------------------------------------------
    # Inserção (sempre no fim, para manter a ordem de chegada)
    # ------------------------------------------------------------------
    def _inserir_no(self, tipo, valor):
        novo = No(tipo, valor)
        if self.vazia():
            self.inicio = novo
        else:
            self.fim.proximo = novo
        self.fim = novo
        self._tamanho += 1

    def inserir_inteiro(self, valor):
        if isinstance(valor, bool) or not isinstance(valor, int):
            raise TypeError("Esperava um inteiro")
        self._inserir_no(Tipo.INTEIRO, valor)

    def inserir_real(self, valor):
        if not isinstance(valor, float):
            raise TypeError("Esperava um real (float)")
        self._inserir_no(Tipo.REAL, valor)

    def inserir_caractere(self, valor):
        if not isinstance(valor, str) or len(valor) != 1:
            raise TypeError("Esperava um único caractere")
        self._inserir_no(Tipo.CARACTERE, valor)

    def inserir_texto(self, valor):
        if not isinstance(valor, str):
            raise TypeError("Esperava um texto (str)")
        self._inserir_no(Tipo.TEXTO, valor)

    def inserir(self, valor):
        """Descobre o tipo sozinho e chama a inserção certa."""
        if isinstance(valor, bool):
            raise TypeError("bool não é suportado")
        if isinstance(valor, int):
            self.inserir_inteiro(valor)
        elif isinstance(valor, float):
            self.inserir_real(valor)
        elif isinstance(valor, str) and len(valor) == 1:
            self.inserir_caractere(valor)
        elif isinstance(valor, str):
            self.inserir_texto(valor)
        else:
            raise TypeError(f"Tipo não suportado: {type(valor).__name__}")

    # ------------------------------------------------------------------
    # Remoção e consultas
    # ------------------------------------------------------------------
    def remover_inicio(self):
        """Remove o primeiro nó e devolve a tupla (tipo, valor)."""
        if self.vazia():
            raise IndexError("Lista vazia")
        no = self.inicio
        self.inicio = no.proximo
        if self.inicio is None:
            self.fim = None  # esvaziou: o fim também precisa ser zerado
        self._tamanho -= 1
        return no.tipo, no.valor

    def contar_por_tipo(self):
        """Devolve um dicionário {Tipo: quantidade}."""
        contagem = {tipo: 0 for tipo in Tipo}
        for tipo, _ in self:
            contagem[tipo] += 1
        return contagem

    def filtrar(self, tipo):
        """Devolve só os valores de um determinado tipo, na ordem da lista."""
        return [valor for t, valor in self if t is tipo]

    def somar_numeros(self):
        """Soma inteiros e reais, ignorando caracteres e textos."""
        return sum(valor for tipo, valor in self if tipo in (Tipo.INTEIRO, Tipo.REAL))

    def imprimir(self):
        print(self)

    def limpar(self):
        self.inicio = None
        self.fim = None
        self._tamanho = 0

    def __len__(self):
        return self._tamanho

    def __iter__(self):
        """Percorre devolvendo (tipo, valor)."""
        atual = self.inicio
        while atual is not None:
            yield atual.tipo, atual.valor
            atual = atual.proximo

    def __str__(self):
        if self.vazia():
            return "None"
        partes = [f"[{tipo.value}: {valor!r}]" for tipo, valor in self]
        return " -> ".join(partes) + " -> None"


if __name__ == "__main__":
    lista = ListaHeterogenea()
    lista.inserir_inteiro(10)
    lista.inserir_real(3.14)
    lista.inserir_caractere("a")
    lista.inserir("estrutura")
    lista.inserir(7)
    lista.imprimir()
    print("Contagem:", {tipo.value: qtd for tipo, qtd in lista.contar_por_tipo().items()})
    print("Só inteiros:", lista.filtrar(Tipo.INTEIRO))
    print("Soma dos números:", lista.somar_numeros())
    print("Removido:", lista.remover_inicio(), "->", lista)
    try:
        lista.inserir(True)
    except TypeError as erro:
        print("Erro esperado:", erro)
