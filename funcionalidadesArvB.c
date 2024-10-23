#include "./funcoesArvB.h"
#include "./funcoesCriadas.h"

//FUNCAO 8
//essa funcao realiza a pesquisa de um registro com base no nome da especie desejada
//para isso a pesquisa eh feita em um arquivo de indices em formato arvore-b, assim achando a localizacao
//do registro no arquivo de dados e exibindo esse dado

//a principal funcionalidade desse programa esta na funcao busca_chave(), que se encontra em um loop while
//realizado ate que a busca chegue ou no "fim" da arvore ou na chave desejada

void pesquisa(){
    int rrn_pesquisa;       //nro do rrn da pagina em que uma determinada pesquisa esta sendo feita
    char nome_arqdados[31];     //nomes dos arquivos a serem abertos
    char nome_arqindices[31];

    //variaveis de cabecalho e de dado de cada tipo de arquivo
    cabecalho CAB_DADOS;
    dados     DADO;
    CabecalhoArvB CAB_ARVB;
    No            NO;

    //variaveis que sao usadas para fazer a pesquisa
    Pesquisa PESQUISA;
    char nome[51];
    char campo[21];
    long chave;

    FILE *arquivodados; 
    FILE *arquivoindices;

    scanf("%s",nome_arqdados);
    scanf("%s",nome_arqindices);

    arquivodados = fopen(nome_arqdados, "rb");       //abre o arquivo de dados em modo leitura
    arquivoindices = fopen(nome_arqindices,"rb");    //abre o arquivo de indices em modo leitura

    if(arquivodados==NULL || arquivoindices==NULL){  //checa a existencia dos arquivos, se um nao existir retorna erro e fecha um possivel arquivo existente
        if (arquivodados!=NULL)
            fclose(arquivodados);
        if(arquivoindices!=NULL)
            fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }

    CAB_DADOS=le_cabecalho(arquivodados);   //le os cabecalhos de ambos os arquivos
    CAB_ARVB=le_cab_arvb(arquivoindices);

    if(CAB_ARVB.status=='0' || CAB_DADOS.status=='0'){  //checa os status dos arquivos, se um deles estiver inconsistente retorna erro e fecha os arquivos
        fclose(arquivodados);
        fclose(arquivoindices);
        printf(ERRO_PADRAO);
        return;
    }

    scanf("%s",campo);              //pega o campo "nome"
    scan_quote_string(nome);        //pega o nome da especie

    if(checa_nome_campo(campo)!=1 || !strcmp(nome,"")){     //se o campo de pesquisa nao for "nome" nao executa a funcao (ou se o nome for invalido)
        printf(ERRO_PADRAO);
        fclose(arquivodados);
        fclose(arquivoindices);  
        return;
    }

    chave=converteNome(nome);       //cria a chave com o nome da especie

    rrn_pesquisa=CAB_ARVB.noRaiz;   //pesquisa comeca pelo no raiz
    while (rrn_pesquisa!=-1 || PESQUISA.encontrado==1)        //se o no da pesquisa chegar ao fim e nao tiver encontrado a chave, sai do loop
    {
        
        PESQUISA=busca_chave(rrn_pesquisa,chave,arquivoindices);   //faz a pesquisa na pagina atual
        
        if(PESQUISA.encontrado==1)  //se encontrou a chave sai do loop
            break;

        rrn_pesquisa=PESQUISA.RRN_pag;//se nao tiver encontrado na pagina atual pula para a proxima pagina indicada na pesquisa
    }
    fclose(arquivoindices);

    if(rrn_pesquisa==-1){       //se nao tiver encontrado o registro printa que nao encontrou
        printf(ERRO_REGISTRO);
    }

    if(PESQUISA.encontrado==1){ //se tiver encontrado o registro, fseek na posicao dentro do arquivo de dados, le o dado e exibe ele
        fseek(arquivodados,PESQUISA.BYOFF_dado,SEEK_SET);
        DADO=le_registro(arquivodados);                     //as duas funcoes sao do trabalho anterior
        exibe_registro(DADO);
        fclose(arquivodados);                               //fecha o arquivo de dados
    }
    
}

void cria_arq_indices(){
    char nome_arqdados[31];
    char nome_arqindices[31];
    Pesquisa PESQ;

    //variaveis de cabecalho e de dado de cada tipo de arquivo
    cabecalho CAB_DADOS;
    dados     DADO;
    CabecalhoArvB CAB_ARVB;
    No            NO;

    //variaveis do rrn de cada registro do arquivo de dados e para a criacao da chave do dado
    int RRN;
    long byteoffset;
    char *linha;
    char *nome;
    long chave;

    scanf("%s",nome_arqdados);
    scanf("%s",nome_arqindices);

    FILE *arquivodados;
    FILE *arquivoindices;

    arquivodados = fopen(nome_arqdados, "rb");   //abre o arquivo de dados em modo leitura

    if(arquivodados==NULL){  //termina a execucao se o arquivo de dados nao existir
        printf(ERRO_PADRAO);
        return;
    }

    CAB_DADOS = le_cabecalho(arquivodados); //le o cabecalho do arquivo de dados
    fseek(arquivodados,1600,SEEK_SET);      //pula para o inicio dos registros do arquivo de dados

    if(CAB_DADOS.status=='0'){              //se o arquivo estiver inconsistente nao executa a funcao e fecha o arquivo
        fclose(arquivodados);
        printf(ERRO_PADRAO);
        return;
    }
    
    arquivoindices = fopen(nome_arqindices,"wb+");   //cria o arquivo de indices em arvore-b

    CAB_ARVB.status='0';
    CAB_ARVB.noRaiz=-1;
    CAB_ARVB.RRNproxNo=0;
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);//reserva o espaco do cabecalho do arquivo de indices


    RRN=-1;
    //loop de leitura dos registros do arquivo de dados e insercao das chaves no arq de indices


    while(1){

        DADO=le_registro(arquivodados); //le o dado do arquivo de dados e atualiza o RRN relativo a esse dado
        RRN++;                                          
        if(DADO.removido=='2')          //sai do loop de leitura de dados caso chegue no fim do arquivo de dados, usa a variavel DADO.removido como flag dentro da funcao le_registro
            break;
        if(DADO.removido=='1')          //se o dado lido estiver logicamente removido volta para o começo do loop para ler o proximo registro
            continue;

        linha = strdup(DADO.variavel);  // separa o campo do nome
        nome = strsep(&linha, "#");

        chave=converteNome(nome);       //cria a chave com a funcao fornecida

        byteoffset = (long)(1600 + RRN*160);     //calcula o byteoffset relativo ao registro que tera a chave inserida
        //CRIA A ARVORE B
        //se a raiz nao existir ainda cria ela no arquivo
        if(CAB_ARVB.noRaiz==-1){
            fseek(arquivoindices,93,SEEK_SET);
            cria_raiz(arquivoindices);
            CAB_ARVB.noRaiz=0;
            CAB_ARVB.RRNproxNo=1;
            fseek(arquivoindices,0,SEEK_SET);
            escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);
        }

        PESQ=insere_chave(chave,byteoffset,arquivoindices,CAB_ARVB.noRaiz,CAB_ARVB.noRaiz);

        CAB_ARVB.noRaiz=PESQ.nova_raiz;         //atualiza a raiz caso ela tenha sofrido split
        CAB_ARVB.RRNproxNo+=PESQ.nova_pagina;   //aumenta o rrn do proximo no caso tenham ocorrido splits

    }
    fclose(arquivodados);   //fecha o arquivo de dados apos ter terminado de ler tudo dele e feito as insercoes

    //ATUALIZACAO DO CABECALHO DO ARQUIVO DE INDICES
    fseek(arquivoindices,0,SEEK_SET);
    CAB_ARVB.status='1';
    escreve_cabecalho_arvb(arquivoindices,CAB_ARVB);//atualiza o cabecalho do arquivo

    fclose(arquivoindices);  
    binarioNaTela(nome_arqindices); //binario da tela mostrando o resultado
}
