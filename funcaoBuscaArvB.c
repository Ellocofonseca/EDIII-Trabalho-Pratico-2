#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

//funcao de busca por um dado dentro do arquivo de indices de acordo com a sua chave,
//a pesquisa sempre comeca pelo RRN da pagina raiz

//para essa funcao uma struct foi criada para que a pesquisa retorne varias informacoes
//assim a funcao pode ser usada tanto para pesquisa de dados no arquivo de dados
//quanto para a pesquisa na hora de inserir um dado novo na arvore

//a funcao tem como parametros a pagina que se esta fazendo a pesquisa, o alvo de pesquisa e o arquivo em que se esta pesquisando
Pesquisa busca_chave(int RRN_pagina,long alvo,FILE *arquivo){

    int i;
    No PAGINA;          //pagina lida no momento
    Pesquisa PESQUISA;  //struct auxiliar da pesquisa
    
    //se o RRN da pesquisa for -1 significa que estamos no fim da arvore
    //e o alvo nao foi encontrado, entao ele nao esta na arvore
    if(RRN_pagina==-1){         
        PESQUISA.encontrado=0;
        return PESQUISA;
    }

    //pula para o local da pesquisa e le a pagina toda
    fseek(arquivo,93*(1+RRN_pagina),SEEK_SET);
    PAGINA = le_no_arvb(arquivo);

    //se a pagina estiver vazia o dado nao existe (util no caso de pesquisa na primeira raiz da arvore)
    if(PAGINA.nroChavesNo==0){
        PESQUISA.encontrado=0;
        PESQUISA.RRN_pag=-1;
    return PESQUISA;

    }

    //loop de pesquisa dentro da pagina
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
            PESQUISA.encontrado=0;          //ainda nao encotrado
            PESQUISA.RRN_pag=PAGINA.P[i];   //nova pesquisa sera feita na pagina P[i]
            return PESQUISA;
        }
        
    }
}