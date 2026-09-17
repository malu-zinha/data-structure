# data-structure

Repositório para a disciplina de Estrutura de Dados e Algoritmos.

Todas as estruturas clássicas implementadas em **Python**, uma por arquivo, com
comentários explicando o funcionamento, tabela de complexidade, pega-ratões e
um exemplo executável no final de cada arquivo.

```
python/
├── listas/     sequencial, encadeada, duplamente encadeada, circular,
│               circular dupla, ordenada e heterogênea
├── pilhas/     pilha em vetor, pilha encadeada e aplicações
│               (parênteses, infixa→pós-fixa, Torre de Hanói, ...)
├── filas/      fila em vetor circular, fila encadeada, deque,
│               fila de prioridade e fila com duas pilhas
├── arvores/    árvore binária, ABB, AVL, rubro-negra e heap (+ heapsort)
├── grafos/     matriz e lista de adjacência, BFS, DFS, ordenação topológica,
│               componentes conexas, Dijkstra, Bellman-Ford, Floyd-Warshall,
│               Kruskal, Prim, Union-Find
└── testes/     176 testes automatizados
```

## Como usar

```bash
cd python
python3 -m listas.lista_encadeada     # roda a demonstração de um arquivo
python3 -m arvores.arvore_avl
python3 -m grafos.caminhos_minimos
python3 -m unittest                   # roda todos os testes
```

Não precisa instalar nada: só a biblioteca padrão do Python 3.

Os detalhes de cada estrutura, a colinha de complexidades e a lista de
pega-ratões estão em [`python/README.md`](python/README.md).

## Versão em C

As implementações em C da segunda avaliação (listas, pilha e fila com
ponteiros) estão no histórico do Git, no commit `14f41c6`:

```bash
git show 14f41c6 --stat            # ver o que tem lá
git restore --source 14f41c6 SegundaProva   # trazer de volta, se precisar
```
