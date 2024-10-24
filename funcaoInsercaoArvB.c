#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

//funcao que cria a primeira raiz da arvore para que as insercoes possam comecar
//nota-se que o diferente dela eh que ela eh uma raiz que tambem eh folha por ser a primeira pagina feita da arvore
void cria_raiz(FILE *arqindices){
    No NO;
    int i;
    //'zera' todos os dados do primeiro no da criacao da arvore
    NO.folha='1';
    NO.nroChavesNo=0;
    NO.RRNdoNo=0;

    //colocando -1 nas chaves,ponteiros e offsets
    for(i=0;i<m;i++){
        NO.P[i]=-1;
        if(i!=m-1){ //impede que a modificacao passe do tamanho do vetor
            NO.C[i]=-1;
            NO.Pr[i]=-1;
        }
    }
    //escreve a primeira pagina zerada na arvore
    escreve_no_arvb(arqindices,NO);
}

//a funcao insere_chave eh uma funcao recursiva que realiza a insercao de chaves no arquivo de indices em arvore B
//a recursao funciona da seguinte forma:
//a funcao de pesquisa eh utilizada em busca da chave que se deseja inserir
//o retorno da pesquisa sempre vai ate o caminho das folhas, eventualmente chegando no rrn_atual=-1. (supondo que a chave ainda nao tenha sido inserida)
//a cada pesquisa a funcao insere_chave eh chamada novamente de forma recursiva porem agora 
//com a pagina de insercao sendo a de retorno da pesquisa anterior

//o inicio dos retornos da recursao se da quando a pesquisa chegar ate o rrn_atual=-1, iniciando o processo de cascateamento de retorno com a chave
//que se desejava inserir inicialmente

//durante o cascateamento as tentativas de insercoes sao feitas nas respectivas paginas atuais onde se esta, as insercoes podem ocorrer
//normalmente quando tem espaco na pagina, assim retornando o resto das chamadas sem inserir nada a mais
//porem tambem pode ocorrer o split, que eh quando, durante o cascateamento, tenta-se inserir uma chave em uma pagina cheia
//esse processo possui uma funcao especifica mais a frente

Pesquisa insere_chave(long chave,long byteoffset,FILE *arquivo,int rrn_atual,int raiz_original){

    int nova_raiz,rrn_pesquisa;      //variavel que sera usada para atualizar a raiz caso ocorra um split numa raiz
    Pesquisa PESQUISA;  //structs auxiliares de pesquisa, leitura de pagina e split
    No PAGINA;
    Split SPLIT;

    PESQUISA.nova_pagina=0; //inicializa o retorno de novas paginas inseridas

    //INICIO DA RECURSAO
    if (rrn_atual!=-1 || PESQUISA.encontrado==1)        
    {   
        PESQUISA=busca_chave(rrn_atual,chave,arquivo);   //faz a pesquisa na pagina atual em busca da chave que se deseja inserir
        rrn_pesquisa=PESQUISA.RRN_pag;
        PESQUISA = insere_chave(chave,byteoffset,arquivo,rrn_pesquisa,raiz_original);   //chama a funcao de insercao novamente no caminho da busca
    }

    //INICIO DA VOLTA/CASCATEAMENTO
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
        PAGINA=le_no_arvb(arquivo);                 //le a pagina

        if(PAGINA.nroChavesNo<=m-2)  //caso haja espaco na pagina atual para inserir a chave, insere ela e retorna promovido=-1 (ok)
        {
            //reordena a pagina com a chave nova apos inserir os dados da promocao na pagina
            PAGINA.C[PAGINA.nroChavesNo]=PESQUISA.chave_promovida;
            PAGINA.P[PAGINA.nroChavesNo+1]=PESQUISA.p_dir_promovido;
            PAGINA.Pr[PAGINA.nroChavesNo]=PESQUISA.byoff_promovido;
            PAGINA.nroChavesNo++;
            PAGINA=reordena_pagina(PAGINA); //reordena a pagina

            fseek(arquivo,93*(1+rrn_atual),SEEK_SET);    //fseek na posicao de reescrita
            escreve_no_arvb(arquivo,PAGINA);             //reescreve a pagina atualizada

            PESQUISA.chave_promovida=-1;                 //sem mais promocoes
            PESQUISA.nova_raiz=raiz_original;            //raiz inalterada
            return PESQUISA;                             //retorna pequisa com promocao=-1  (recursao volta sem fazer mais nada no arquivo)

        }
        else if(PAGINA.nroChavesNo==m-1)    //split (SEM ESPACO NA PAGINA)
        {
            if (rrn_atual==raiz_original)   //SPLIT NA RAIZ
            {
                if(PAGINA.folha=='1')       //RAIZ+FOLHA (primeiro split)
                {  
                    //reordena as paginas de acordo com o 'tipo' de split e retorna as paginas que serao reescritas
                    SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,1,1,arquivo);
                    fseek(arquivo,93,SEEK_SET);             //AJUSTA PARA ESCREVER AS 3 PRIMEIRAS PAGINAS
                    escreve_no_arvb(arquivo,SPLIT.PAG1);    //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE
                    escreve_no_arvb(arquivo,SPLIT.PAG2);    //ESCREVE A PAGINA FOLHA CRIADA
                    escreve_no_arvb(arquivo,SPLIT.PAG3);    //ESCREVE A PAGINA RAIZ NOVA
                }
                else if(PAGINA.folha=='0') //RAIZ APENAS
                {  
                    //reordena as paginas de acordo com o 'tipo' de split e retorna as paginas que serao reescritas
                    SPLIT=ordena_split(PAGINA,PESQUISA.chave_promovida,PESQUISA.byoff_promovido,PESQUISA.p_dir_promovido,1,0,arquivo);
                    fseek(arquivo,93*(1+rrn_atual),SEEK_SET);   //AJUSTA PARA ATUALIZAR A PAGINA
                    escreve_no_arvb(arquivo,SPLIT.PAG1);        //REESCREVE A PAGINA (RAIZ->FOLHA) QUE PERMANECE
                    fseek(arquivo,0,SEEK_END);                  //AJUSTA PARA ESCREVER NO FIM DO ARQUIVO
                    escreve_no_arvb(arquivo,SPLIT.PAG2);        //ESCREVE A PAGINA FOLHA CRIADA
                    escreve_no_arvb(arquivo,SPLIT.PAG3);        //ESCREVE A PAGINA RAIZ NOVA
                }
                PESQUISA.nova_raiz=SPLIT.PAG3.RRNdoNo;  //ATUALIZA O RRN DA RAIZ
                PESQUISA.nova_pagina+=2;                //AUMENTA EM 2 O NRO DE PAGINAS CRIADAS
                PESQUISA.chave_promovida=-1;            //SEM MAIS CHAVES PROMOVIDAS
                return PESQUISA;                        //retorna pequisa com promocao=-1  (recursao volta sem fazer mais nada no arquivo)
            }
            else if (rrn_atual!=raiz_original) //SPLIT FORA DA RAIZ
            {
                //SPLIT FORA DA RAIZ (SENDO OU NAO SENDO FOLHA)
                if (PAGINA.folha == '1')
                {
                    SPLIT = ordena_split(PAGINA, PESQUISA.chave_promovida, PESQUISA.byoff_promovido, PESQUISA.p_dir_promovido, 0, 1, arquivo);
                }
                else if (PAGINA.folha == '0')
                {
                    SPLIT = ordena_split(PAGINA, PESQUISA.chave_promovida, PESQUISA.byoff_promovido, PESQUISA.p_dir_promovido, 0, 0, arquivo);
                }

                fseek(arquivo,93*(1+rrn_atual),SEEK_SET);   //ATUALIZA A PAGINA QUE SOFREU SPLIT
                escreve_no_arvb(arquivo,SPLIT.PAG1);        //REESCREVE A PAGINA QUE PERMANECE
                fseek(arquivo,0,SEEK_END);                  //ESCREVE A PAGINA IRMA NO FIM DO ARQUIVO
                escreve_no_arvb(arquivo,SPLIT.PAG2);        //ESCREVE A PAGINA FOLHA CRIADA

                PESQUISA.chave_promovida=SPLIT.C_promovido; //COLOCA COMO RETORNO A CHAVE QUE FOI PROMOVIDA NO SPLIT
                PESQUISA.byoff_promovido=SPLIT.Pr_promovido;
                PESQUISA.p_dir_promovido=SPLIT.P_promovido;        
                PESQUISA.nova_pagina+=1;                    //AUMENTA A CONTAGEM DO NRO DE PAGINAS NOVAS QUE FORAM CRIADAS
                PESQUISA.nova_raiz=raiz_original;           //MANTEM A RAIZ INALTERADA
                return PESQUISA;                            //RETORNO (a recursao ainda nao acabou aqui, pois ha uma chave promovida)

            }

        }

    }   
}


//funcao usada para reordenar uma pagina que tem no maximo m-1 chaves, ou seja, no caso onde nao ha split
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


//a funcao de split recebe uma pagina cheia e os dados de uma chave que precisa ser inserida mas nao ha espaco, alem de indicacoes se a pagina eh folha/raiz
//a operacao da funcao funciona da seguinte maneira:
//uma "pagina" de ordem m+1 eh criada para que entao a chave anterior que nao cabia seja inserida nesse vetor auxiliar
//o vetor auxiliar eh entao ordenado

//apos a ordenacao do vetor os dados dele sao separados em 3 partes
//a primeira parte eh a PAG1 (pagina que sofreu o split e permanecera no mesmo lugar do arquivo apos ser atualizada, recebe os dados antes do no promovido)
//a segunda parte eh a PAG2 (pagina irma da pagina que sofreu o split, que recebe os dados depois do no promovido)
//a terceira parte (se o split for na raiz) eh a PAG3 que eh a pagina raiz nova do arquivo, que recebe o no promovido
//se o split nao for em uma raiz entao o codigo retorna apenas os dados promovidos em si para que a recursao possa continuar se propagando em direcao a raiz

//VETOR AUXILIAR ORDENADO:  P1 C1 Pr1 | P2 C2 Pr2 | P3 C3 Pr3 | P4 C4 Pr4 | P5 C5 Pr5 | P6



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
    Ps[m]=pont_dir_p;     //coloca o dado que esta tentando ser inserido para ordenacao dentro dos vetores que serao ordenados
    Cs[m-1]=chave_p;
    Prs[m-1]=byoff_p;

    //inicia a partir de i=j=1 para pular o primeiro ponteiro que nao faz parte da ordenacao
    for (int i = 1; i < m+1; i++) {
        for (int j = 1; j < m+1 - i; j++) {
            //reordena com bubblesort (sim)
            if (Cs[j-1] > Cs[j]){ 

                P_aux=Ps[j];
                Ps[j]=Ps[j+1];
                Ps[j+1]=P_aux;

                if(j<m){            //nao realiza a troca ao chegar no fim das chaves
                    C_aux=Cs[j-1];
                    Cs[j-1]=Cs[j];
                    Cs[j]=C_aux;


                    Pr_aux=Prs[j-1];
                    Prs[j-1]=Prs[j]; 
                    Prs[j]=Pr_aux;
                }
            }


        }
    }

    // coloca os dados nas respectivas paginas resultantes do split, nesse caso sao as duas primeiras chaves da pagina e seus respectivos ponteiros
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

    if((eh_raiz==1 && eh_folha==0) || (eh_raiz==0 && eh_folha==0)){
        SPLIT.PAG1.P[2] = Ps[2];
    }

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
    //zerando o resto que sobrou do for anterior
    SPLIT.PAG2.C[2] = -1;
    SPLIT.PAG2.Pr[2] = -1;
    SPLIT.PAG2.P[m-1]=-1;
    SPLIT.PAG2.nroChavesNo=(int)(m/2);
    if (eh_folha == 1)                  //determina se a pagina irma eh folha ou nao dependendo se o irmao eh folha ou nao
    {
        SPLIT.PAG2.folha = '1';
    }
    else if (eh_folha == 0)
    {
        SPLIT.PAG2.folha = '0';
    }

    fseek(arquivo,-93,SEEK_END);
    PAGINA=le_no_arvb(arquivo); //le ultima pagina do arquivo para saber o ultimo rrn, assim pegando os valores para os proximos rrns
    SPLIT.PAG2.RRNdoNo=PAGINA.RRNdoNo;
    SPLIT.PAG2.RRNdoNo+=1;

    //A PARTIR DAQUI SAO OS CASOS DO QUE SERA PROMOVIDO, SE O SPLIT FOI NA RAIZ RETORNA DIRETO A PAGINA QUE SERA INSERIDA
    //SE O SPLIT N FOI NA RAIZ RETORNA APENAS OS DADOS DO QUE FOI PROMOVIDO
    if (eh_raiz==1) //caso de split de raiz
    {

        SPLIT.PAG3.C[0] = Cs[(int)(m / 2)];   // chave promovida                                       C[0]
        SPLIT.PAG3.P[0] = SPLIT.PAG1.RRNdoNo; // ponteiro a esquerda aponta pra pagina anterior        P[0]
        SPLIT.PAG3.Pr[0] = Prs[(int)(m / 2)]; // byteoffset referente a chave promovida                C[0]
        SPLIT.PAG3.nroChavesNo = 1;           // pagina contem apenas uma chave
        SPLIT.PAG3.folha = '0';               // pagina nao eh folha
        SPLIT.PAG3.RRNdoNo = PAGINA.RRNdoNo;  // rrn da pagina eh o do fim do arquivo+2
        SPLIT.PAG3.RRNdoNo += 2;
        for (i = 1; i < m; i++)
        { //'zerando' o resto da pagina
            SPLIT.PAG3.P[i] = -1;
            if (i != m - 1)
            {
                SPLIT.PAG3.C[i] = -1;
                SPLIT.PAG3.Pr[i] = -1;
            }
        }
        SPLIT.PAG3.P[1] = SPLIT.PAG2.RRNdoNo; // ponteiro a direita aponta pra pagina que foi criada   P[1]


    }
    else if (eh_raiz==0)    //caso de split abaixo da raiz
    {
        SPLIT.C_promovido=Cs[(int)(m/2)];       //CHAVE PROMOVIDA
        SPLIT.P_promovido=SPLIT.PAG2.RRNdoNo;   //PONTEIRO PARA A PAGINA FILHO DA DIREITA DO QUE FOI PROMOVIDO
        SPLIT.Pr_promovido=Prs[(int)(m/2)];     //BYOFF PROMOVIDO

    }
    
    return SPLIT;   //RETORNA AS INFORMACOES DO SPLIT
}
