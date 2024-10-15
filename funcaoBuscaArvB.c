#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

Pesquisa busca_chave(int RRN_pagina,long alvo,char nome_arqindices[31]){
    int i;
    No PAGINA;
    Pesquisa PESQUISA;

    FILE *arquivo;

    
    if(RRN_pagina==-1){
        PESQUISA.encontrado=0;
        return PESQUISA;
    }

    //abre o arquivo em leitura, pula para o local da pesquisa e le a pagina toda
    arquivo=fopen(nome_arqindices,"rb");
    fseek(arquivo,93*(1+RRN_pagina),SEEK_SET);
    PAGINA = le_no_arvb(arquivo);
    fclose(arquivo);

    for(i=0;i<PAGINA.nroChavesNo;i++){
        if(PAGINA.C[i]==alvo){
            PESQUISA.encontrado=1;              //se a chave atual for a desejada marca que encontrou
            PESQUISA.BYOFF_dado=PAGINA.Pr[i];   //e salva o BYTEOFFSET (no arquivo de dados) do dado relativo a chave na pesquisa, se houver
            return PESQUISA;
        }
        if(PAGINA.C[i]<alvo){
            if(i==m-1){                         //caso a pesquisa tenha chegado ao ultimo dado possivel de uma pagina 
                PESQUISA.encontrado=0;          //e nao tenha encontado o alvo retorna a proxima pagina de pesquisa
                PESQUISA.RRN_pag=PAGINA.P[m];   //que é a relativa ao RRN em P[m].
            }

            continue;//se a chave atual for menor que a chave sendo buscada pula para a proxima chave da pagina, caso a situacao anterior nao tenha ocorrido
        }
        if (PAGINA.C[i]>alvo){
            PESQUISA.encontrado=0;
            PESQUISA.RRN_pag=PAGINA.P[i];
            return PESQUISA;
        }
        
    }
}