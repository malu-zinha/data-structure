# Estruturas de Dados em Python

Todas as estruturas clássicas da disciplina, cada uma em um arquivo, com
comentários explicando o funcionamento, tabela de complexidade, pega-ratões e
um exemplo executável no final do arquivo.

## Como rodar

Sempre a partir desta pasta (`python/`). Cada arquivo tem uma demonstração:

```bash
cd python
python3 -m listas.lista_encadeada
python3 -m arvores.arvore_avl
python3 -m grafos.caminhos_minimos
```

Para rodar os testes automatizados (176 testes, sem instalar nada):

```bash
cd python
python3 -m unittest            # tudo
python3 -m unittest testes.test_arvores -v   # só um arquivo, mostrando os nomes
```

> Use sempre `python3 -m pacote.modulo` (com ponto, sem o `.py`). Alguns
> arquivos importam outros — por exemplo, a fila de prioridade usa o heap.

## O que tem aqui

### `listas/` — todas as listas
| arquivo | estrutura |
|---|---|
| `lista_sequencial.py` | lista estática em vetor (capacidade fixa) |
| `lista_encadeada.py` | lista simplesmente encadeada |
| `lista_duplamente_encadeada.py` | com ponteiro `anterior` e `proximo` |
| `lista_circular.py` | circular simples (+ problema de Josephus) |
| `lista_circular_dupla.py` | circular duplamente encadeada |
| `lista_ordenada.py` | encadeada que se mantém ordenada (+ mesclagem) |
| `lista_heterogenea.py` | nós de tipos diferentes (o `enum` + `union` do C) |

Operações em todas: `inserir_inicio`, `inserir_fim`, `inserir_posicao`,
`remover_inicio`, `remover_fim`, `remover_posicao`, `remover_valor`, `buscar`,
`obter`, `alterar`, `inverter`, `limpar`, `vazia`, `tamanho`.

### `pilhas/` — LIFO
| arquivo | conteúdo |
|---|---|
| `pilha_sequencial.py` | pilha em vetor (com `cheia()`) |
| `pilha_encadeada.py` | pilha dinâmica com nós |
| `aplicacoes_pilha.py` | parênteses balanceados, infixa→pós-fixa, avaliação de expressão, inversão de texto, palíndromo, conversão de base, Torre de Hanói, próximo maior elemento |

### `filas/` — FIFO
| arquivo | conteúdo |
|---|---|
| `fila_sequencial.py` | fila em vetor **circular** |
| `fila_encadeada.py` | fila dinâmica com nós |
| `deque.py` | fila de duas pontas |
| `fila_prioridade.py` | fila de prioridade usando heap |
| `fila_duas_pilhas.py` | exercício clássico: fila feita com duas pilhas |

### `arvores/`
| arquivo | conteúdo |
|---|---|
| `arvore_binaria.py` | árvore binária genérica: percursos (pré, em, pós, nível — recursivos e iterativos), altura, folhas, espelhar, reconstruir a partir dos percursos, classificações |
| `arvore_binaria_busca.py` | ABB: inserir, buscar, remover (3 casos), mínimo, máximo, sucessor, antecessor, k-ésimo menor, intervalo, ancestral comum |
| `arvore_avl.py` | AVL: fator de balanceamento e as 4 rotações (LL, RR, LR, RL) |
| `arvore_rubro_negra.py` | rubro-negra (CLRS): inserção e remoção com recoloração e rotações |
| `heap.py` | heap mínimo, heap máximo e heapsort |

### `grafos/`
| arquivo | conteúdo |
|---|---|
| `grafo_lista.py` | lista de adjacência (dirigido ou não, com peso) |
| `grafo_matriz.py` | matriz de adjacência |
| `buscas.py` | **BFS**, **DFS** (recursiva e iterativa), distâncias, menor caminho sem peso, todos os caminhos, tempos de descoberta/finalização |
| `propriedades.py` | componentes conexas, detecção de ciclo, ordenação topológica (Kahn e DFS), grafo bipartido, componentes fortemente conexas (Kosaraju), pontes e pontos de articulação (Tarjan) |
| `caminhos_minimos.py` | Dijkstra, Bellman-Ford, Floyd-Warshall |
| `arvore_geradora_minima.py` | Kruskal, Prim e Union-Find (conjuntos disjuntos) |

Os algoritmos funcionam com as **duas** representações: eles só usam
`grafo.vertices()` e `grafo.vizinhos(v)`.

## Colinha das complexidades

| estrutura | busca | inserção | remoção | observação |
|---|---|---|---|---|
| Lista sequencial | O(n) | O(n) | O(n) | acesso por posição é O(1) |
| Lista encadeada | O(n) | O(1) no início | O(1) no início | sem acesso direto |
| Lista dupla | O(n) | O(1) nas pontas | O(1) nas pontas | percorre nos 2 sentidos |
| Pilha / Fila / Deque | — | O(1) | O(1) | só mexe nas pontas |
| Heap / Fila de prioridade | O(1) o topo | O(log n) | O(log n) | árvore completa em vetor |
| ABB | O(log n)* | O(log n)* | O(log n)* | *O(n) se ficar torta |
| AVL / Rubro-negra | O(log n) | O(log n) | O(log n) | sempre balanceada |
| Grafo (lista adj.) | O(1) a aresta | O(1) | O(1) | memória O(V + E) |
| Grafo (matriz adj.) | O(1) a aresta | O(1) | O(1) | memória O(V²) |

| algoritmo de grafo | complexidade | para quê |
|---|---|---|
| BFS / DFS | O(V + E) | percorrer, alcançabilidade |
| Menor caminho sem peso (BFS) | O(V + E) | menos arestas |
| Dijkstra | O((V + E) log V) | menor custo, pesos ≥ 0 |
| Bellman-Ford | O(V · E) | aceita peso negativo |
| Floyd-Warshall | O(V³) | todos os pares |
| Kahn / topológica | O(V + E) | ordem de pré-requisitos |
| Kosaraju | O(V + E) | componentes fortemente conexas |
| Kruskal | O(E log E) | AGM (pensa nas arestas) |
| Prim | O(E log V) | AGM (pensa nos vértices) |

## Pega-ratões que mais caem em prova

- **Lista encadeada:** para inserir/remover no meio, pare no nó **anterior**.
- **Lista circular:** não existe `None` no fim — parar no `None` vira loop infinito.
- **Lista dupla:** toda ligação tem dois lados (`anterior` **e** `proximo`).
- **Pilha:** conferir se está vazia antes de desempilhar.
- **Fila encadeada:** ao esvaziar, zerar também o ponteiro `fim`.
- **Fila em vetor:** sem o `% capacidade` o índice estoura; guarde o tamanho para
  diferenciar vazia de cheia.
- **ABB:** valores inseridos já ordenados deixam a árvore torta (vira lista).
- **Árvore:** em ordem numa ABB sai **ordenado** — é o teste rápido para saber
  se a remoção deu certo.
- **AVL:** na remoção pode ser preciso rotacionar em vários níveis.
- **Grafo:** marcar como visitado ao **enfileirar** na BFS, não ao desenfileirar.
- **Dijkstra:** não funciona com peso negativo (use Bellman-Ford).
- **Matriz:** `[[None] * n] * n` cria n cópias da **mesma** linha.
