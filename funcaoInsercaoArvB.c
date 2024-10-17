#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

void cria_primeira_raiz(char nome_arqindices[31]){
    FILE *arquivoindices;
    No PAGINA;
    int i;
    

    //cria a primeira raiz toda zerada, mas como no folha tambem
    PAGINA.nroChavesNo=0;
    PAGINA.folha='1';
    PAGINA.RRNdoNo=0;
    for(i=0;i<m;i++){
        PAGINA.P[i]=-1;
        if(i!=m-1){
            PAGINA.C[i]=-1;
            PAGINA.Pr[i]=-1;
        }    
    }

    arquivoindices = fopen(nome_arqindices,"ab");
    escreve_no_arvb(arquivoindices,PAGINA);
    fclose(arquivoindices);
}

int insere_chave(long chave,long byteoffset,char nome_arqindices[31],int raiz){
    int nova_raiz,i,j;
    int rrn_pesquisa;
    Pesquisa PESQUISA;
    No PAGINA;
    FILE *arquivo;

    int nos_visitados[62];      //vetor que sera usado para salvas os nos que foram visitados na hora da pesquisa, com isso nao eh preciso fazer
                                //a rotina de split de forma recursiva por exemplo, e tambem nao corre o risco de nao ter espaco o suficente
                                //para salvar os nos, ja que uma arvore b de ordem 5 com 61 niveis precisa de MUITOS dados
    for(i=0;i<62;i++)
        nos_visitados[i]=-2;

    rrn_pesquisa=raiz;
    nova_raiz=raiz;

    i=0;
    while (rrn_pesquisa!=-1 || PESQUISA.encontrado==1)        //se o no da pesquisa chegar ao fim e nao tiver encontrado a chave, sai do loop
    {   
        nos_visitados[i]=rrn_pesquisa;
        i++;
        PESQUISA=busca_chave(rrn_pesquisa,chave,nome_arqindices);   //faz a pesquisa na pagina atual
        if(PESQUISA.encontrado==1)  //se encontrou a chave sai do loop
            break;
        rrn_pesquisa=PESQUISA.RRN_pag;//se nao tiver encontrado na pagina atual pula para a proxima pagina indicada na pesquisa
    }

    //se a chave nao existir no arquivo de dados realiza a insercao dela
    if(PESQUISA.encontrado==0 && rrn_pesquisa==-1){
        for(i=61;i>=0;i--){             //loop para checar os nos que foram visitados de baixo pra cima (bottom-up)
            if(nos_visitados[i]!=-2){   //comeca apenas nos valores validos de no visitado

                arquivo=fopen(nome_arqindices,"rb");                //le a pagina
                fseek(arquivo,93*(1+nos_visitados[i]),SEEK_SET);
                PAGINA = le_no_arvb(arquivo);
                fclose(arquivo);
                
                //ta tudo errado abaixo disso aqui
                //insercao normal, eba alegria
                if(PAGINA.nroChavesNo<m-1){
                    
                    break;
                }
                //insercao com split, dor e sofrimento
                //lembrar de checar se modificou a raiz!
                if(PAGINA.nroChavesNo==m-1){
                    //nova_raiz=insere_chave();
                }

            }
        }
    }

    

    return nova_raiz;
}