# Calculadora RPN

Calculadora de expressões matemáticas que converte a expressão digitada em
notação **polonesa invertida** (notação pós-fixada / RPN), usando estruturas de
dados implementadas do zero (`Fila`, `Pilha` e `Lue` — lista unidirecional
encadeada).

O programa lê uma expressão infixa (ex.: `3 + ((5 * a) - (b / (3+c)))`),
mostra a expressão convertida para pós-fixada (`3 5 a * b 3 c + / - +`), pede o
valor de cada variável e apresenta o resultado final (`26`). Erros sintáticos
(operador sem operando, delimitadores desbalanceados ou fora de ordem, nomes de
variável inválidos, etc.) são detectados na etapa de análise e encerram a
resolução antes de qualquer cálculo.

## Como compilar e executar

Pré-requisito: `g++` (MinGW/MSYS2 no Windows).

```bash
g++ -std=c++17 -Wall -Wextra -o main.exe main.cpp
./main.exe
```

Por padrão a aplicação usa as estruturas **dinâmicas**. Para rodar com as
estáticas basta compilar definindo `ESTRUTURA_ESTATICA` (ou trocar o `#define`
no topo do `main.cpp`):

```bash
g++ -std=c++17 -Wall -Wextra -DESTRUTURA_ESTATICA=1 -o main_estatico.exe main.cpp
```

As duas versões produzem exatamente a mesma saída.

## Estrutura do projeto

| Arquivo             | Conteúdo                                                                                                                         |
| ------------------- | -------------------------------------------------------------------------------------------------------------------------------- |
| `main.cpp`          | Ponto de entrada: escolhe as estruturas (estáticas ou dinâmicas), inicializa, usa e libera.                                       |
| `calculadora.hpp`   | Regras do domínio: helpers de operador/prioridade, validação e conversão para RPN (Shunting-yard), leitura das variáveis e cálculo. |
| `FilaEstatica.hpp`  | Fila circular sobre vetor de tamanho fixo (`TAM`).                                                                               |
| `FilaDinamica.hpp`  | Fila encadeada com ponteiros para o começo e o fim.                                                                              |
| `PilhaEstatica.hpp` | Pilha sobre vetor de tamanho fixo (`TAM`).                                                                                       |
| `PilhaDinamica.hpp` | Pilha encadeada.                                                                                                                 |
| `Lue.hpp`           | Lista unidirecional encadeada (LUE) usada para guardar as variáveis da expressão. Cada nó tem `info`, `label` e `elo`.           |

As estruturas são **header-only** e **template**, então a mesma implementação
serve para `char`, `string` ou qualquer outro tipo. A escolha entre versão
estática e dinâmica é feita na `main` (`#define ESTRUTURA_ESTATICA` ou
`-DESTRUTURA_ESTATICA=1`), apenas trocando o tipo declarado — nenhuma linha da
lógica muda. As quatro estruturas expõem a mesma interface
(`inicializar`, `Vazia`, `Cheia`, `push`/`queue`, `pop`/`dequeue`, `top`,
`mostrar`, `liberar`).

## Explicação do `calculadora.hpp`

O arquivo é o núcleo lógico do projeto e não declara nenhuma estrutura própria:
ele opera sobre as estruturas recebidas por referência, o que permite usar
`FilaEstatica`/`FilaDinamica`, `PilhaEstatica`/`PilhaDinamica` sem reescrever a
lógica.

### `bool is_operador_binario(char c)`, `bool is_abre(char c)`, `bool is_fecha(char c)`

Helpers que classificam os símbolos da gramática:

- `is_operador_binario` reconhece `+`, `-`, `*`, `/` e `^`;
- `is_abre` reconhece `(`, `[` e `{`;
- `is_fecha` reconhece `)`, `]` e `}`.

### `bool is_operador(char c)`

Helper que responde se um caractere é um operador binário **ou** um
abre/fecha delimitador. É a base da tokenização: os operadores separam os tokens
(números ou nomes de variável).

### `char abre_correspondente(char fecha)`

Devolve o abre-delimitador que casa com um fecha (`)` → `(`, `]` → `[`,
`}` → `{`). É o que permite validar **casamento e ordem**: `(3+4]` é recusado
porque o grupo aberto no topo da pilha é `(`.

### `bool is_digito(char c)`, `bool is_letra(char c)`, `bool is_separador(char c)`

Helpers da regra léxica dos tokens:

- `is_digito` reconhece `0`–`9`;
- `is_letra` reconhece `a`–`z` e `A`–`Z`;
- `is_separador` reconhece espaço e tabulação — espaço **só separa tokens**,
  nunca entra na notação de saída (`3 5` continua sendo dois tokens).

Gramática aceita (trabalho em dupla):

```
expressao := operando (operador operando)*
operando  := numero | variavel | abre expressao fecha
numero    := digito+ ('.' digito+)?
variavel  := letra{1,2}          // LETRAS_VARIAVEL = 2; usar 1 no trabalho individual
abre      := '(' | '[' | '{'
fecha     := ')' | ']' | '}'     // precisa casar com o abre do topo da pilha
operador  := '+' | '-' | '*' | '/' | '^'
```

### `int prioridade(char c)` e `bool associativo_direita(char c)`

Definem a precedência usada durante a conversão: `+` e `-` valem `1`, `*` e `/`
valem `2`, `^` vale `3`. Qualquer outro caractere (inclusive delimitadores)
retorna `0`. `^` é o único operador **associativo à direita**
(`2^3^2` = `2^(3^2)` = `512`), o que é tratado no laço de desempilhamento.

### `template <typename Fila> bool inserir_expressao_na_fila(Fila &fila, const string &expressao)`

Percorre a expressão e enfileira cada caractere, preservando a ordem original.
Espaços em branco são colapsados em **um único** `' '`: sem esse separador
`3 5` viraria o número `35` e o erro passaria despercebido. Retorna `false` se a
fila estiver cheia (estática) ou sem memória (dinâmica), propagando o erro para
a `main`.

### `template <...> bool converter_para_notacao_polonesa(...)`

Implementa o **Shunting-yard** (Dijkstra) e recebe as estruturas por referência,
mais uma `string &mensagem_erro` que recebe a causa quando algo está errado:

- `fila_guarda_expressao` (`Fila<char>`) — entrada, consumida com `dequeue`;
- `fila_notacao_polonesa_invertida` (`Fila<string>`) — saída em RPN;
- `pilha_operadores` (`Pilha<char>`) — operadores e abre-delimitadores;
- `lista_guarda_variaveis` (`Lue<string>`) — nomes de variáveis encontrados.

A função também concentra a **análise sintática**: mantém a flag
`espera_operando` (verdadeira no início, depois de um operador e depois de um
abre-delimitador) para garantir a alternância operando/operador. Regras
aplicadas no laço principal:

1. **Espaço**: ignorado — ele apenas separa tokens.
2. **Abre-delimitador** (`(`, `[`, `{`): se não se espera um operando,
   é multiplicação implícita (`3(4)`) e a função retorna erro; senão é
   empilhado.
3. **Fecha-delimitador** (`)`, `]`, `}`): desempilha e emite os operadores do
   grupo até encontrar um abre-delimitador. Se a pilha esvaziar, o fecha está
   sobrando; se o abre encontrado não for o `abre_correspondente` do fecha, o
   grupo está **fora de ordem** (`(3+4]`). Por fim desempilha o abre e passa a
   esperar um operador.
4. **Operador binário**: se ainda se esperava um operando, o operador está sem
   operando à esquerda (`*3+4`, `3++4`). Caso contrário, desempilha e emite os
   operadores de prioridade **maior** e, quando a prioridade é **igual**, só
   desempilha se o operador atual for associativo à esquerda — por isso `^`
   fica associativo à direita. Depois empilha o operador atual.
5. **Operando**: se ainda se esperava um operador, há dois operandos seguidos
   (`3 5`, `a b`). Os caracteres são acumulados em `variavel` até o próximo ser
   operador, separador ou até a fila acabar. O primeiro caractere define o tipo
   do token: dígito → número (aceita um único `.` como separador decimal),
   letra → nome de variável (`LETRAS_VARIAVEL`, 2 no trabalho em dupla).
   Qualquer outro símbolo (`@`, `#`, `_`) ou token malformado (`2a`, `a1`,
   `12.`, `1.2.3`, `abc`) faz a função retornar erro. Ao fechar o token, ele é
   enfileirado na saída; apenas **variáveis** são inseridas em
   `lista_guarda_variaveis` com `inserirFinal(lista, string(), variavel)`: o
   nome vai no **`label`** do nó e o `info` fica vazio, reservado para o valor.
   Antes de inserir, `pesquisarLabel` verifica se o nome já está na lista —
   variável repetida (`a+a`) não gera nó duplicado.
6. **Final**: expressão sem nenhum token é vazia (erro); se ainda se esperava um
   operando, a expressão termina em operador ou com grupo aberto sem conteúdo
   (erro); depois desempilha o restante e, se sobrar um abre-delimitador, o
   grupo não foi fechado (erro).

Todas as operações de estrutura (`queue`, `push`, `pop`, `top`) têm retorno
verificado; qualquer falha interrompe a conversão com `false`. O `top` é usado
sobre a fila de entrada como _peek_ (consulta sem remover) para saber se o token
atual terminou.

### `template <typename T> bool setar_variaveis(Lue<T> &lista_guarda_variaveis)`

Percorre a lista de variáveis (uma vez só, na ordem em que aparecem na
expressão) e pede o valor de cada uma. O texto digitado é validado caractere por
caractere — só dígitos e no máximo um `.` — e gravado no `info` do nó. Retorna
`false` se a entrada terminar (`EOF`) antes de todos os valores, evitando o laço
infinito que ocorreria lendo de arquivo ou redirecionamento.

### `bool texto_para_double(const string &texto, double &valor)`

Converte o texto de um operando para `double` sem `stod`/`atof`: soma as
posições da parte inteira e vai dividindo a parte fracionária por potências de
10. É usada tanto para constantes quanto para os valores das variáveis.

### `template <typename T> bool obter_valor_operando(...)`

Resolve um token da RPN: se começa com dígito é uma constante (vai direto para
`texto_para_double`); caso contrário procura o nome na lista de variáveis e
converte o `info` do nó.

### `template <...> bool calcular_expressao(...)`

Avalia a RPN com uma pilha de valores: operandos são empilhados; ao encontrar um
operador, dois valores são desempilhados (o da direita é o último empilhado), a
operação é feita e o resultado volta para a pilha. Trata divisão por zero
(mensagem própria) e usa `pow` para `^`. No fim a pilha precisa conter
exatamente um valor — o resultado. Qualquer inconsistência preenche
`mensagem_erro` e retorna `false`.

### `void imprimir_rotulo(const string &)`, `exibir_notacao_polonesa(...)` e `executar_calculadora(...)`

Montagem da aplicação, sem conhecer o tipo concreto das estruturas:

- `imprimir_rotulo` padroniza as mensagens da aplicação com o prefixo `- `;
- `exibir_notacao_polonesa` mostra a fila de saída com `mostrar` (percurso que
  **não** consome a fila, que ainda será usada no cálculo);
- `executar_calculadora` executa uma resolução completa na ordem do enunciado:
  lê a expressão → preenche a fila → analisa e converte (erro encerra aqui) →
  **mostra a RPN** → pede os valores das variáveis (uma vez cada) → calcula e
  apresenta o resultado.


## Exemplo de execução

```
Estruturas em uso: DINAMICAS

- Expressao (operadores + - * / ^, parenteses, colchetes e chaves):
3 + ((5 * a) - (b / (3+c)))
Notacao polonesa invertida: 3 5 a * b 3 c + / - +
Digite o valor da variavel a: 5
Digite o valor da variavel b: 8
Digite o valor da variavel c: 1
Resultado: 26
```

Casos de erro detectados na análise (o processo de resolução termina e nenhum
valor é pedido):

| Entrada        | Mensagem                                                        |
| -------------- | --------------------------------------------------------------- |
| `3++4`         | o operador '+' esta sem operando a esquerda                     |
| `*3+4`         | o operador '*' esta sem operando a esquerda                     |
| `3+`           | a expressao termina sem operando                                |
| `3 5`          | dois operandos seguidos sem operador entre eles                 |
| `(3+4`         | o abre-delimitador '(' nao foi fechado                          |
| `3+4)`         | o fechamento ')' nao tem abre correspondente                    |
| `(3+4]`        | fechamento ']' fora de ordem: o grupo aberto e '('              |
| `3(4)`         | falta um operador antes do abre-delimitador                     |
| `()`, `{[]}`   | o fechamento ')' nao tem operando                               |
| `abc+1`        | nome de variavel invalido: "abc" tem mais de 2 letras           |
| `3/0`          | (erro de cálculo) divisao por zero                              |

## Padrões de projeto (convenções do código)

Convenções já aplicadas no repositório:

- **Funções em `snake_case` começando com letra minúscula**: `is_operador`,
  `prioridade`, `inserir_expressao_na_fila`, `converter_para_notacao_polonesa`.
- **Tipos em `PascalCase`**: `FilaEstatica`, `FilaDinamica`, `PilhaEstatica`,
  `PilhaDinamica`, `Lue`, `NoLue`.
- **Sem bibliotecas de estrutura de dados**: nada de `std::stack`, `std::queue`,
  `std::list` ou `std::vector` para as estruturas do domínio. Só as estruturas
  escritas no projeto (`Fila*`, `Pilha*`, `Lue`) são usadas. As únicas inclusões
  da STL são `iostream`, `string`, `new` (para `std::nothrow`) e `cmath` (só
  para `pow`, no operador `^`).
- **Retorno `bool` para sinalizar erro**: as operações de estrutura
  (`queue`, `dequeue`, `push`, `pop`, `top`, `inserir*`, `retirar*`) e as funções
  de domínio retornam `true`/`false`; o valor de saída vem por parâmetro de
  referência. Exceções: helpers que devolvem um valor computado
  (`is_digito`, `is_letra`, `is_operador`, `prioridade`, `comprimento`) e `void`
  para operações que não podem falhar (`inicializar`, `liberar`, `mostrar`).
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
