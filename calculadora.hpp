/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 */

#ifndef CALCULADORA_HPP
#define CALCULADORA_HPP

#include <cmath>
#include <string>

#include "FilaDinamica.hpp"
#include "FilaEstatica.hpp"
#include "PilhaEstatica.hpp"
#include "PilhaDinamica.hpp"
#include "Lue.hpp"

using namespace std;

// Trabalho em dupla: cada variável pode ter até 2 letras. Para o trabalho
// individual, basta trocar para 1.
#define LETRAS_VARIAVEL 2

// ------
// HELPERS -> helpers são funções que auxiliam no desenvolvimento da função, facilitando a leitura do código, separando funcionalidades, etc. Essa funções normalmente são simples
// ------

bool is_operador_binario(char c)
{
  switch (c)
  {
  case '+':
    return true;
  case '-':
    return true;
  case '/':
    return true;
  case '*':
    return true;
  case '^':
    return true;
  default:
    return false;
  }
}

bool is_abre(char c)
{
  return c == '(' || c == '[' || c == '{';
}

bool is_fecha(char c)
{
  return c == ')' || c == ']' || c == '}';
}

// Delimitador de token: operador binário ou abre/fecha de grupo.
bool is_operador(char c)
{
  return is_operador_binario(c) || is_abre(c) || is_fecha(c);
}

bool is_digito(char c)
{
  return c >= '0' && c <= '9';
}

bool is_letra(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Espaço em branco: apenas separa tokens, nunca entra na notação de saída.
bool is_separador(char c)
{
  return c == ' ' || c == '\t';
}

// Fecha que corresponde a cada abre, para validar casamento E ordem.
char abre_correspondente(char fecha)
{
  switch (fecha)
  {
  case ')':
    return '(';
  case ']':
    return '[';
  case '}':
    return '{';
  default:
    return '\0';
  }
}

int prioridade(char c)
{
  switch (c)
  {
  case '+':
    return 1;
  case '-':
    return 1;
  case '/':
    return 2;
  case '*':
    return 2;
  case '^':
    return 3;
  default:
    return 0;
  }
}

// '^' é associativo à direita: 2^3^2 = 2^(3^2) = 512.
bool associativo_direita(char c)
{
  return c == '^';
}

template <typename Fila>
bool inserir_expressao_na_fila(Fila &fila, const string &expressao)
{
  int tamanho = expressao.length();
  bool separador_anterior = false;

  for (int i = 0; i < tamanho; i++)
  {
    char c = expressao[i];

    if (is_separador(c))
    {
      if (separador_anterior)
        continue; // colapsa espaços repetidos

      separador_anterior = true;

      if (!queue(fila, ' '))
        return false; // fila cheia (estática) ou sem memória (dinâmica)
      continue;
    }

    separador_anterior = false;

    if (!queue(fila, c))
      return false; // fila cheia (estática) ou sem memória (dinâmica)
  }
  return true;
}

// Converte a expressão infixa (já enfileirada) para a notação polonesa
// invertida com o algoritmo Shunting-yard. Toda a validação sintática acontece
// aqui: se algo estiver errado, a função retorna false e preenche
// mensagem_erro, encerrando o processo antes de qualquer cálculo.
//
// Os parâmetros têm tipos diferentes: a entrada é uma fila de char, a pilha
// guarda operadores e abre-delimitadores (char) e a saída guarda tokens (string).
template <typename FilaEntrada, typename FilaSaida, typename Pilha, typename Lista>
bool converter_para_notacao_polonesa(FilaEntrada &fila_guarda_expressao, FilaSaida &fila_notacao_polonesa_invertida, Pilha &pilha_operadores, Lista &lista_guarda_variaveis, string &mensagem_erro)
{
  char atual;                      // os caracteres recebidos
  string variavel = "";            // vai ser o que guarda os nomes das variaveis
  bool caracter_eh_numero = false; // o caracter atual começou com dígito, então é uma constante
  bool caracter_tem_ponto = false; // o número atual já usou o separador decimal
  bool espera_operando = true;     // no começo (e após operador/abre) falta um operando
  bool leu_algum_token = false;

  // fazer dequeue até a fila fique vazia
  while (dequeue(fila_guarda_expressao, atual))
  {
    if (is_separador(atual))
      continue; // espaço só separa tokens, não entra na notação

    if (is_abre(atual))
    {
      // "3(4)" seria multiplicação implícita, que não faz parte da gramática
      if (!espera_operando)
      {
        mensagem_erro = "falta um operador antes do abre-delimitador (multiplicacao implicita nao suportada)";
        return false;
      }

      if (!push(pilha_operadores, atual))
      {
        mensagem_erro = "nao foi possivel empilhar o abre-delimitador";
        return false;
      }
      leu_algum_token = true;
      continue;
    }

    if (is_fecha(atual))
    {
      char operador_do_topo;

      // "()" ou "(3+)" chegam aqui com um fechamento sem operando
      if (espera_operando)
      {
        mensagem_erro = string("o fechamento '") + atual + "' nao tem operando";
        return false;
      }

      // desempilha os operadores do grupo até encontrar o abre-delimitador
      while (top(pilha_operadores, operador_do_topo) && !is_abre(operador_do_topo))
      {
        if (!pop(pilha_operadores, operador_do_topo))
        {
          mensagem_erro = "nao foi possivel desempilhar o operador";
          return false;
        }
        if (!queue(fila_notacao_polonesa_invertida, string(1, operador_do_topo)))
          return false;
      }

      // pilha vazia: este fechamento não tem abre-delimitador
      if (!top(pilha_operadores, operador_do_topo))
      {
        mensagem_erro = string("o fechamento '") + atual + "' nao tem abre correspondente";
        return false;
      }

      // achou um abre, mas tem que ser do mesmo tipo e na ordem correta
      if (operador_do_topo != abre_correspondente(atual))
      {
        mensagem_erro = string("fechamento '") + atual + "' fora de ordem: o grupo aberto e '" + operador_do_topo + "'";
        return false;
      }

      if (!pop(pilha_operadores, operador_do_topo))
      {
        mensagem_erro = "nao foi possivel desempilhar o abre-delimitador";
        return false;
      }

      espera_operando = false; // o grupo inteiro vale como um operando
      continue;
    }

    if (is_operador_binario(atual))
    {
      char operador_do_topo;

      // operador no começo da expressão ou logo após outro operador
      if (espera_operando)
      {
        mensagem_erro = string("o operador '") + atual + "' esta sem operando a esquerda";
        return false;
      }

      // desempilha prioridade maior; com prioridade igual, só desempilha se o
      // operador atual for associativo à esquerda (o '^' é à direita)
      while (top(pilha_operadores, operador_do_topo) && !is_abre(operador_do_topo) &&
             (prioridade(operador_do_topo) > prioridade(atual) ||
              (prioridade(operador_do_topo) == prioridade(atual) && !associativo_direita(atual))))
      {
        if (!pop(pilha_operadores, operador_do_topo))
          return false;
        if (!queue(fila_notacao_polonesa_invertida, string(1, operador_do_topo)))
          return false;
      }

      if (!push(pilha_operadores, atual))
      {
        mensagem_erro = "nao foi possivel empilhar o operador";
        return false;
      }

      espera_operando = true;
      leu_algum_token = true;
      continue;
    }

    // Operando: o primeiro caractere define o tipo do token: dígito -> número,
    // letra -> variável. Qualquer outro símbolo é inválido.
    if (variavel.empty())
    {
      // dois operandos seguidos ("3 5", "a b") não têm operador entre eles
      if (!espera_operando)
      {
        mensagem_erro = "dois operandos seguidos sem operador entre eles";
        return false;
      }

      if (is_digito(atual))
        caracter_eh_numero = true;
      else if (is_letra(atual))
        caracter_eh_numero = false;
      else
      {
        mensagem_erro = string("o caractere '") + atual + "' nao pertence a gramatica";
        return false; // ex: '@', '#'
      }
    }
    else if (caracter_eh_numero)
    {
      if (!is_digito(atual))
      {
        if (atual != '.' || caracter_tem_ponto)
        {
          mensagem_erro = "numero malformado (ex: 12a ou 1.2.3)";
          return false;
        }
        caracter_tem_ponto = true;
      }
    }
    else if (!is_letra(atual))
    {
      mensagem_erro = "nome de variavel invalido: use apenas letras";
      return false; // ex: "a1", "a_b"
    }

    variavel += atual; // acumula até o proximo ser um operador, se for, ele limpa

    char proximo;
    if (!top(fila_guarda_expressao, proximo) || is_operador(proximo) || is_separador(proximo))
    {
      if (caracter_eh_numero && variavel.back() == '.')
      {
        mensagem_erro = "numero malformado: \"" + variavel + "\" nao tem parte fracionaria";
        return false; // "12."
      }

      // Trabalho em dupla: o nome da variável tem no máximo LETRAS_VARIAVEL letras
      if (!caracter_eh_numero && (int)variavel.length() > LETRAS_VARIAVEL)
      {
        mensagem_erro = "nome de variavel invalido: \"" + variavel + "\" tem mais de " +
                        to_string(LETRAS_VARIAVEL) + " letras";
        return false;
      }

      if (!queue(fila_notacao_polonesa_invertida, variavel))
        return false;

      // Só nomes de variáveis entram na lista; constantes numéricas não.
      // O nome vai no label do nó; info fica vazio, reservado para o valor da variável.
      // Uma variável repetida (ex: "a+a") já está na lista e não é inserida de novo.
      if (!caracter_eh_numero && !pesquisarLabel(lista_guarda_variaveis, variavel) &&
          !inserirFinal(lista_guarda_variaveis, string(), variavel))
        return false;

      variavel = "";
      caracter_eh_numero = false;
      caracter_tem_ponto = false;
      espera_operando = false;
      leu_algum_token = true;
    }
  }

  if (!leu_algum_token)
  {
    mensagem_erro = "expressao vazia";
    return false;
  }

  // expressão terminando em operador ou com grupo aberto sem operando
  if (espera_operando)
  {
    mensagem_erro = "a expressao termina sem operando (operador ou abre-delimitador sobrando)";
    return false;
  }

  // desempilha o que sobrou da pilha de operadores
  char operador_do_topo;
  while (pop(pilha_operadores, operador_do_topo))
  {
    if (is_abre(operador_do_topo))
    {
      mensagem_erro = string("o abre-delimitador '") + operador_do_topo + "' nao foi fechado";
      return false;
    }

    if (!queue(fila_notacao_polonesa_invertida, string(1, operador_do_topo)))
      return false;
  }

  return true;
}

// Pergunta o valor de cada variável encontrada na expressão. A lista guarda o
// nome da variável no label e reserva o info para o valor, então o valor
// digitado (concatenado caractere por caractere) é gravado direto no info do nó.
// Retorna false se a entrada terminar (EOF) antes de todos os valores.
template <typename T>
bool setar_variaveis(Lue<T> &lista_guarda_variaveis)
{
  // sem variáveis na expressão não há nada para perguntar
  if (Vazia(lista_guarda_variaveis))
    return true;

  NoLue<T> *atual = lista_guarda_variaveis.comeco; // a lista já está inicializada

  while (atual != NULL)
  {
    string valor = "";
    bool tem_ponto = false;

    while (true)
    {
      cout << "Digite o valor da variavel " << atual->label << ": ";

      string digitado;
      if (!getline(cin, digitado))
        return false; // EOF: sem isso o laço ficaria girando para sempre

      // concatena só o que forma um número válido: dígitos e no máximo um '.'
      for (int i = 0; i < (int)digitado.length(); i++)
      {
        char c = digitado[i];

        if (is_digito(c))
          valor += c;
        else if (c == '.' && !tem_ponto && !valor.empty())
        {
          tem_ponto = true;
          valor += c;
        }
        else
        {
          valor = "";
          break;
        }
      }

      // "12." não tem parte fracionária, então também é inválido
      if (!valor.empty() && valor[valor.length() - 1] != '.')
        break;

      cout << "Valor invalido, tente novamente." << endl;
      valor = "";
      tem_ponto = false;
    }

    atual->info = valor;
    atual = atual->elo; // NULL encerra o while
  }

  return true;
}

// Converte o texto de uma constante (ex: "12.5") para double. O texto já é
// validado na conversão para notação polonesa, então aqui é só a conversão.
bool texto_para_double(const string &texto, double &valor)
{
  if (texto.empty() || texto[texto.length() - 1] == '.')
    return false; // "12." não tem parte fracionária

  double parte_inteira = 0.0;
  double parte_fracionaria = 0.0;
  double divisor = 1.0;
  bool tem_ponto = false;

  for (int i = 0; i < (int)texto.length(); i++)
  {
    char c = texto[i];

    if (is_digito(c))
    {
      if (!tem_ponto)
        parte_inteira = parte_inteira * 10 + (c - '0');
      else
      {
        divisor *= 10;
        parte_fracionaria += (c - '0') / divisor;
      }
    }
    else if (c == '.' && !tem_ponto)
      tem_ponto = true;
    else
      return false;
  }

  valor = parte_inteira + parte_fracionaria;
  return true;
}

// Um operando é uma constante (ex: "12") ou um nome de variável. No segundo
// caso, o valor da variável está no info do nó cujo label é o nome.
template <typename T>
bool obter_valor_operando(const string &operando, const Lue<T> &lista_guarda_variaveis, double &valor)
{
  if (!operando.empty() && is_digito(operando[0]))
    return texto_para_double(operando, valor);

  NoLue<T> *aux = lista_guarda_variaveis.comeco;
  while (aux != NULL)
  {
    if (aux->label == operando)
      return texto_para_double(aux->info, valor);
    aux = aux->elo;
  }
  return false; // variável sem valor definido
}

// Calcula a expressão que já está em notação polonesa invertida. A pilha de
// valores guarda os operandos já resolvidos: ao encontrar um operador, os dois
// últimos valores empilhados são retirados, a operação é feita e o resultado
// ocupa o lugar deles. O operando da direita é o último que foi empilhado.
template <typename FilaRPN, typename Pilha, typename T>
bool calcular_expressao(FilaRPN &fila_notacao_polonesa_invertida, Pilha &pilha_valores, const Lue<T> &lista_guarda_variaveis, double &resultado, string &mensagem_erro)
{
  string atual;

  while (dequeue(fila_notacao_polonesa_invertida, atual))
  {
    if (atual.length() != 1 || !is_operador_binario(atual[0]))
    {
      // operando: constante ou variável (nesse caso pega o valor da lista)
      double valor;
      if (!obter_valor_operando(atual, lista_guarda_variaveis, valor))
      {
        mensagem_erro = "a variavel \"" + atual + "\" nao tem valor definido";
        return false;
      }
      if (!push(pilha_valores, valor))
      {
        mensagem_erro = "nao foi possivel empilhar o operando";
        return false;
      }
      continue;
    }

    // operador: pega os dois últimos valores empilhados
    double direita, esquerda;
    if (!pop(pilha_valores, direita) || !pop(pilha_valores, esquerda))
    {
      mensagem_erro = string("o operador '") + atual + "' nao tem dois operandos";
      return false;
    }

    double novo_valor;
    switch (atual[0])
    {
    case '+':
      novo_valor = esquerda + direita;
      break;
    case '-':
      novo_valor = esquerda - direita;
      break;
    case '*':
      novo_valor = esquerda * direita;
      break;
    case '/':
      if (direita == 0)
      {
        mensagem_erro = "divisao por zero";
        return false;
      }
      novo_valor = esquerda / direita;
      break;
    case '^':
      novo_valor = pow(esquerda, direita);
      break;
    default: // abre/fecha delimitador: não deveriam estar na notação polonesa
      mensagem_erro = string("token inesperado na notacao polonesa: \"") + atual + "\"";
      return false;
    }

    // o resultado substitui os dois operandos que foram retirados
    if (!push(pilha_valores, novo_valor))
    {
      mensagem_erro = "nao foi possivel empilhar o resultado parcial";
      return false;
    }
  }

  // no fim a pilha tem que ter exatamente o resultado
  if (!pop(pilha_valores, resultado))
  {
    mensagem_erro = "a notacao polonesa nao produziu resultado";
    return false;
  }

  if (!Vazia(pilha_valores))
  {
    mensagem_erro = "sobraram operandos na pilha de valores";
    return false;
  }

  return true;
}

// ------
// MONTAGEM DA APLICACAO
// As funções recebem as estruturas por referência, então a aplicação funciona
// do mesmo jeito com as versões estáticas ou dinâmicas: trocar a estrutura não
// exige mudar nenhuma linha da lógica.
// ------

// Toda saída da aplicação começa com "- " para se destacar das mensagens do
// usuário; assim fica fácil separar o resultado quando a entrada vem de arquivo.
void imprimir_rotulo(const string &texto)
{
  cout << "- " << texto << endl;
}

template <typename FilaSaida>
void exibir_notacao_polonesa(FilaSaida &fila_notacao_polonesa_invertida)
{
  cout << "Notacao polonesa invertida: ";
  mostrar(fila_notacao_polonesa_invertida);
  cout << endl;
}

// Executa uma resolução completa: entrada, análise, conversão, leitura dos
// valores das variáveis e apresentação do resultado.
// Retorna false apenas quando a entrada de dados termina (EOF), o que encerra
// o programa; erro de sintaxe ou de cálculo apenas encerra esta resolução.
template <typename FilaEntrada, typename FilaSaida, typename PilhaOperadores, typename PilhaValores, typename Lista>
bool executar_calculadora(FilaEntrada &fila_guarda_expressao, FilaSaida &fila_notacao_polonesa_invertida, PilhaOperadores &pilha_operadores, PilhaValores &pilha_valores, Lista &lista_guarda_variaveis)
{
  string expressao;

  imprimir_rotulo("Expressao (operadores + - * / ^, parenteses, colchetes e chaves):");

  if (!getline(cin, expressao))
    return false; // entrada encerrada

  // 1) entrada de dados: a expressão vai para a fila caractere por caractere
  if (!inserir_expressao_na_fila(fila_guarda_expressao, expressao))
  {
    imprimir_rotulo("Erro: a expressao e grande demais para a fila de entrada.");
    return true;
  }

  // 2) análise e conversão: qualquer erro de sintaxe encerra o processo aqui
  string mensagem_erro;
  if (!converter_para_notacao_polonesa(fila_guarda_expressao, fila_notacao_polonesa_invertida, pilha_operadores, lista_guarda_variaveis, mensagem_erro))
  {
    imprimir_rotulo("Erro de sintaxe: " + mensagem_erro + ".");
    return true;
  }

  // a notação polonesa invertida é mostrada antes de pedir os valores
  exibir_notacao_polonesa(fila_notacao_polonesa_invertida);

  // 3) uma única solicitação de valor para cada variável em uso
  if (!setar_variaveis(lista_guarda_variaveis))
    return false; // entrada encerrada antes de informar todos os valores

  // 4) resolução e apresentação do resultado
  double resultado;
  mensagem_erro = "";
  if (!calcular_expressao(fila_notacao_polonesa_invertida, pilha_valores, lista_guarda_variaveis, resultado, mensagem_erro))
  {
    imprimir_rotulo("Erro de calculo: " + mensagem_erro + ".");
    return true;
  }

  cout << "Resultado: " << resultado << endl;
  return true;
}

#endif // CALCULADORA_HPP