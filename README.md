# data-structure

Estruturas de dados e algoritmos da disciplina de Estrutura de Dados e Algoritmos, implementados em Python e em C++.

As duas versões seguem a mesma organização: uma pasta por estrutura, com os mesmos nomes de arquivo, de classe e de operação. Dá para estudar uma implementação em uma linguagem e comparar com a mesma coisa na outra.

Cada arquivo começa com um comentário que explica como a estrutura funciona, a tabela de complexidade e os erros mais comuns (pega-ratões). Toda estrutura tem testes automatizados.

## Organização

```
data-structure/
├── python/
│   ├── list/            ─┐
│   ├── linked-list/      │
│   ├── stack/            │
│   ├── queue/            │  mesmas pastas e
│   ├── heap/             │  mesmos arquivos
│   ├── tree/             │  nas duas linguagens
│   ├── graphs/          ─┘
│   └── tests/
└── cpp/
    ├── list/ ... graphs/
    ├── tests/
    ├── util.hpp         funções de exibição usadas nas demonstrações
    └── Makefile
```

Em Python cada estrutura fica em um `.py`; em C++, em um `.hpp`. O nome base é o mesmo: `linked-list/lista_encadeada.py` e `linked-list/lista_encadeada.hpp`.

### `list/`
| arquivo | conteúdo |
|---|---|
| `lista_sequencial` | lista estática em vetor, com capacidade fixa |

### `linked-list/`
| arquivo | conteúdo |
|---|---|
| `lista_encadeada` | lista simplesmente encadeada |
| `lista_duplamente_encadeada` | nós com ponteiro `anterior` e `proximo` |
| `lista_circular` | circular simples, com o problema de Josephus |
| `lista_circular_dupla` | circular duplamente encadeada |
| `lista_ordenada` | encadeada que se mantém ordenada, com mesclagem de duas listas |
| `lista_heterogenea` | nós de tipos diferentes (`enum` + `union`) |

Operações comuns às listas: `inserir_inicio`, `inserir_fim`, `inserir_posicao`, `remover_inicio`, `remover_fim`, `remover_posicao`, `remover_valor`, `buscar`, `obter`, `alterar`, `inverter`, `limpar`, `vazia`, `tamanho`.

### `stack/`
| arquivo | conteúdo |
|---|---|
| `pilha_sequencial` | pilha em vetor, com `cheia()` |
| `pilha_encadeada` | pilha dinâmica com nós |
| `aplicacoes_pilha` | parênteses balanceados, infixa para pós-fixa, avaliação de expressão, inversão de texto, palíndromo, conversão de base, Torre de Hanói e próximo maior elemento |

### `queue/`
| arquivo | conteúdo |
|---|---|
| `fila_sequencial` | fila em vetor circular |
| `fila_encadeada` | fila dinâmica com nós |
| `deque` | fila de duas pontas |
| `fila_prioridade` | fila de prioridade sobre o heap de `heap/` |
| `fila_duas_pilhas` | fila feita com duas pilhas de `stack/` |

### `heap/`
| arquivo | conteúdo |
|---|---|
| `heap` | heap mínimo, heap máximo e heapsort |

### `tree/`
| arquivo | conteúdo |
|---|---|
| `arvore_binaria` | árvore binária genérica: percursos recursivos e iterativos, altura, folhas, espelhar, reconstruir a partir dos percursos, classificações (estritamente binária, completa, cheia) |
| `arvore_binaria_busca` | ABB: inserir, buscar, remover (3 casos), mínimo, máximo, sucessor, antecessor, k-ésimo menor, intervalo, ancestral comum |
| `arvore_avl` | AVL: fator de balanceamento e as 4 rotações (LL, RR, LR, RL) |
| `arvore_rubro_negra` | rubro-negra (CLRS): inserção e remoção com recoloração e rotações |

### `graphs/`
| arquivo | conteúdo |
|---|---|
| `grafo_lista` | lista de adjacência (dirigido ou não, com peso) |
| `grafo_matriz` | matriz de adjacência |
| `buscas` | BFS, DFS (recursiva e iterativa), distâncias, menor caminho sem peso, todos os caminhos, tempos de descoberta e finalização |
| `propriedades` | componentes conexas, detecção de ciclo, ordenação topológica (Kahn e DFS), grafo bipartido, componentes fortemente conexas (Kosaraju), pontes e pontos de articulação (Tarjan) |
| `caminhos_minimos` | Dijkstra, Bellman-Ford, Floyd-Warshall |
| `arvore_geradora_minima` | Kruskal, Prim e Union-Find |

Os algoritmos de grafo funcionam com as duas representações, porque só usam `vertices()` e `vizinhos(v)`.

## Python

Só usa a biblioteca padrão do Python 3; não precisa instalar nada.

Cada arquivo tem uma demonstração no final. Os comandos abaixo são executados dentro de `python/`:

```bash
cd python
python3 linked-list/lista_encadeada.py
python3 tree/arvore_avl.py
python3 graphs/caminhos_minimos.py
```

Testes:

```bash
python3 -m unittest                          # todos
python3 -m unittest tests.test_arvores -v    # um arquivo, mostrando o nome de cada teste
```

Como os nomes das pastas têm hífen (`linked-list`), elas não são pacotes Python: cada arquivo roda como script e importa os vizinhos da mesma pasta direto (`from grafo_lista import GrafoLista`). Os dois arquivos que usam outra pasta (`queue/fila_prioridade.py` usa `heap/` e `queue/fila_duas_pilhas.py` usa `stack/`) acrescentam essa pasta ao `sys.path`. O `tests/__init__.py` faz o mesmo com todas as pastas.

## C++

C++17, sem bibliotecas externas. Cada estrutura é um header (`.hpp`) com templates, e cada pasta tem um `main.cpp` que demonstra as estruturas dela.

Os comandos abaixo são executados dentro de `cpp/`:

```bash
cd cpp
make linked-list    # compila e roda a demonstração de uma pasta
make                # compila todas as demonstrações em build/
make test           # compila e roda os testes
make clean
```

Sem `make`, dá para compilar direto:

```bash
g++ -std=c++17 linked-list/main.cpp -o lista && ./lista
```

Para usar uma estrutura em outro programa, basta incluir o header:

```cpp
#include "linked-list/lista_encadeada.hpp"

ListaEncadeada<int> lista;
lista.inserir_fim(10);
```

Diferenças em relação à versão em Python:

- As estruturas são templates: `ListaEncadeada<int>`, `PilhaEncadeada<std::string>`.
- Os nós são alocados com `new` e liberados com `delete`. O destrutor libera todos os nós, e a cópia das estruturas está desabilitada para que duas variáveis não liberem a mesma memória.
- Os erros são exceções da biblioteca padrão: `std::out_of_range` para estrutura vazia ou posição inválida, `std::overflow_error` para estrutura cheia e `std::invalid_argument` para argumento inválido.
- Onde o Python devolve `None`, o C++ devolve `std::optional` vazio. Um caminho que não existe vem como vetor vazio.
- `para_vetor()` devolve o conteúdo da estrutura em um `std::vector`, usado nas demonstrações e nos testes.

Os testes usam um mini framework próprio (`tests/teste.hpp`) e compilam com AddressSanitizer, que acusa acesso a memória já liberada.

## Complexidades

| estrutura | busca | inserção | remoção | observação |
|---|---|---|---|---|
| Lista sequencial | O(n) | O(n) | O(n) | acesso por posição é O(1) |
| Lista encadeada | O(n) | O(1) no início | O(1) no início | sem acesso direto |
| Lista dupla | O(n) | O(1) nas pontas | O(1) nas pontas | percorre nos 2 sentidos |
| Pilha / Fila / Deque | — | O(1) | O(1) | só mexe nas pontas |
| Heap / Fila de prioridade | O(1) o topo | O(log n) | O(log n) | árvore completa em vetor |
| ABB | O(log n)* | O(log n)* | O(log n)* | *O(n) se ficar torta |
| AVL / Rubro-negra | O(log n) | O(log n) | O(log n) | sempre balanceada |
| Grafo (lista adj.) | O(grau) a aresta | O(grau) | O(grau) | memória O(V + E); no Python, com dicionário, as três são O(1) |
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

## Pega-ratões

- Lista encadeada: para inserir ou remover no meio, pare no nó anterior.
- Lista circular: não existe fim nulo (`None` / `nullptr`); parar nele vira loop infinito.
- Lista dupla: toda ligação tem dois lados (`anterior` e `proximo`).
- Pilha: conferir se está vazia antes de desempilhar.
- Fila encadeada: ao esvaziar, zerar também o ponteiro `fim`.
- Fila em vetor: sem o `% capacidade` o índice estoura; guarde o tamanho para diferenciar vazia de cheia.
- ABB: valores inseridos já ordenados deixam a árvore torta (vira lista).
- Árvore: o percurso em ordem de uma ABB sai ordenado, o que serve para conferir se uma remoção deu certo.
- AVL: na remoção pode ser preciso rotacionar em vários níveis.
- Grafo: na BFS, marcar como visitado ao enfileirar, não ao desenfileirar.
- Dijkstra: não funciona com peso negativo (use Bellman-Ford).
- C++: todo `new` precisa de um `delete`; remover um nó sem liberá-lo vaza memória.
- Python: `[[None] * n] * n` cria n referências para a mesma linha.

## Versão em C

As implementações em C da segunda avaliação (listas, pilha e fila com ponteiros) estão no histórico do Git, no commit `14f41c6`:

```bash
git show 14f41c6 --stat                      # ver o que tem lá
git restore --source 14f41c6 SegundaProva    # trazer de volta, se precisar
```
