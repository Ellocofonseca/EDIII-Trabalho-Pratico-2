#ifndef __FUNCOES_CRIADAS__
#define __FUNCOES_CRIADAS__

//-------BIBLIOTECAS------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

//------MENSAGENS-DE-ERRO-----
#define ERRO_PADRAO     "Falha no processamento do arquivo\n" //mensagem padrao de erro
#define ERRO_REGISTRO   "Registro inexistente.\n"              //mensagem de erro para registro inexistente
#define ERRO_COMANDO    "O comando digitado não existe\n"     //mensagem de erro caso o usuario digite um comando invalido
#define ERRO_CADASTRO   "Informação já inserida no arquivo\n" //mensagem de erro para dados ja cadastrados


// CADA PAGINA DE DISCO POSSUI 1600 BYTES
// REGISTRO DE CABECALHO, 21 BYTES + lixo = 1600 bytes
typedef struct
{
    char status;                // Indicacao da consistencia do arq. de dados, se for 1 esta consistente e se for 0 nao     1 byte
    int topo;                   // Armazena o RRN de um registro logicamente removido ou -1 caso nao haja                   4 bytes
    int proxRRN;                // Armazena o valor do prox RRN disponivel, inicia como 0 e muda caso necessario            4 bytes
    int nroRegRem;              // Armazena o nro de registros logicamene marcados como removidos, inicia como 0            4 bytes
    int nroPagDisco;            // Armazena o nro de paginas de disco ocupadas pelo arq. de dados                           4 bytes
    int qttCompacta;            // Indica a qtd de vezes que o arquivo foi compactado                                       4 bytes
    //char lixo_pag[1579];      // Lixo para completar a pag de disco, preencher com '$'                                    1579 bytes
} cabecalho;


// REGISTRO DE DADOS, 18 BYTES fixos + 142 BYTES VARIAVEIS = 160   1/10 DE PAGINA DE DISCO
typedef struct
{
    char removido;              //  Indica se o registro esta logicamente removido                    1 byte
    int encadeamento;           //  Armazena o RRN do proximo registro logicamente removido           4 bytes
    int populacao;              //  Qtd de individuos da especie que viviam em determinado lugar      4 bytes
    float tamanho;              //  Tamanho do individuo                                              4 bytes
    char unidadeMedida;         //  Unidade de medida da velocidade do individuo                      1 byte
    int velocidade;             //  Velocidade do individuo                                           4 bytes

    char variavel[142];         //  Bytes variaveis                                                   142 bytes
                                //  Nome-Especie-Habitat-Tipo-Dieta-Alimento
                                //  Cada campo variavel eh separado por um delimitador '#'
                                //  As strings nao devem terminar com \0
                                //  Se sobrar espaço preencher com '$'
} dados;


//--------FUNCOES-DO-PROGRAMA------------

//FUNCOES PADRAO, REDIRECIONAMENTO, COMPARACAO, TECLADO
void direcionaComando(int codigo);                              //FUNCAO DE SELECAO DE FUNCAO DE ACORDO COM O COMANDO
int checa_nome_campo(char *string);                             //FUNCAO QUE RETORNA UM INTEIRO DE ACORDO COM O NOME DE UM CAMPO ESCRITO
dados le_do_teclado();                                          //FUNCAO QUE LE DADOS PARA SEREM INSERIDOS EM UM ARQUIVO EXISTENTE DO TECLADOS

//FUNCAO DE PRINTF
void exibe_registro(dados DADO);                                //FUNCAO QUE PRINTA NA TELA UM REGISTRO 

//FUNCOES DE FREAD DE ARQUIVO BINARIO
cabecalho le_cabecalho(FILE *arquivo);                                      //FUNCAO QUE LE A PORCAO INICIAL DO CABECALHO DE UM ARQUIVO BIN
dados le_registro(FILE *arquivo);                                           //FUNCAO QUE LE UM DOS VARIOS DADOS
//FUNCOES DE FWRITE EM ARQUIVO BINARIO
void escreve_cabecalho_bin(char nomebin[31],cabecalho CAB);                 //FUNCAO QUE PEGA UM REGISTRO DO TIPO cabecalho E ESCREVE NUM ARQUIVO BIN
void atualiza_cabecalho_bin(char nomebin[31],cabecalho CAB);                //FUNCAO QUE PEGA UM REGISTRO DO TIPO cabecalho E ATUALIZA O BIN
void escreve_dado_bin(char nomebin[31],dados DADO);                         //FUNCAO QUE PEGA UM REGISTRO DO TIPO dados E ESCREVE NO FIM DO ARQUIVO
void atualiza_dado_bin(dados DADO,FILE *arquivobin);                        //FUNCAO QUE PEGA UM REGISTRO DO TIPO dados E ATUALIZA O BIN, PODENDO SOBRESCREVER UM REGISTRO LOGICAMENTE REMOVIDO
void remove_dado_bin(FILE *arquivobin,int encademento);                     //FUNCAO QUE REALIZA A REMOCAO LOGICA DE UM REGISTRO

//FUNCOES DE CADA COMANDO EM ORDEM
void csv_para_bin();            //COMANDO 1 -> LE DE UM ARQUIVO CVS E ARMAZENA SUAS INFORMACOES NUM ARQUIVO BINARIO 
void exibe_campos();            //COMANDO 2 -> GERA UM RELATORIO QUE IMPRIME NO TERMINAL AS INFORMACOES DE TODAS AS ESPECIES NAO REMOVIDAS DE UM ARQUIVO BIN
void busca_determinada();       //COMANDO 3 -> GERA UM RELATORIO QUE IMPRIME NO TERMINAL AS INFORMACOES DE ESPECIES QUE POSSUIREM PARAMETROS IGUAIS AOS DE PESQUISA 
void remocao_logica();          //COMANDO 4 -> REALIZA A REMOCAO LOGICA DE DADOS ESPECIFICADOS
void insere_registro();         //COMANDO 5 -> REALIZA A INSERCAO DE UM DADO NOVO NO ARQUIVO BIN APOS LER DADOS DO TECLADO, AS INSERCOES PRIMEIRO SAO FEITAS NA PILHA DE REMOCAO E DEPOIS AO FIM DO ARQUIVO CASO NAO HAJA REGISTROS LOGICAMENTE REMOVIDOS
void compactador();             //COMANDO 6 -> COMPACTA O ARQUIVO REESCREVENDO ELE SEM OS REGISTROS LOGICAMENTE REMOVIDOS

//FUNCOES FORNECIDAS
void scan_quote_string(char *str);

/**
 * Use essa função para comparação no run.codes.
 * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
 *
 * Ela vai abrir de novo para leitura e depois fechar
 * (você não vai perder pontos por isso se usar ela).
 */
void binarioNaTela(char *nomeArquivoBinario);

#endif // __FUNCOES__