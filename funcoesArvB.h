#ifndef __FUNCOES_ARVB__
#define __FUNCOES_ARVB__


#include "./funcoesCriadas.h"
//----ORDEM-DA-ARVORE-----

#define m 5

//----NOS-DA-ARVORES

typedef struct{
    char folha;         //indica se eh no folha
    int  nroChavesNo;   //quantas chaves estao armazenadas aqui
    int  RRNdoNo;       //RRN do no
    int  P[m];          //ponteiros do no
    long C[m-1];        //chaves do no
    long Pr[m-1];       //BYTEOFFSET do dado referente a chave no arquivo de dados

}No;

//----CABECALHO------

typedef struct{
    char status;        //status do cabecalho
    int  noRaiz;        //RRN do no raiz 
    int  RRNproxNo;     //RRN do proximo no que sera inserido

}CabecalhoArvB;

//----AUXILIAR-PESQUISA----
//essa struct auxiliar eh usada na hora de fazer pesquisas na arvore B, ela sera usada tanto na hora
//de fazer pesquisas normais e pesquisas para insercao de dados na arvore, assim possibilitando
//que a mesma funcao de pesquisa possa ser utilizada em ambos os casos de maneira normal
typedef struct{
    //USADOS NA PESQUISA 'NORMAL'
    int encontrado; //variavel que marca se o dado foi encontrado ou nao
    int BYOFF_dado; //byteoffset do dado relativo a chave (essa variavel pode ou nao ser usada, depende se a funcao eh de busca ou insercao)
    int RRN_pag;    //RRN da proxima pagina em que a pesquisa sera feita se o dado ainda nao tenha sido encontrado

    //USADOS NA PESQUISA DE INSERCAO
    //dados da promocao na volta da chamada recursiva
    long chave_promovida;   
    long byoff_promovido;
    int p_dir_promovido;

    //dados que indicam uma possivel atualizacao de raiz caso ocorra um split em uma raiz 
    //e tambem a quantidade de paginas criadas para atualizar o rrn do proximo no
    int nova_raiz;
    int nova_pagina;
}Pesquisa;

//----AUXILIAR-SPLIT-------
// tipo auxiliar usado para reordenar as paginas e determinar quem sera promovido na hora do split
// e qual a pagina nova que sera criada
// a struct auxiliar pode ser usada nos dois tipos principais de split
typedef struct{
    No PAG1;            //pagina que permanecera
    No PAG2;            //pagina irma que sera criada
    No PAG3;            //pagina da nova raiz em caso de split numa raiz

    int P_promovido;    //dados de promocao no caso de split fora da raiz
    long C_promovido;
    long Pr_promovido;
}Split;

long converteNome(char* str);   //funcao dada que transforma o campo de nome do arquivo de dados em uma chave para os indices da arvore-b

//FUNCIONALIDADES
void cria_arq_indices();        //funcao que cria o arquivo de indices em arvore-b, FUNCAO 7
void pesquisa();                //funcao que realiza uma busca na arvore para econtrar o BYTEOFFSET de um dado relativo a uma chave no arquivo de dados, FUNCAO 8
void insere_novo_dado();        //funcao que insere um novo dado tanto no arquivo de dados quanto numa arvore B ja existente, FUNCAO 9

//funcoes de FREAD e FWRITE usadas para escrever e ler tanto as paginas quanto o cabecalho do arquivo de indices
void escreve_cabecalho_arvb(FILE *arquivo,CabecalhoArvB CAB);
void escreve_no_arvb(FILE *arquivo,No no);
CabecalhoArvB le_cab_arvb(FILE *arquivo);
No le_no_arvb(FILE *arquivo);

//FUNCOES DE INSERCAO - funcoes que realizam a insercao de chaves num arquivo de indices
Pesquisa insere_chave(long chave,long byteoffset,FILE *arquivo,int rrn_atual,int raiz_original);                    //funcao recursiva que realiza a insercao de uma chave na arvore-b
void cria_raiz(FILE *arqindices);                                                                                   //funcao que cria uma raiz vazia caso nao exista no arquivo
No reordena_pagina(No PAGINA);                                                                                      //funcao que reordena uma pagina com no maximo m-1 chaves (numero maximo de chaves na pagina)
Split ordena_split(No PAGINA, long chave_p, long byoff_p, int pont_dir_p, int eh_raiz, int eh_folha,FILE *arquivo); //funcao que ordena as m-1 chaves de uma pagina junto da chave que esta tentando se inserir na pagina, determinando (de acordo com as regras de split), quem sera promovido e em quais paginas as 'm' chaves estarao

//FUNCAO DE BUSCA - funcao de busca por uma chave na arvore-b (ela eh bem simples mas esta separada em um arquivo proprio)
Pesquisa busca_chave(int RRN_raiz,long alvo,FILE *arquivo);

#endif // __FUNCOES__