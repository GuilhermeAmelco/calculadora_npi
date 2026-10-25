/*
 * Trabalho de Estrutura de Dados - Calculadora em notacao polonesa invertida
 * Integrantes: Guilherme Amelco e Caue
 *
 * Estruturas usadas:
 *   Fila  -> entrada da expressao (ordem de leitura) e saida em RPN
 *   Pilha -> operadores durante a conversao e valores durante o calculo
 *   Lista -> variaveis da expressao e seus valores
 *
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

#ifndef ESTRUTURA_ESTATICA
#define ESTRUTURA_ESTATICA 0
#endif

bool calcular_resultado()
{
  // FilaEstatica<char> fila_guarda_expressao;
  // FilaEstatica<string> fila_notacao_polonesa_invertida;
  // PilhaEstatica<char> pilha_operadores;
  // PilhaEstatica<double> pilha_valores;
  // Lue<string> lista_guarda_variaveis;
  FilaDinamica<char> fila_guarda_expressao;             // entrada: expressão caractere por caractere
  FilaDinamica<string> fila_notacao_polonesa_invertida; // saída: tokens em RPN
  PilhaDinamica<char> pilha_operadores;                 // operadores e abre-delimitadores
  PilhaDinamica<double> pilha_valores;                  // operandos já resolvidos
  Lue<string> lista_guarda_variaveis;                   // nome e valor de cada variável

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
  // cout << "Estruturas em uso: ESTATICAS" << endl;
  cout << "Estruturas em uso: DINAMICAS" << endl;

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