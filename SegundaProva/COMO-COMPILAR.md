# Guia rápido — Segunda Prova (Estruturas de Dados)

Cada estrutura está na sua pasta com 3 arquivos:
- `*.h`  → declarações (o QUE a estrutura faz)
- `*.c`  → implementação (o COMO funciona)
- `main.c` → programa de teste
- `*.txt` → resumo teórico para revisar

Todas foram implementadas de forma **encadeada** (com ponteiros, não vetor).

---

## Como compilar e rodar (uma de cada vez)

Para compilar, passe o `main.c` **junto** com o `.c` da estrutura.
O `-o` define o nome do programa final.

### Lista Encadeada
```bash
cd ListaEncadeada
gcc main.c lista-encadeada.c -o lista
./lista
```

### Pilha (LIFO)
```bash
cd Pilha
gcc main.c pilha.c -o pilha
./pilha
```

### Fila (FIFO)
```bash
cd Fila
gcc main.c fila.c -o fila
./fila
```

### Lista Duplamente Encadeada
```bash
cd ListaDuplamenteEncadeada
gcc main.c lista-dupla.c -o lista
./lista
```

### Lista Circular
```bash
cd ListaCircular
gcc main.c lista-circular.c -o lista
./lista
```

### Lista Heterogênea
```bash
cd ListaHeterogenea
gcc main.c lista-hetero.c -o lista
./lista
```

---

## Compilar e rodar TODAS de uma vez

A partir da pasta `SegundaProva`, cole este bloco no terminal:

```bash
( cd ListaEncadeada            && gcc main.c lista-encadeada.c -o prog && echo "== Lista Encadeada =="            && ./prog )
( cd Pilha                     && gcc main.c pilha.c           -o prog && echo "== Pilha =="                     && ./prog )
( cd Fila                      && gcc main.c fila.c            -o prog && echo "== Fila =="                      && ./prog )
( cd ListaDuplamenteEncadeada  && gcc main.c lista-dupla.c     -o prog && echo "== Lista Duplamente Encadeada ==" && ./prog )
( cd ListaCircular             && gcc main.c lista-circular.c  -o prog && echo "== Lista Circular =="            && ./prog )
( cd ListaHeterogenea          && gcc main.c lista-hetero.c    -o prog && echo "== Lista Heterogênea =="         && ./prog )
```

> Os parênteses `( ... )` rodam cada bloco num subshell, então o `cd`
> não "gruda" e cada estrutura é compilada na sua própria pasta.

---

## Dica de flag útil

Compile com `-Wall` para o gcc avisar sobre possíveis erros:
```bash
gcc -Wall main.c lista-encadeada.c -o lista
```

## Colinha das estruturas

| Estrutura            | Marca registrada                  | Pega-ratão                         |
|----------------------|-----------------------------------|------------------------------------|
| Lista Encadeada      | nós ligados por `next`, fim = NULL | acesso sequencial (sem `lista[i]`) |
| Pilha                | LIFO, só mexe no topo             | checar vazia antes do pop          |
| Fila                 | FIFO, entra no fim / sai no início | zerar o `fim` quando esvazia       |
| Duplamente Encadeada | tem `prev` **e** `next`           | atualizar os dois ponteiros        |
| Circular             | último aponta pro primeiro        | NÃO parar no NULL → loop infinito  |
| Heterogênea          | `enum` + `union` (tipos variados) | ler campo errado da union = lixo   |
```
