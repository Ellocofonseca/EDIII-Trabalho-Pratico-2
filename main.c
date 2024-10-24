/*******************************************************************
 * @brief   Trabalho Pratico 2 - Estruturas de dados 3
 * @authors Miguel Rodrigues Fonseca, Bruno Calabria Cortez Navas
 * @date    23/10/2024
 * @note    Trabalho de ED3, quarto periodo, USP Sao Carlos
 *******************************************************************/
#include "./funcoesCriadas.h"

int main(){

    int comando;                //variavel de comando
    scanf("%d",&comando);       //leitura do comando que sera executado
    direcionaComando(comando);  //direciona o programa para cada funcao de acordo com o nro de comando

    return 0;
}