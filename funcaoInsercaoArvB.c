#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

void cria_raiz(FILE *arqindices){
    No NO;
    int i;
    //'zera' todos os dados do primeiro no da criacao da arvore
    NO.folha='1';
    NO.nroChavesNo=0;
    NO.RRNdoNo=0;

    for(i=0;i<m;i++){
        NO.P[i]=-1;
        if(i!=m-1){
            NO.C[i]=-1;
            NO.Pr[i]=-1;
        }
    }
    //escreve o primeiro no zerado na arvore como folha;
    escreve_no_arvb(arqindices,NO);

}


Pesquisa insere_chave(long chave,long byteoffset,FILE *arquivo,int rrn_atual,int raiz_original){
    int nova_raiz;
    int rrn_pesquisa;
    Pesquisa PESQUISA;
    No PAGINA;
    Split SPLIT;

    PESQUISA.nova_pagina=0;


    if (rrn_atual!=-1 || PESQUISA.encontrado==1)        //se o no da pesquisa chegar ao fim e nao tiver encontrado a chave, inicia a volta da recursao
    {   
        PESQUISA=busca_chave(rrn_atual,chave,arquivo);   //faz a pesquisa na pagina atual
        //teoricamente teria que criar uma parada caso a chave ja exista no arquivo, porem nao faz parte do trabalho
        rrn_pesquisa=PESQUISA.RRN_pag;//se nao tiver encontrado na pagina atual pula para a proxima pagina indicada na pesquisa
        PESQUISA = insere_chave(chave,byteoffset,arquivo,rrn_pesquisa,raiz_original);
    }
    //quando chegar no fim da pesquisa (raiz=-1) a funcao pula pra ca, quando esse caso retornar as recursoes partem daqui tambem

    if(rrn_atual==-1)//quando chegar no fim da pesquisa retorna a PRIMEIRA chave como promovida para voltar na recursao e sem ponteiro para filho
    {
        PESQUISA.chave_promovida=chave;
        PESQUISA.byoff_promovido=byteoffset;
        PESQUISA.p_dir_promovido=-1;
        return PESQUISA;
    }

    //se houver chave promovida realiza uma insercao, checando split etc etc e retornando se tem promocao ou nao, alem de se vai ter um no raiz novo quando houver split da raiz atual
    if(PESQUISA.chave_promovida==-1)//se nao houver chave promovida pra inserir nao faz nada, assim a partir desse ponto a recursao volta sem fazer mais nada
    { 
        return PESQUISA;
    }
    else//se houver chave promovida tem que acontecer split ou insercao normal
    {
        fseek(arquivo,93*(1+rrn_atual),SEEK_SET);   //move o ponteiro para ler a pagina do arquivo
        PAGINA=le_no_arvb(arquivo);

        if(PAGINA.nroChavesNo<=m-2)  //caso haja espaco na pagina atual para inserir a chave, insere ela e retorna promovido=-1
        {
            //reordena a pagina com a chave nova
            PAGINA.C[PAGINA.nroChavesNo]=PESQUISA.chave_promovida;
            PAGINA.P[PAGINA.nroChavesNo+1]=PESQUISA.p_dir_promovido;
            PAGINA.Pr[PAGINA.nroChavesNo]=PESQUISA.byoff_promovido;
            PAGINA.nroChavesNo++;
            PAGINA=reordena_pagina(PAGINA);

            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);    //fseek na posicao de reescrita
            escreve_no_arvb(arquivo,PAGINA);             //reescreve a pagina atualizada
            PESQUISA.chave_promovida=-1;                 //sem mais promocoes
            return PESQUISA;                             //retorna pequisa com promocao=-1;

        }
        else if(PAGINA.nroChavesNo==m-1 && PAGINA.folha=='1' && rrn_atual==raiz_original)   //split no folha + raiz
        {
            //reordena as paginas de acordo com o split delas, abre para escrita no fim do arquivo e escreve as paginas novas
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,1,1);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA
            escreve_no_arvb(arquivo,SPLIT.PAG3);    //ESCREVE A PAGINA RAIZ NOVA
            PESQUISA.nova_raiz=SPLIT.PAG3.RRNdoNo;  //ATUALIZA O RRN DA NOVA RAIZ
            PESQUISA.nova_pagina+=2;                //AUMENTA EM 2 O NRO DE PAGINAS CRIADAS
            PESQUISA.chave_promovida=-1;            //SEM MAIS CHAVES PROMOVIDAS
            return PESQUISA;

        }
        else if(PAGINA.nroChavesNo==m-1 && PAGINA.folha=='0' && rrn_atual==raiz_original)   //splir raiz
        {
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,1,0);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA
            escreve_no_arvb(arquivo,SPLIT.PAG3);    //ESCREVE A PAGINA RAIZ NOVA
            PESQUISA.nova_raiz=SPLIT.PAG3.RRNdoNo;  //ATUALIZA O RRN DA NOVA RAIZ
            PESQUISA.nova_pagina+=2;                //AUMENTA EM 2 O NRO DE PAGINAS CRIADAS
            PESQUISA.chave_promovida=-1;            //SEM MAIS CHAVES PROMOVIDAS
            return PESQUISA;
        }
        else if(PAGINA.nroChavesNo==m-1 && PAGINA.folha=='1')   //split no folha
        {
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,0,1);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA QUE PERMANECE
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA

            PESQUISA.chave_promovida=SPLIT.C_promovido;
            PESQUISA.byoff_promovido=SPLIT.Pr_promovido;
            PESQUISA.p_dir_promovido=SPLIT.P_promovido;        
            PESQUISA.nova_pagina+=1;
            return PESQUISA;

        }
        else if(PAGINA.nroChavesNo==m-1 && PAGINA.folha=='0')   //split fora no folha
        {
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,0,0);

            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE    
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA

            PESQUISA.chave_promovida=SPLIT.C_promovido;
            PESQUISA.byoff_promovido=SPLIT.Pr_promovido;
            PESQUISA.p_dir_promovido=SPLIT.P_promovido;
            PESQUISA.nova_pagina+=1;
            return PESQUISA;

        }
        
    }   
}

No reordena_pagina(No PAGINA)
{
    int i,j;
    long C_aux, Pr_aux;
    int P_aux;
    for (int i = 0; i < PAGINA.nroChavesNo - 1; i++) {
        for (int j = 0; j < PAGINA.nroChavesNo - i - 1; j++) {
            //reordena com bubblesort (sim), fazendo com que os ponteiros a direita de cada chave sejam trocados
            if (PAGINA.C[j] > PAGINA.C[j + 1]){ 
                C_aux=PAGINA.C[j];
                P_aux=PAGINA.P[j+1];
                Pr_aux=PAGINA.Pr[j];

                PAGINA.C[j]=PAGINA.C[j+1];
                PAGINA.P[j+1]=PAGINA.C[j+2];
                PAGINA.Pr[j]=PAGINA.C[j+1];
                
                PAGINA.C[j+1]=C_aux;
                PAGINA.P[j+2]=P_aux;
                PAGINA.Pr[j+1]=Pr_aux;
            }


        }
    }

    return PAGINA;
}

Split ordena_split(No PAGINA, long chave_p, long byoff_p, int pont_dir_p, int eh_raiz, int eh_folha){
    Split SPLIT;

    if (eh_raiz==1 && eh_folha==1) //caso de split da primeira raiz  
    {
        


    }
    else if (eh_raiz==1 && eh_folha==0) //split de outras raizes
    {
        
        
    }
    else if(eh_raiz==0 && eh_folha==1)                //split no folha generico
    {



    }
    else if(eh_raiz==0 && eh_folha==0)               //split no interior generico
    {



    }
    

    return SPLIT;
}

// P1 C1 Pr1 | P2 C2 Pr2 | P3 C3 Pr3 | P4 C4 Pr4 | P5 

// 0 1 | 2 3 4 -> chave 2 sobe, esquerda aponta pra pagina antiga e direita aponta pra pagina nova