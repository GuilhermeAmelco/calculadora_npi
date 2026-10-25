# Calculadora RPN

Calculadora de expressões matemáticas que converte a expressão digitada em
notação **polonesa invertida** (notação pós-fixada / RPN), usando estruturas de
dados implementadas do zero (`Fila`, `Pilha` e `Lue` — lista unidirecional
encadeada).

O programa lê uma expressão infixa (ex.: `(a+b)*c`), converte para pós-fixada
(ex.: `ab+c*`) e deixa a fila de saída pronta para a etapa de avaliação.

## Como compilar e executar

Pré-requisito: `g++` (MinGW/MSYS2 no Windows).

```bash
g++ main.cpp -o main.exe
./main.exe
```

## Estrutura do projeto

| Arquivo             | Conteúdo                                                                                                                         |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| `main.cpp`          | Ponto de entrada: lê a expressão, escolhe as estruturas (estáticas ou dinâmicas) e chama a conversão.                            |
| `calculadora.hpp`   | Regras do domínio: helpers de operador/prioridade, inserção da expressão na fila e conversão para RPN (algoritmo Shunting-yard). |
| `FilaEstatica.hpp`  | Fila circular sobre vetor de tamanho fixo (`TAM`).                                                                               |
| `FilaDinamica.hpp`  | Fila encadeada com ponteiros para o começo e o fim.                                                                              |
| `PilhaEstatica.hpp` | Pilha sobre vetor de tamanho fixo (`TAM`).                                                                                       |
| `PilhaDinamica.hpp` | Pilha encadeada.                                                                                                                 |
| `Lue.hpp`           | Lista unidirecional encadeada (LUE) usada para guardar as variáveis da expressão. Cada nó tem `info`, `label` e `elo`.           |

As estruturas são **header-only** e **template**, então a mesma implementação
serve para `char`, `string` ou qualquer outro tipo. A escolha entre versão
estática e dinâmica é feita na `main`, apenas trocando o tipo declarado.

## Explicação do `calculadora.hpp`

O arquivo é o núcleo lógico do projeto e não declara nenhuma estrutura própria:
ele opera sobre as estruturas recebidas por referência, o que permite usar
`FilaEstatica`/`FilaDinamica`, `PilhaEstatica`/`PilhaDinamica` sem reescrever a
lógica.

### `bool is_operador(char c)`

Helper que responde se um caractere é um operador binário (`+`, `-`, `*`, `/`)
ou um delimitador de grupo (`(`, `)`). É a base da tokenização: os operadores
separam os tokens (números ou nomes de variável).

### `bool is_digito(char c)`, `bool is_inicio_variavel(char c)`, `bool is_caractere_variavel(char c)`

Helpers da regra léxica dos tokens:

- `is_digito` reconhece `0`–`9`;
- `is_inicio_variavel` reconhece letra ou `_` — **um nome de variável nunca
  começa com dígito**, o que impede `12` de ser lido como a variável `12`;
- `is_caractere_variavel` reconhece letra, dígito ou `_` (válidos apenas a partir
  do segundo caractere do nome).

Gramática aceita:

```
numero   := digito+ ('.' digito+)?
variavel := (letra | '_') (letra | digito | '_')*
```

### `int prioridade(char c)`

Define a precedência usada durante a conversão: `+` e `-` valem `1`, `*` e `/`
valem `2`. Qualquer outro caractere (inclusive parênteses) retorna `0`.

### `template <typename Fila> bool inserir_expressao_na_fila(Fila &fila, const string &expressao)`

Percorre a expressão, ignora os espaços em branco e enfileira cada caractere
restante, preservando a ordem original. Retorna `false` se a fila estiver cheia
(estática) ou sem memória (dinâmica), propagando o erro para a `main`.

### `template <...> bool converter_para_notacao_polonesa(...)`

Implementa o **Shunting-yard** (Dijkstra) e recebe quatro estruturas por
referência:

- `fila_guarda_expressao` (`Fila<char>`) — entrada, consumida com `dequeue`;
- `fila_notacao_polonesa_invertida` (`Fila<string>`) — saída em RPN;
- `pilha_operadores` (`Pilha<char>`) — operadores aguardando serem emitidos;
- `lista_guarda_variaveis` (`Lue<string>`) — nomes de variáveis encontrados.

Regras aplicadas no laço principal:

1. **Operando**: os caracteres são acumulados em `variavel` até o próximo
   caractere ser um operador (ou até a fila acabar). O primeiro caractere define
   o tipo do token: dígito → número (aceita um único `.` como separador
   decimal), letra/`_` → nome de variável. Qualquer outro símbolo (`@`, `#`, ...)
   faz a função retornar erro, assim como tokens malformados (`2a`, `12.`,
   `1.2.3`). Ao fechar o token, ele é enfileirado na saída
   (`fila_notacao_polonesa_invertida`); apenas **variáveis** são inseridas em
   `lista_guarda_variaveis` com `inserirFinal(lista, string(), variavel)`: o
   nome da variável vai no **`label`** do nó e o `info` fica vazio, reservado
   para o valor da variável. Antes de inserir, `pesquisarLabel` verifica se o
   nome já está na lista — variável repetida (`a+a`) não gera nó duplicado.
2. **`(`**: é empilhado.
3. **`)`**: desempilha e emite operadores até encontrar o `(` correspondente.
   Se o `(` não existir, retorna erro.
4. **Operador binário**: desempilha e emite todos os operadores de prioridade
   maior ou igual, até achar `(` ou a pilha ficar vazia; depois empilha o
   operador atual.
5. **Final**: desempilha o restante. Se sobrar um `(`, a expressão está
   desbalanceada e a função retorna `false`.

Todas as operações de estrutura (`queue`, `push`, `pop`, `top`) têm retorno
verificado; qualquer falha interrompe a conversão com `false`. O `top` é usado
sobre a fila de entrada como _peek_ (consulta sem remover) para saber se o token
atual terminou.

## Padrões de projeto (convenções do código)

Convenções já aplicadas no repositório:

- **Funções em `snake_case` começando com letra minúscula**: `is_operador`,
  `prioridade`, `inserir_expressao_na_fila`, `converter_para_notacao_polonesa`.
- **Tipos em `PascalCase`**: `FilaEstatica`, `FilaDinamica`, `PilhaEstatica`,
  `PilhaDinamica`, `Lue`, `NoLue`.
- **Sem bibliotecas de estrutura de dados**: nada de `std::stack`, `std::queue`,
  `std::list` ou `std::vector` para as estruturas do domínio. Só as estruturas
  escritas no projeto (`Fila*`, `Pilha*`, `Lue`) são usadas. As únicas inclusões
  da STL são `iostream`, `string` e `new` (para `std::nothrow`).
- **Retorno `bool` para sinalizar erro**: as operações de estrutura
  (`queue`, `dequeue`, `push`, `pop`, `top`, `inserir*`, `retirar*`) e as funções
  de domínio retornam `true`/`false`; o valor de saída vem por parâmetro de
  referência. Exceções: helpers que devolvem um valor computado
  (`is_operador`, `prioridade`, `comprimento`) e `void` para operações que não
  podem falhar (`inicializar`, `liberar`, `mostrar`).
- **`inicializar` explícito**: toda estrutura precisa ser inicializada antes do
  uso (`inicializar(fila)`, `inicializar(pilha)`, `inicializar(lista)`), mesmo
  quando o construtor padrão já zeraria os ponteiros.
- **`liberar` para memória dinâmica**: as versões dinâmicas têm `liberar` que
  percorre e faz `delete` de todos os nós; as estáticas apenas reinicializam o
  índice.
- **Alocação com `new (std::nothrow)`** nas versões dinâmicas, para que falha de
  memória vire retorno `false` em vez de exceção.
- **Estruturas e funções no mesmo header, sem classe**: os dados são `struct`
  "burras" e o comportamento são funções livres template, no estilo de TAD
  procedural (não há métodos nem encapsulamento).
- **Tudo por referência (`&`) na entrada/saída** e `const &` quando a estrutura
  só é lida, evitando cópias desnecessárias.
- **Include guards** em todos os headers (`#ifndef ... #define ... #endif`).
- **Nomes descritivos e em português** para variáveis e funções
  (`fila_guarda_expressao`, `pilha_operadores`, `variavel`), com comentários
  explicando o "porquê" de decisões não óbvias (ex.: por que a pilha de
  operadores é criada na `main`).
