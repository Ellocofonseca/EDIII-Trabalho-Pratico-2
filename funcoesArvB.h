#ifndef __FUNCOES_ARVB__
#define __FUNCOES_ARVB__

//----ORDEM-DA-ARVORE-----

#define m 5

//----NOS-DA-ARVORES

typedef struct{
    char folha;         //indica se eh no folha
    int  nroChavesNo;   //quantas chaves estao armazenadas aqui
    int  RRNdoNo;       //RRN do no
    int  P[m];          //ponteiros do no
    long C[m-1];        //chaves do no
    long Pr[m-1]        //Local do dado referente a chave no arquivo de dados

}No;

//----CABECALHO------

typedef struct{
    char status;        //status do cabecalho
    int  noRaiz;        //RRN do no raiz 
    int  RRNproxNo;     //RRN do proximo no que sera inserido

}CabecalhoArvB;

typedef struct{
    int encontrado; //variavel que marca se o dado foi encontrado ou nao
    int BYOFF_dado; //byteoffset do dado relativo a chave (essa variavel pode ou nao ser usada, depende se a funcao eh de busca ou insercao)
    int RRN_pag;    //RRN da proxima pagina em que a pesquisa sera feita se o dado ainda nao tenha sido encontrado
}Pesquisa;

long converteNome(char* str);   //funcao que transforma o campo de nome do arquivo de dados em uma chave para os indices da arvore-b

void cria_arq_indices();        //funcao que cria o arquivo de indices em arvore-b, FUNCAO 7


//funcoes usadas para escrever tanto os nos quanto o cabecalho no arquivo de indices
void escreve_cabecalho_arvb(FILE *arquivo,CabecalhoArvB CAB);
void escreve_no_arvb(FILE *arquivo,No no);

//funcoes que leem o cabecalho e os nos do arquivo de indices;
CabecalhoArvB le_cab_arvb(FILE *arquivo);
No le_no_arvb(FILE *arquivo);

void driver();


void insere_no();
void insere_chave(long chave,int RRN,char nome_arqindices[31]);

Pesquisa busca_chave(int RRN_raiz,long alvo,char nome_arqindices[31]);

#endif // __FUNCOES__