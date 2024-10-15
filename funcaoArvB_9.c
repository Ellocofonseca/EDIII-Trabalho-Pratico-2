#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

void insere_novo_dado(){
    char nome_arqdados[31];
    char nome_arqindices[31];

    //variaveis de cabecalho e de dado de cada tipo de arquivo
    cabecalho CAB_DADOS;
    dados     DADO;

    CabecalhoArvB CAB_ARVB;
    No            NO;

    //variaveis do rrn de cada registro do arquivo de dados e para a criacao da chave do dado
    int RRN;
    char *linha;
    char *nome;
    long chave;

    scanf("%s",nome_arqdados);
    scanf("%s",nome_arqindices);

    FILE *arquivodados;
    FILE *arquivoindices;

    arquivodados = fopen(nome_arqdados, "r");   //abre o arquivo de dados em modo leitura

    if(arquivodados==NULL)  //termina a execucao se o arquivo de dados nao existir
        return;


}
