"""
FILA DE PRIORIDADE
==================

- Cada elemento entra com uma PRIORIDADE. Sai sempre o de maior prioridade,
  não importa a ordem de chegada. Aqui: MENOR número = MAIOR prioridade
  (como no pronto-socorro: prioridade 1 é atendida antes da 5).
- Empate na prioridade: sai quem chegou primeiro (usamos um contador de chegada).
- Implementada com HEAP MÍNIMO (veja `heap/heap.py`).

Comparação de implementações
----------------------------
| implementação       | inserir  | remover o prioritário |
|---------------------|----------|-----------------------|
| lista desordenada   | O(1)     | O(n)                  |
| lista ordenada      | O(n)     | O(1)                  |
| heap binário        | O(log n) | O(log n)              |  <- a usada aqui

Usos: Dijkstra, Prim, escalonamento de processos, simulações de eventos.

Rode a partir da pasta `python/`:  python3 queue/fila_prioridade.py
"""

import sys
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parent.parent / "heap"))

from heap import HeapMinimo  # noqa: E402


class FilaPrioridade:
    def __init__(self):
        self._heap = HeapMinimo()
        self._chegada = 0  # desempate: quem chegou antes sai antes

    def vazia(self):
        return self._heap.vazio()

    def tamanho(self):
        return len(self._heap)

    def enfileirar(self, valor, prioridade):
        # A tupla é comparada campo a campo: primeiro a prioridade, depois a
        # ordem de chegada. Como a chegada nunca se repete, o valor em si nunca
        # precisa ser comparado.
        self._heap.inserir((prioridade, self._chegada, valor))
        self._chegada += 1

    def desenfileirar(self):
        """Remove e devolve o valor mais prioritário."""
        if self.vazia():
            raise IndexError("Fila de prioridade vazia")
        _, _, valor = self._heap.remover()
        return valor

    def frente(self):
        """Devolve (valor, prioridade) do próximo a sair, sem remover."""
        if self.vazia():
            raise IndexError("Fila de prioridade vazia")
        prioridade, _, valor = self._heap.topo()
        return valor, prioridade

    def __len__(self):
        return self.tamanho()

    def __str__(self):
        ordem = sorted(self._heap)  # só para exibir; não altera o heap
        return "[" + ", ".join(f"{valor}(p={prioridade})" for prioridade, _, valor in ordem) + "]"


if __name__ == "__main__":
    pronto_socorro = FilaPrioridade()
    pronto_socorro.enfileirar("corte leve", 4)
    pronto_socorro.enfileirar("parada cardíaca", 1)
    pronto_socorro.enfileirar("febre", 3)
    pronto_socorro.enfileirar("fratura", 2)
    pronto_socorro.enfileirar("dor de cabeça", 3)
    print("Fila:", pronto_socorro)
    print("Próximo:", pronto_socorro.frente())
    while not pronto_socorro.vazia():
        print("Atendendo:", pronto_socorro.desenfileirar())
