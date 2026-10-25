/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 *
 * O programa le uma expressao numerica, converte para a notacao polonesa
 * invertida (RPN) usando pilha e fila, pede o valor das variaveis e apresenta
 * o resultado final.
 *
 * Estruturas usadas:
 *   Fila  -> entrada da expressao (ordem de leitura) e saida em RPN
 *   Pilha -> operadores durante a conversao e valores durante o calculo
 *   Lista -> variaveis da expressao e seus valores
 *
 * Compilacao: g++ -std=c++17 -Wall -Wextra -o main.exe main.cpp
 */

#include <iostream>
#include <string>

// Capacidade das versões estáticas (pilha e fila). Precisa ser definida antes
// dos headers porque as estruturas estáticas usam TAM como tamanho do vetor.
#ifndef TAM
#define TAM 200
#endif // TAM

#include "calculadora.hpp"
#include "FilaDinamica.hpp"
#include "FilaEstatica.hpp"
#include "PilhaDinamica.hpp"
#include "PilhaEstatica.hpp"
#include "Lue.hpp"

using namespace std;

// Troque para 1 para rodar com as estruturas ESTÁTICAS (pilha em vetor e fila
// circular em vetor) ou deixe 0 para as DINÂMICAS (encadeadas). Também é
// possível escolher na compilação: g++ -DESTRUTURA_ESTATICA=1 ...
// A aplicação se comporta da mesma forma nas duas versões: toda a lógica chama
// as operações (inicializar, queue/push, dequeue/pop, top, Vazia, Cheia,
// liberar) por funções livres, então só o tipo das variáveis abaixo muda.
#ifndef ESTRUTURA_ESTATICA
#define ESTRUTURA_ESTATICA 0
#endif

// Resolve uma expressão: cria, inicializa, usa e libera as estruturas.
// As variáveis precisam ficar na main (e não dentro de converter_para_notacao_polonesa)
// justamente para permitir trocar a estrutura sem tocar na lógica.
// Retorna false quando a entrada de dados termina (EOF).
bool calcular_resultado()
{
#if ESTRUTURA_ESTATICA
  FilaEstatica<char> fila_guarda_expressao;             // entrada: expressão caractere por caractere
  FilaEstatica<string> fila_notacao_polonesa_invertida; // saída: tokens em RPN
  PilhaEstatica<char> pilha_operadores;                 // operadores e abre-delimitadores
  PilhaEstatica<double> pilha_valores;                  // operandos já resolvidos
  Lue<string> lista_guarda_variaveis;                   // nome e valor de cada variável
#else
  FilaDinamica<char> fila_guarda_expressao;             // entrada: expressão caractere por caractere
  FilaDinamica<string> fila_notacao_polonesa_invertida; // saída: tokens em RPN
  PilhaDinamica<char> pilha_operadores;                 // operadores e abre-delimitadores
  PilhaDinamica<double> pilha_valores;                  // operandos já resolvidos
  Lue<string> lista_guarda_variaveis;                   // nome e valor de cada variável
#endif

  inicializar(fila_guarda_expressao);
  inicializar(fila_notacao_polonesa_invertida);
  inicializar(pilha_operadores);
  inicializar(pilha_valores);
  inicializar(lista_guarda_variaveis);

  bool continuar = executar_calculadora(fila_guarda_expressao, fila_notacao_polonesa_invertida,
                                        pilha_operadores, pilha_valores, lista_guarda_variaveis);

  // libera a memória ocupada por todas as estruturas (nas estáticas só reinicializa)
  liberar(fila_guarda_expressao);
  liberar(fila_notacao_polonesa_invertida);
  liberar(pilha_operadores);
  liberar(pilha_valores);
  liberar(lista_guarda_variaveis);

  return continuar;
}

int main()
{
#if ESTRUTURA_ESTATICA
  cout << "Estruturas em uso: ESTATICAS" << endl;
#else
  cout << "Estruturas em uso: DINAMICAS" << endl;
#endif
  cout << endl;

  while (calcular_resultado())
  {
    string resposta;

    cout << endl
         << "Deseja calcular outra expressao? (s/N): ";

    if (!getline(cin, resposta))
      break; // entrada encerrada
    if (resposta != "s" && resposta != "S")
      break;

    cout << endl;
  }

  cout << "Fim." << endl;
  return 0;
}