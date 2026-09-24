"""Coloca as pastas das estruturas no sys.path.

Os nomes das pastas têm hífen (`linked-list`), então não dá para importá-las
como pacotes. Em vez disso, cada pasta entra no caminho de busca e os testes
importam os módulos direto: `from lista_encadeada import ListaEncadeada`.
"""

import sys
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent

for pasta in ("list", "linked-list", "stack", "queue", "heap", "tree", "graphs"):
    sys.path.insert(0, str(RAIZ / pasta))
