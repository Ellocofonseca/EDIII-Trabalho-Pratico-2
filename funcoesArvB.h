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
    long Pr[m-1];       //Local do dado referente a chave no arquivo de dados

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
    int encontrado; //variavel que marca se o dado foi encontrado ou nao
    int BYOFF_dado; //byteoffset do dado relativo a chave (essa variavel pode ou nao ser usada, depende se a funcao eh de busca ou insercao)
    int RRN_pag;    //RRN da proxima pagina em que a pesquisa sera feita se o dado ainda nao tenha sido encontrado
    
                    //usados na pesquisa de insercao
    int eh_folha;   //indica se a pagina final eh um no folha
    int ocupacao;   //indica a ocupacao da pagina 
}Pesquisa;

long converteNome(char* str);   //funcao que transforma o campo de nome do arquivo de dados em uma chave para os indices da arvore-b

void cria_arq_indices();        //funcao que cria o arquivo de indices em arvore-b, FUNCAO 7
void pesquisa();                //funcao que realiza uma busca na arvore para econtrar o BYTEOFFSET de um dado relativo a uma chave no arquivo de dados, FUNCAO 8
void insere_novo_dado();        //funcao que insere um novo dado tanto no arquivo de dados quanto numa arvore B ja existente 

//funcoes usadas para escrever tanto os nos quanto o cabecalho no arquivo de indices
void escreve_cabecalho_arvb(FILE *arquivo,CabecalhoArvB CAB);
void escreve_no_arvb(FILE *arquivo,No no);

//funcoes que leem o cabecalho e os nos do arquivo de indices;
CabecalhoArvB le_cab_arvb(FILE *arquivo);
No le_no_arvb(FILE *arquivo);

void driver();
void insere_no();
void insere_chave(long chave,int RRN,char nome_arqindices[31]);

//funcao que realiza a busca na arvore b tanto em casos de pesquisa de RRN do arquivo de dados quanto em casos de insercao
Pesquisa busca_chave(int RRN_raiz,long alvo,char nome_arqindices[31]);

#endif // __FUNCOES__