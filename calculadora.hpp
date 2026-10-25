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

bool is_digito(char c)
{
  return c >= '0' && c <= '9';
}

bool is_letra(char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Um nome de variável precisa começar com letra ou '_' (nunca com dígito,
// senão "12" seria confundido com uma variável chamada "12").
bool is_inicio_variavel(char c)
{
  return is_letra(c) || c == '_';
}

// Depois do primeiro caractere, um nome de variável aceita letras, dígitos e '_'.
bool is_caractere_variavel(char c)
{
  return is_letra(c) || is_digito(c) || c == '_';
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
  char atual;                      // os caracteres recebidos
  string variavel = "";            // vai ser o que guarda os nomes das variaveis
  bool caracter_eh_numero = false; // o caracter atual começou com dígito, então é uma constante
  bool caracter_tem_ponto = false; // o número atual já usou o separador decimal

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
      // O primeiro caractere define o tipo do caracter: dígito -> número,
      // letra/'_' -> variável. Qualquer outro símbolo é inválido.
      if (variavel.empty())
      {
        if (is_digito(atual))
          caracter_eh_numero = true;
        else if (is_inicio_variavel(atual))
          caracter_eh_numero = false;
        else
          return false; // caractere que não pertence à gramática (ex: '@', '#')
      }
      else if (caracter_eh_numero)
      {
        if (!is_digito(atual))
        {
          if (atual != '.' || caracter_tem_ponto)
            return false; // "12a" ou "1.2.3"
          caracter_tem_ponto = true;
        }
      }
      else if (!is_caractere_variavel(atual))
      {
        return false; // nome de variável com símbolo inválido (ex: "a@b")
      }

      variavel += atual; // acumula até o proximo ser um operador, se for, ele limpa

      char proximo;
      if (!top(fila_guarda_expressao, proximo) || is_operador(proximo))
      {
        if (caracter_eh_numero && variavel.back() == '.')
          return false; // "12." não tem parte fracionária

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

// Pergunta o valor de cada variável encontrada na expressão. A lista guarda o
// nome da variável no label e reserva o info para o valor, então o valor
// digitado (concatenado caractere por caractere) é gravado direto no info do nó.
template <typename T>
void setar_variaveis(Lue<T> &lista_guarda_variaveis)
{
  // sem variáveis na expressão não há nada para perguntar
  if (Vazia(lista_guarda_variaveis))
    return;

  NoLue<T> *atual = lista_guarda_variaveis.comeco; // a lista já está inicializada

  while (atual != NULL)
  {
    string valor = "";
    bool tem_ponto = false;

    while (true)
    {
      cout << "Digite o valor da variavel " << atual->label << ": ";

      string digitado;
      getline(cin, digitado);

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
bool calcular_expressao(FilaRPN &fila_notacao_polonesa_invertida, Pilha &pilha_valores, const Lue<T> &lista_guarda_variaveis, double &resultado)
{
  string atual;

  while (dequeue(fila_notacao_polonesa_invertida, atual))
  {
    if (atual.length() != 1 || !is_operador(atual[0]))
    {
      // operando: constante ou variável (nesse caso pega o valor da lista)
      double valor;
      if (!obter_valor_operando(atual, lista_guarda_variaveis, valor))
        return false;
      if (!push(pilha_valores, valor))
        return false;
      continue;
    }

    // operador: pega os dois últimos valores empilhados
    double direita, esquerda;
    if (!pop(pilha_valores, direita) || !pop(pilha_valores, esquerda))
      return false; // operador sem dois operandos anteriores

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
        cout << "Erro: divisao por zero." << endl;
        return false;
      }
      novo_valor = esquerda / direita;
      break;
    default: // '(' ou ')': não deveriam estar na notação polonesa
      return false;
    }

    // o resultado substitui os dois operandos que foram retirados
    if (!push(pilha_valores, novo_valor))
      return false;
  }

  // no fim a pilha tem que ter exatamente o resultado
  if (!pop(pilha_valores, resultado))
    return false;
  return Vazia(pilha_valores);
}

#endif // CALCULADORA_HPP