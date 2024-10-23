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
        if(PAGINA.nroChavesNo<=m-2)  //caso haja espaco na pagina atual para inserir a chave, insere ela e retorna promovido=-1 (ok)
        {
            //reordena a pagina com a chave nova
            PAGINA.C[PAGINA.nroChavesNo]=PESQUISA.chave_promovida;
            PAGINA.P[PAGINA.nroChavesNo+1]=PESQUISA.p_dir_promovido;
            PAGINA.Pr[PAGINA.nroChavesNo]=PESQUISA.byoff_promovido;
            PAGINA.nroChavesNo++;
            PAGINA=reordena_pagina(PAGINA);

            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);    //fseek na posicao de reescrita
            escreve_no_arvb(arquivo,PAGINA);             //reescreve a pagina atualizada

            fseek(arquivo,-93,SEEK_CUR);
            PAGINA=le_no_arvb(arquivo);
            printf("INSERCAO NORMAL: %ld\n",chave);
            print(PAGINA);
            printf("\n");

            PESQUISA.chave_promovida=-1;                 //sem mais promocoes
            PESQUISA.nova_raiz=raiz_original;            //raiz inalterada
            return PESQUISA;                             //retorna pequisa com promocao=-1;

        }
        else if(PAGINA.nroChavesNo==m-1 && rrn_atual==raiz_original && PAGINA.folha=='1')   //split raiz+folha (ok)
        {
            //reordena as paginas de acordo com o split delas, abre para escrita no fim do arquivo e escreve as paginas novas
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,1,1,arquivo);
            fseek(arquivo,93,SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA
            escreve_no_arvb(arquivo,SPLIT.PAG3);    //ESCREVE A PAGINA RAIZ NOVA

            PESQUISA.nova_raiz=SPLIT.PAG3.RRNdoNo;  //ATUALIZA O RRN DA RAIZ
            PESQUISA.nova_pagina+=2;                //AUMENTA EM 2 O NRO DE PAGINAS CRIADAS
            PESQUISA.chave_promovida=-1;            //SEM MAIS CHAVES PROMOVIDAS

            printf("SPLIT RAIZ+FOLHA: %ld\n",chave);
            fseek(arquivo,93,SEEK_SET);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            printf("\n");


            return PESQUISA;

        }
        else if(PAGINA.nroChavesNo==m-1 && rrn_atual==raiz_original && PAGINA.folha=='0')   //split raiz nao folha
        {
            //reordena as paginas de acordo com o split delas, abre para escrita no fim do arquivo e escreve as paginas novas
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,1,0,arquivo);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA
            escreve_no_arvb(arquivo,SPLIT.PAG3);    //ESCREVE A PAGINA RAIZ NOVA

            printf("SPLIT RAIZ: %ld\n",chave);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            fseek(arquivo,-93*2,SEEK_END);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            printf("\n");


            PESQUISA.nova_raiz=SPLIT.PAG3.RRNdoNo;  //ATUALIZA O RRN DA NOVA RAIZ
            PESQUISA.nova_pagina+=2;                //AUMENTA EM 2 O NRO DE PAGINAS CRIADAS
            PESQUISA.chave_promovida=-1;            //SEM MAIS CHAVES PROMOVIDAS
            return PESQUISA;

        }
        else if(PAGINA.nroChavesNo==m-1 && rrn_atual!=raiz_original && PAGINA.folha=='0')   //split nao eh folha nem raiz
        {
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,0,0,arquivo);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA QUE PERMANECE
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA

            printf("SPLIT GENERICO: %ld\n",chave);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            fseek(arquivo,-93,SEEK_END);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            printf("Promovido: %ld",SPLIT.C_promovido);
            printf("\n");



            PESQUISA.chave_promovida=SPLIT.C_promovido;
            PESQUISA.byoff_promovido=SPLIT.Pr_promovido;
            PESQUISA.p_dir_promovido=SPLIT.P_promovido;        
            PESQUISA.nova_pagina+=1;
            PESQUISA.nova_raiz=raiz_original;
            return PESQUISA;

        }
        else if(PAGINA.nroChavesNo==m-1 && rrn_atual!=raiz_original && PAGINA.folha=='1')   //split eh folha    (OK)
        {
            SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,0,1,arquivo);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA QUE PERMANECE
            fseek(arquivo,0,SEEK_END);
            escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA

            printf("SPLIT FOLHA: %ld\n",chave);
            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            fseek(arquivo,-93,SEEK_END);
            PAGINA=le_no_arvb(arquivo);
            print(PAGINA);
            printf("Promovido: %ld",SPLIT.C_promovido);
            printf("\n");


            PESQUISA.chave_promovida=SPLIT.C_promovido;
            PESQUISA.byoff_promovido=SPLIT.Pr_promovido;
            PESQUISA.p_dir_promovido=SPLIT.P_promovido;        
            PESQUISA.nova_pagina+=1;
            PESQUISA.nova_raiz=raiz_original;
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
                PAGINA.P[j+1]=PAGINA.P[j+2];
                PAGINA.Pr[j]=PAGINA.Pr[j+1];
                
                PAGINA.C[j+1]=C_aux;
                PAGINA.P[j+2]=P_aux;
                PAGINA.Pr[j+1]=Pr_aux;
            }


        }
    }

    return PAGINA;
}

Split ordena_split(No PAGINA, long chave_p, long byoff_p, int pont_dir_p, int eh_raiz, int eh_folha, FILE *arquivo){
    Split SPLIT;
    int i,j;
    long Cs[m],Prs[m];
    int Ps[m+1];
    long C_aux, Pr_aux;
    int P_aux;

     //coloca os dados da pagina para fazer a ordenacao
    for(i=0;i<m;i++){      
        Ps[i]=PAGINA.P[i];
        if(i!=m-1){
            Cs[i]=PAGINA.C[i];
            Prs[i]=PAGINA.Pr[i];
        }
    }
    Ps[m+1]=pont_dir_p;     //coloca o dado que esta tentando ser inserido para ordenacao
    Cs[m]=chave_p;
    Prs[m]=byoff_p;


    for (int i = 0; i < m+1 - 1; i++) {
        for (int j = 0; j < m+1 - i - 1; j++) {
            //reordena com bubblesort (sim), fazendo com que os ponteiros a direita de cada chave sejam trocados
            if (Cs[j] > Cs[j + 1]){ 
                C_aux=Cs[j];
                P_aux=Ps[j+1];
                Pr_aux=Prs[j];

                Cs[j]=Cs[j+1];
                Ps[j+1]=Ps[j+2];
                Prs[j]=Prs[j+1];
                
                Cs[j+1]=C_aux;
                Ps[j+2]=P_aux;
                Prs[j+1]=Pr_aux;
            }


        }
    }

    // coloca os dados nas respectivas paginas resultantes do split
    for (i = 0; i < m; i++)
    {
        if (i < (int)(m / 2))
        {
            SPLIT.PAG1.C[i] = Cs[i];
            SPLIT.PAG1.P[i] = Ps[i];
            SPLIT.PAG1.Pr[i] = Prs[i];
        }
        if (i >= (int)(m / 2))
        {
            SPLIT.PAG1.P[i] = -1;

            if (i != m - 1)
            {
                SPLIT.PAG1.C[i] = -1;
                SPLIT.PAG1.Pr[i] = -1;
            }
        }
    }
    SPLIT.PAG1.nroChavesNo=(int)(m/2);  //nova qtd de chaves dentro do no
    SPLIT.PAG1.RRNdoNo=PAGINA.RRNdoNo;  //rrn do no eh o que veio da pagina
    SPLIT.PAG1.folha=PAGINA.folha;      //status de folha ou nao tambem vem da pagina anterior

    //pagina que sera criada apos
    //os dados dessa pagina sao os da metade superior, tirando o que foi promovido
    for (i = 0; i < m+1; i++)
    {
        if (i > (int)(m / 2))
        {
            SPLIT.PAG2.C[i-3] = Cs[i];
            SPLIT.PAG2.P[i-3] = Ps[i];
            SPLIT.PAG2.Pr[i-3] = Prs[i];
        }
        if(i<(int)(m/2)){
            SPLIT.PAG2.C[i+3] = -1;
            SPLIT.PAG2.P[i+2] = -1;
            SPLIT.PAG2.Pr[i+2] = -1;
        }

    }
    SPLIT.PAG2.C[2] = -1;
    SPLIT.PAG2.Pr[2] = -1;
    SPLIT.PAG2.P[m-1]=-1;

    if((eh_raiz==1 && eh_folha==0) || (eh_raiz==0 && eh_folha==0)){
        SPLIT.PAG1.P[2] = Ps[2];
    }
    
    fseek(arquivo,-93,SEEK_END);
    PAGINA=le_no_arvb(arquivo); //le ultima pagina do arquivo para saber o ultimo rrn, assim pegando os valores para os proximos rrns
    SPLIT.PAG2.nroChavesNo=(int)(m/2);
    SPLIT.PAG2.RRNdoNo=PAGINA.RRNdoNo;
    SPLIT.PAG2.RRNdoNo+=1;



    
    if (eh_raiz==1) //caso de split de raiz
    {
        SPLIT.PAG3.C[0]=Cs[(int)(m/2)];     //chave promovida
        SPLIT.PAG3.P[0]=SPLIT.PAG1.RRNdoNo; //ponteiro a esquerda aponta pra pagina anterior
        SPLIT.PAG3.Pr[0]=Prs[(int)(m/2)];   //byteoffset referente a chave promovida
        SPLIT.PAG3.nroChavesNo=1;           //no contem apenas uma chave
        SPLIT.PAG3.folha='0';               //no nao eh folha
        SPLIT.PAG3.RRNdoNo=PAGINA.RRNdoNo;  //rrn da pagina eh o da pagina atual+2
        SPLIT.PAG3.RRNdoNo+=2;
        for(i=1;i<m;i++){                   //zerando o resto
            SPLIT.PAG3.P[i]=-1;
            if(i!=m-1){
                SPLIT.PAG3.C[i]=-1;
                SPLIT.PAG3.Pr[i]=-1;
            }
        }
        SPLIT.PAG3.P[1]=SPLIT.PAG2.RRNdoNo; //ponteiro a direita aponta pra pagina que foi criada

        if (eh_folha==1)                    //dependendo se a pagina do split for folha ou nao determina se o irmao eh folha ou n tambem
        {
            SPLIT.PAG2.folha='1';
        }
        if(eh_folha==0){
            SPLIT.PAG2.folha='0';
        }
        

    }
    else if (eh_raiz==0)    //caso de split abaixo da raiz
    {
        SPLIT.C_promovido=Cs[(int)(m/2)];
        SPLIT.P_promovido=SPLIT.PAG2.RRNdoNo;
        SPLIT.Pr_promovido=Prs[(int)(m/2)];
        if (eh_folha==1)
        {
            SPLIT.PAG2.folha='1';
        }
        if(eh_folha==0)
        {
            SPLIT.PAG2.folha='0';
        }
    }

    return SPLIT;
}

// P1 C1 Pr1 | P2 C2 Pr2 | P3 C3 Pr3 | P4 C4 Pr4 | P5 

// 0 1 | 2 3 4 -> chave 2 sobe, esquerda aponta pra pagina antiga e direita aponta pra pagina nova

void print(No PAGINA){

    printf("%c ",PAGINA.folha);
    printf("%d ",PAGINA.nroChavesNo);
    printf("%d ",PAGINA.RRNdoNo);

    for(int i=0;i<m;i++){
        printf("%d ",PAGINA.P[i]);

        if(i!=m-1){
            printf("%ld ",PAGINA.C[i]);
            printf("%ld ",PAGINA.Pr[i]);

        }
    }
    printf("\n");
}