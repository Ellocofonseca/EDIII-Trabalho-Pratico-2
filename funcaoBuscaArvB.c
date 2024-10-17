#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

//funcao de busca por um dado dentro do arquivo de indices de acordo com a sua chave,
//a pesquisa sempre comeca pelo RRN da pagina raiz
//para essa funcao uma struct foi criada para que a pesquisa retorne varias informacoes
//assim a funcao pode ser usada tanto para pesquisa de dados no arquivo de dados
//quanto para a pesquisa na hora de inserir um dado novo na arvore
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

    //printf("%lu %lu %lu %lu %lu\n",PAGINA.C[0],PAGINA.C[1],PAGINA.C[2],PAGINA.C[3],PAGINA.C[4]);

    for(i=0;i<PAGINA.nroChavesNo;i++){          //checa todas as chaves inseridas na pagina
        if(PAGINA.C[i]==alvo){
            PESQUISA.encontrado=1;              //se a chave atual for a desejada marca que encontrou
            PESQUISA.BYOFF_dado=PAGINA.Pr[i];   //e salva o BYTEOFFSET (no arquivo de dados) do dado relativo a chave na pesquisa, se houver
            return PESQUISA;
        }
        if(PAGINA.C[i]<alvo){
            if(i==PAGINA.nroChavesNo-1){                         //caso a pesquisa tenha chegado ao ultimo dado possivel de uma pagina 
                PESQUISA.encontrado=0;                           //e nao tenha encontado o alvo retorna a proxima pagina de pesquisa
                PESQUISA.RRN_pag=PAGINA.P[PAGINA.nroChavesNo];   //que é a relativa ao RRN em P[PAGINA.nroChavesNo].
                return PESQUISA;
            }

            continue;//se a chave atual for menor que a chave sendo buscada pula para a proxima chave da pagina, caso a situacao anterior nao tenha ocorrido
        }
        if (PAGINA.C[i]>alvo){              //se a chave atual for maior que a chave desejada pula para a pagina apontada anterior à chave atual
            PESQUISA.encontrado=0;
            PESQUISA.RRN_pag=PAGINA.P[i];   //nova pesquisa sera feita na pagina P[i]
            return PESQUISA;
        }
        
    }
}