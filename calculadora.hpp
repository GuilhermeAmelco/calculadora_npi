#ifndef CALCULADORA_HPP
#define CALCULADORA_HPP

#include <string>

#include "FilaDinamica.hpp"
#include "FilaEstatica.hpp"
#include "PilhaEstatica.hpp"
#include "PilhaDinamica.hpp"
#include "Lue.hpp"

using namespace std;

// ------
// HELPERS -> helpers são funções que auxiliam no desenvolvimento da função, facilitando a leitura do código, separando funcionalidades, etc. Essa funções normalmente são simples
// ------

// Compara o caracter, vê se é operador
bool is_operador(char c)
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
  case '(':
    return true;
  case ')':
    return true;
  default:
    return false;
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
  default:
    return 0;
  }
}

// Remove os espaços em branco da expressão e enfileira cada caractere
// restante, na mesma ordem em que aparecem na string.
template <typename Fila>
bool inserir_expressao_na_fila(Fila &fila, const string &expressao)
{
  int tamanho = expressao.length();

  for (int i = 0; i < tamanho; i++)
  {
    char c = expressao[i];

    if (c == ' ')
      continue;

    if (!queue(fila, c))
      return false; // fila cheia (estática) ou sem memória (dinâmica)
  }
  return true;
}

// Os três parâmetros têm tipos diferentes: a entrada é uma fila de char, a
// pilha guarda operadores (char) e a saída guarda variáveis (string).
template <typename FilaEntrada, typename FilaSaida, typename Pilha, typename Lista>
bool converter_para_notacao_polonesa(FilaEntrada &fila_guarda_expressao, FilaSaida &fila_notacao_polonesa_invertida, Pilha &pilha_operadores, Lista &lista_guarda_variaveis)
{
  char atual;           // os caracteres recebidos
  string variavel = ""; // vai ser o que guarda os nomes das variaveis

  // fazer dequeue até a fila fique vazia
  while (dequeue(fila_guarda_expressao, atual))
  {
    if (is_operador(atual))
    {
      if (atual == ')')
      {
        char operador_do_topo;

        // desempilha os operadores até encontrar o '(' que abriu o grupo
        while (top(pilha_operadores, operador_do_topo) && operador_do_topo != '(')
        {
          if (!pop(pilha_operadores, operador_do_topo))
            return false;
          if (!queue(fila_notacao_polonesa_invertida, string(1, operador_do_topo)))
            return false;
        }
        if (!pop(pilha_operadores, operador_do_topo))
          return false; // ')' sem '(' correspondente
      }
      else if (atual == '(')
      {
        if (!push(pilha_operadores, atual))
          return false;
      }
      else
      {
        char operador_do_topo;

        // desempilha os operadores de prioridade maior ou igual
        while (top(pilha_operadores, operador_do_topo) && operador_do_topo != '(' &&
               prioridade(operador_do_topo) >= prioridade(atual))
        {
          if (!pop(pilha_operadores, operador_do_topo))
            return false;
          if (!queue(fila_notacao_polonesa_invertida, string(1, operador_do_topo)))
            return false;
        }

        if (!push(pilha_operadores, atual))
          return false;
      }
    }
    else
    {
      variavel += atual; // acumula até o proximo ser um operador, se for, ele limpa

      char proximo;
      if (!top(fila_guarda_expressao, proximo) || is_operador(proximo))
      {
        if (!queue(fila_notacao_polonesa_invertida, variavel))
          return false;

        if (!inserirFinal(lista_guarda_variaveis, variavel))
          return false;
        variavel = "";
      }
    }
  }

  // desempilha o que sobrou da pilha de operadores
  char operador_do_topo;
  while (pop(pilha_operadores, operador_do_topo))
  {
    if (operador_do_topo == '(')
      return false; // '(' sem ')' correspondente

    if (!queue(fila_notacao_polonesa_invertida, string(1, operador_do_topo)))
      return false;
  }

  return true;
}

#endif // CALCULADORA_HPP